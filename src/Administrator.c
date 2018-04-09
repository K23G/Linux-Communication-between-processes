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

int main()
{
    int opcja = 0;
    int ktory = 0;
    
    FILE *pidy;
    pidy = fopen("./PID", "r");
    if(pidy == NULL)
    {
	printf("PLIK Z PIDAMI WCZYTANY NIEPOPRAWNIE \n");
	exit(1);
    }
    int pid[4];
    int i;
    for(i =0; i<4; i++)
    {
	fscanf(pidy, "%d", &pid[i]);
    }
    
    fclose(pidy);
    
    while(1)
    {
	opcja = 0;
	ktory = 0;
	system("clear");
	printf("---------MENU---------\n");
	printf("WYBIERZ KTORY PROGRAM CHCESZ OBSLUGIWAC: \n");
	printf("1. Program P1 - PID: %d \n", pid[1]);
	printf("2. Program K1 - PID: %d \n", pid[2]);
	printf("3. Program K2 - PID: %d \n", pid[3]);
	printf("Opcja: ");
	while(opcja != 1 && opcja != 2 && opcja != 3)
	{
	    scanf("%d", &opcja);
	    if(opcja != 1 && opcja != 2 && opcja != 3)
	    {
		printf("Podaj poprawna opcje \n");
	    }
	}
	
	switch(opcja)
	{
	    case 1:
	    {
		system("clear");
		printf("---------MENU SYGNALOW---------\n");
		printf("WYBIERZ JAKI SYGNAL CHCESZ WYSLAC DO PROCESOW(P1): \n");
		printf("1. Sygnal do wstrzymania przesylania danych \n");
		printf("2. Sygnal do wznowienia przesylania danych \n");
		printf("3. Sygnal do zakonczenia dzialania programow \n");
		printf("Opcja: ");
		
		while(ktory != 1 && ktory != 2 && ktory != 3)
		{
		    scanf("%d", &ktory);
		    if(ktory != 1 && ktory != 2 && ktory != 3)
		    {
			printf("Podaj poprawny sygnal \n");
		    }
		}
		switch(ktory)
		{
		    case 1:
		    {
			system("clear");
			printf("Wysylam sygnal do wstrzymania przekazywania do procesu P1 \n");
			kill(pid[1], 10);
			break;
		    }
		    case 2:
		    {
			system("clear");
			printf("Wysylam sygnal do wznowienia przekazywania do procesu P1 \n");
			kill(pid[1], 12);
			break;
		    }
		    case 3:
		    {
			system("clear");
			printf("Wysylam sygnal do zakonczenia dzialania programow do procesu P1 \n");
			kill(pid[1], 14);
			
			printf("KONCZE ROWNIEZ SWOJA PRACE - PROGRAM DO WYSYLANIA SYGNALOW!");
			sleep(1);
			exit(0);
		    }
		}
		break;
	    }
	    case 2:
	    {
		system("clear");
		printf("---------MENU SYGNALOW---------\n");
		printf("WYBIERZ JAKI SYGNAL CHCESZ WYSLAC DO PROCESOW(K1): \n");
		printf("1. Sygnal do wstrzymania przesylania danych \n");
		printf("2. Sygnal do wznowienia przesylania danych \n");
		printf("3. Sygnal do zakonczenia dzialania programow \n");
		printf("Opcja: ");
		
		while(ktory != 1 && ktory != 2 && ktory != 3)
		{
		    scanf("%d", &ktory);
		    if(ktory != 1 && ktory != 2 && ktory != 3)
		    {
			printf("Podaj poprawny sygnal \n");
		    }
		}
		
		switch(ktory)
		{
		    case 1:
		    {
			system("clear");
			printf("Wysylam sygnal do wstrzymania przekazywania do procesu K1 \n");
			kill(pid[2], 10);
			break;
		    }
		    case 2:
		    {
			system("clear");
			printf("Wysylam sygnal do wznowienia przekazywania do procesu K1 \n");
			kill(pid[2], 12);
			break;
		    }
		    case 3:
		    {
			system("clear");
			printf("Wysylam sygnal do zakonczenia dzialania programow do procesu K1 \n");
			kill(pid[2], 14);
			
			printf("KONCZE ROWNIEZ SWOJA PRACE - PROGRAM DO WYSYLANIA SYGNALOW!");
			sleep(1);
			exit(0);
		    }
		}
		break;
	    }
	    case 3:
	    {
		system("clear");
		printf("---------MENU SYGNALOW---------\n");
		printf("WYBIERZ JAKI SYGNAL CHCESZ WYSLAC DO PROCESOW(K2): \n");
		printf("1. Sygnal do wstrzymania przesylania danych \n");
		printf("2. Sygnal do wznowienia przesylania danych \n");
		printf("3. Sygnal do zakonczenia dzialania programow \n");
		printf("Opcja: ");
		
		while(ktory != 1 && ktory != 2 && ktory != 3)
		{
		    scanf("%d", &ktory);
		    if(ktory != 1 && ktory != 2 && ktory != 3)
		    {
			printf("Podaj poprawny sygnal \n");
		    }
		}
		
		switch(ktory)
		{
		    case 1:
		    {
			system("clear");
			printf("Wysylam sygnal do wstrzymania przekazywania do procesu K2 \n");
			kill(pid[3], 10);
			break;
		    }
		    case 2:
		    {
			system("clear");
			printf("Wysylam sygnal do wznowienia przekazywania do procesu K2 \n");
			kill(pid[3], 12);
			break;
		    }
		    case 3:
		    {
			system("clear");
			printf("Wysylam sygnal do zakonczenia dzialania programow do procesu K2 \n");
			kill(pid[3], 14);
			
			printf("KONCZE ROWNIEZ SWOJA PRACE - PROGRAM DO WYSYLANIA SYGNALOW!");
			sleep(1);
			exit(0);
		    }
		}
	    }
	    break;
	}
	
	sleep(1);
    }
    
    
}
