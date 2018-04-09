#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#include <errno.h>

int SYG = 0;
int SYG_K = 0;

void handler_p1(int sig)
{
    FILE *plik;
    plik = fopen("./PID", "r");
    if(plik == NULL)
    {
	printf("BLAD PLIKU Funkcja HND_P1");
	sleep(2);
	exit(1);
    }
    int pid[4];
    int i;
    for(i =0; i<4; i++)
    {
	fscanf(plik, "%d", &pid[i]);
    }
    fclose(plik);
    
    if(sig == 10 && SYG == 0)
    {
	printf("Wstrzymanie przekazywania danych \n");
	printf("Do ponownego uruchomienia przekazywania wyslij sygnal 12 \n");
	
	SYG = 1;
	
	/*sleep(1);
	kill(pid[2],10);
	
	sleep(1);
	kill(pid[3],10);*/
	
	sleep(1);
	kill(pid[2],23);
	
	sleep(1);
	kill(pid[3],23);
	/*while(1)
	{
	    signal(12, handler_p1);
	    sleep(1);
	}*/
    }
    else if(sig == 12 && SYG == 1)
    {
	
	SYG = 0;
	sleep(1);
	kill(pid[2], 12);
	
	sleep(1);
	kill(pid[3], 12);
	printf("Wznawianie przekazywania danych: \n");
    }
    else if(sig == 14)
    {
	printf("Koniec dzialania programow \n");
	kill(pid[0], 18);
    }
    else if(sig == 23)
    {
	SYG = 1;
    }
}

void handler_kid(int sig)
{
    FILE *plik;
    plik = fopen("./PID", "r");
    if(plik == NULL)
    {
	printf("BLAD PLIKU Funkcja HND_K1");
	sleep(2);
	exit(1);
    }
    int pid[4];
    int i;
    for(i =0; i<4; i++)
    {
	fscanf(plik, "%d", &pid[i]);
    }
    fclose(plik);
    
    if(sig == 10 && SYG_K == 0)
    {
	printf("Wstrzymanie przekazywania danych \n");
	printf("Do ponownego uruchomienia przekazywania wyslij sygnal 12 \n");
	
	
	SYG_K = 1;
	
	/*sleep(1);
	kill(pid[1], 10);
	
	sleep(1);
	kill(pid[3], 10);*/
	
	sleep(1);
	kill(pid[1], 23);
	
	sleep(1);
	kill(pid[3], 23);
	
	
	/*while(1)
	{
	    signal(12, handler_kid);
	    sleep(1);
	}*/
	
    }
    else if(sig == 12 && SYG_K == 1)
    {
	SYG_K = 0;
	
	sleep(1);
	kill(pid[1], 12);
	
	sleep(1);
	kill(pid[3], 12);
	
	
	
	printf("Wznawianie przekazywania danych: \n");
    }
    else if(sig == 14)
    {
	printf("Koniec dzialania programow \n");
	kill(pid[0], 18);
    }
    else if(sig == 23)
    {
	SYG_K = 1;
    } 
}

static struct sembuf buf;

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
            perror("Opuszczenie semafora K2");
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
            perror("Podnoszenie semafora K2");
            exit(1);
        }
    }
}

