#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

static struct sembuf buf;

int SYG_K2 = 0;

void podnies(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1)
    {
        if(errno == EINTR)
        {
        }
        else
        {
            perror("Podnoszenie semafora K2");
            exit(1);
        }
    }
}
void opusc(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1)
    {
        if(errno == EINTR)
        {
        }
        else
        {
            perror("Opuszczenie semafora K2");
            exit(1);
        }
    }
}

void handler_kid(int sig)
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
    
    if(sig == 10 && SYG_K2 == 0)
    {
	printf("Wstrzymanie przekazywania danych \n");
	printf("Do ponownego uruchomienia przekazywania wyslij sygnal 12 \n");
	
	SYG_K2 = 1;
	
	/*sleep(1);
	kill(pid[1], 10);
	
	sleep(1);
	kill(pid[2], 10);*/
	
	sleep(1);
	kill(pid[1], 23);
	
	sleep(1);
	kill(pid[2], 23);
	
	/*while(1)
	{
	    signal(12, handler_kid);
	    sleep(1);
	}*/
	
    }
    else if(sig == 12 && SYG_K2 == 1)
    {
	SYG_K2 = 0;
	sleep(1);
	kill(pid[1], 12);
	
	sleep(1);
	kill(pid[2], 12);
	
	printf("Wznawianie przekazywania danych: \n");
    }
    else if(sig == 14)
    {
	printf("Koniec dzialania programow \n");
	kill(pid[0], 18);
    }
    else if(sig == 23)
    {
	SYG_K2 = 1;
    }
}

int main()
{
    int shmid, semid;
    int *i;
    //ZAPISYWANIE PID OD PLIKU
    FILE *plik;
	plik = fopen("./PID", "a+");
	if(plik == NULL)
	{
	    printf("BLAD PLIKU");
	    exit(1);
	}
	fprintf(plik, "%d\n", getpid());
	fclose(plik);
    semid = semget(45000, 2, 0600); // Podłączenie do semaforów
    if(semid == -1)
    {
        printf("BLAD SEMGET K2");
        exit(1);
    }

    shmid = shmget(45000, 10*sizeof(int), 0600); // podłączenie do pamięci współdzielonej
    if(shmid == -1)
    {
        printf("BLAD SHMGET K2");
        exit(1);
    }
    
    i = shmat(shmid, NULL, 0); // Podłączenie zmiennej do której przekazywana jest liczba znaków
	if(i == NULL)
	{
		printf("BLAD PODLACZENIA DO PAMIECI K2");
		exit(1);
	}
    
    while(1)
    {
	while(SYG_K2 == 0)
	{
	    signal(14, handler_kid);
	    signal(10, handler_kid);
	    signal(12, handler_kid);
	    signal(23, handler_kid);
	    if(SYG_K2 == 1) break;
	    opusc(semid, 1);
	    printf("K1 Odczytal: %d \n", *i);
	    podnies(semid, 0);
	}
	while(SYG_K2 == 1)
	{
	    signal(14, handler_kid);
	    signal(10, handler_kid);
	    signal(12, handler_kid);
	    signal(23, handler_kid);
	    if(SYG_K2 == 0) break;
	    system("clear");
	    printf("PRZESYLANIE DANYCH WSTRZYMANE");
	    fflush(stdout);
	    sleep(1);
	}
    }
    
    return 0;
}
