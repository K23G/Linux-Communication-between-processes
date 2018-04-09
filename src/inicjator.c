#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/sem.h>

static struct sembuf buf;

void podnies(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1)
    {
        perror("Podnoszenie semafora P1");
        exit(1);
    }
}
void opusc(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;

    if (semop(semid, &buf, 1) == -1)
    {
        perror("Opuszczenie semafora P2");
        exit(1);
    }
}

void handler_org(int sig)
{
    FILE *plik;
    plik = fopen("./PID", "r");
    if(plik == NULL)
    {
	printf("BLAD PLIKU Funkcja HND_P1");
	exit(1);
    }
    int pid[4];
    int i;
    for(i =0; i<4; i++)
    {
	fscanf(plik, "%d", &pid[i]);
    }
    fclose(plik);
    printf("JESTEM OPID: %d, ODEBRALEM SYGNAL: %d\n", getpid(), sig);
    if(sig == 19)
    {
	kill(pid[1], 18);
	kill(pid[2], 18);
    }
}

int main(int argc, char **argv)
{
    int semid, shmid;
    int *bufor;   
    remove("./PID");
    //ZAPISYWANIE PID OD PLIKU
    FILE *plik;
    plik = fopen("./PID", "w+");
    if(plik == NULL)
    {
	printf("BLAD PLIKU");
	exit(1);
    }
    fprintf(plik, "%d\n", getpid());
    fclose(plik);
    
    semid = semget(45000, 2, IPC_CREAT|0600); // Tworzenie semaforów
    if(semid == -1)
    {
        printf("BLAD SEMGET INI1");
        exit(1);
    }
    if(semctl(semid, 0, SETVAL, (int)1) == -1) // Początkowa wartość semafora (0) = 1, można pisać - do P1
    {
        printf("BLAD NADAWANIA WARTOSCI DLA 0");
        exit(1);
    }
    if(semctl(semid, 1, SETVAL, (int)0) == -1) // Początkowa wartość semafora (1) = 0, nie ma nic do czytania - do K1
    {
        printf("BLAD NADAWANIA WARTOSCI DLA 1");
        exit(1);
    }
    shmid = shmget(45000, 10*sizeof(int), IPC_CREAT|0600); // Tworzenie segmentu pamięci współdzielonej
    if(shmid == -1)
    {
        printf("BLAD SHMGET INI1");
        exit(1);
    }
    bufor = shmat(shmid, NULL, 0); // Podłączenie bufora pod pamięć współdzieloną
    if(bufor == NULL)
    {
	    printf("BLAD PODLACZENIA DO PAMIECI K1");
	    exit(1);
    }
    
    sleep(2);
    
    if(fork() == 0) // Producent oraz K2
    {
        //system("gnome-terminal");
	//execl("./p1k1","p1k1",NULL);
	execlp("/usr/bin/xterm", "xterm", "-e", "./p1k1", NULL);
	//execl("gnome-terminal", "./p1k1", NULL);
    }
    
    sleep(2);
    if(fork() == 0) // K2
    {
	execlp("/usr/bin/xterm", "xterm", "-e", "./k2", NULL);
	//execl("./k2","k2",NULL);
    }
    
    kill(getpid(),19);
    
    int pid[4];
    plik = fopen("./PID", "r");
    if(plik == NULL)
    {
	printf("BLAD PLIKU");
	exit(1);
    }
    
    fscanf(plik, "%d", &pid[0]);
    printf("PID 1: %d ", pid[0]);
    fscanf(plik, "%d", &pid[1]);
    printf("PID 2: %d ", pid[1]);
    fscanf(plik, "%d", &pid[2]);
    printf("PID 3: %d ", pid[2]);
    fscanf(plik, "%d", &pid[3]);
    printf("PID 4: %d ", pid[3]);
    fclose(plik);
    
    kill(pid[3], SIGKILL);
    kill(pid[2], SIGKILL);
    kill(pid[1], SIGKILL);
    
    //Potrzebuję do funkcji shmid i semid)
    
    shmctl(shmid, IPC_RMID, NULL); // Usuwanie pamięci współdzielonej
    semctl(semid, 0, IPC_RMID, NULL); // Usuwanie semaforów
    
    
    return 0;
}