int main(int argc, char **argv)
{
    int ptab[2];
    int argv0size = strlen(argv[0]);
    
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
    
    int chck = pipe(ptab); // Tworzenie pipe
    if(chck == -1) 
    {
	printf("PIPE UTWORZONY NIEPOMYŚLNIE");
	exit(1);
    }

    if(fork() == 0)
    {
	strncpy(argv[0],"child",argv0size);
	
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
	
	//Podlaczanie do pamieci i semaforow
	int *i;
	int semid, shmid;
	semid = semget(45000, 2, 0600); // Tworzenie semaforów
	if(semid == -1)
	{
	    printf("BLAD SEMGET K1");
	    exit(1);
	}
	shmid = shmget(45000, 10*sizeof(int), 0600); // Tworzenie segmentu pamięci współdzielonej
	if(shmid == -1)
	{
	    printf("BLAD SHMGET K1");
	    exit(1);
	}
    
	i = shmat(shmid, NULL, 0); // Podłączenie bufora pod pamięć współdzieloną
	if(i == NULL)
	{
	    printf("BLAD PODLACZENIA DO PAMIECI K1");
	    exit(1);
	}
	//printf("ID procesu macierzystego: %d \n",getppid());
        //printf("ID procesu potomnego: %d \n",getpid());
        char buff[128];
        int rozmiar = 1;
        close(ptab[1]);
        while(1)
        {
	    while(SYG_K == 0)
	    {
		signal(12, handler_kid);
		signal(10, handler_kid);
		signal(14, handler_kid);
		signal(23, handler_kid);
		if(SYG_K == 1) break;
		read(ptab[0],buff,sizeof(buff));
		//Liczenie liczby znaków:
		for(rozmiar = 0; rozmiar<sizeof(buff); rozmiar++)
		{
		    if(buff[rozmiar] == NULL) break;
		    if(buff[rozmiar] == '\n') break;
		}
		printf("%d: Odebralem: %s Rozmiar: %d\n",getpid(), buff, rozmiar);
	    
		opusc(semid, 0);
		//strcpy(buf,buff);
		i[0] = rozmiar;
		podnies(semid, 1);
	    }
	    while(SYG_K == 1)
	    {
		signal(12, handler_kid);
		signal(10, handler_kid);
		signal(14, handler_kid);
		signal(23, handler_kid);
		if(SYG_K == 0) break;
		system("clear");
		printf("PRZEKAZYWANIE DANYCH WSTRZYMANE");
		fflush(stdout);
		sleep(1);
	    }
	    
        }
    }
    else
    {
	strncpy(argv[0],"parent",argv0size);
    }
    
    // PROCES PRODUCENTA!
    char c[128];
    close(ptab[0]);
    int opcja = 0;
    while(1)
    {
	while(SYG == 0)
	{
	    //MENU:
	    opcja = 0;
	    printf("-------MENU-------\n");
	    printf("Podaj sposob w jaki chcesz przekazac dane: \n 1 - Z klawiatury \n 2 - Z pliku\n Podaj sposob: ");
	
	
	    signal(14, handler_p1);
	    signal(10, handler_p1);
	    signal(23, handler_p1);
	    signal(12, handler_p1);
	    if(SYG == 1) break;
	    scanf("%d", &opcja);
	    if(SYG == 1) break;
	    switch(opcja)
	    {
		default:
		{
		    printf("Podaj poprawna opcje! \n");
		    break;
		}
		case 1:
		{
		    scanf("%s", c);
		    write(ptab[1], c, sizeof(c));
		    break;
		}
		case 2:
		{
		    printf("Podaj sciezke z jakiej chcesz zaladowac plik: ");
		    char src[64];
		    scanf("%s", src);
		    FILE *tekst;
		    tekst = fopen(src, "r");
		    if(tekst == NULL)
		    {
			printf("SPRAWDZ CZY PODALES POPRAWNA SCIEZKE, LUB PLIK NIE ISTNIEJE! \n");
			exit(1);
		    }
		
		    while(fgets(c, 128, tekst))
		    {
			
			write(ptab[1], c, sizeof(c));
			sleep(2);
			//fscanf(tekst, "%s", dodatkowa);
			//if(feof(tekst)) break;
		    }
		    fclose(tekst);
		    break;
		}
	    }
	}
	
	while(SYG == 1)
	{
	    signal(14, handler_p1);
	    signal(10, handler_p1);
	    signal(23, handler_p1);
	    signal(12, handler_p1);
	    system("clear");
	    printf("PRZESYLANIE DANYCH WSTRZYMANE");
	    fflush(stdout);
	    sleep(1);
	}
    }
    
    return 0;
	
}
