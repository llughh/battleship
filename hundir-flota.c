#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>


#define BOARD_SIZE 10


void realizarDisparos(char tableroPropio[BOARD_SIZE][BOARD_SIZE], char tableroContrincante[BOARD_SIZE][BOARD_SIZE], sem_t *semaphore) 
{
    // Generar una semilla aleatoria única basada en el ID del proceso hijo
    unsigned int seed = getpid()*10000;
    srand(seed);

    int juegoAcabado = 0;
    int disparoExitoso = 1;
    int direccionDisparo = 0;  // 1: derecha, 2: abajo, 3: izquierda, 4: arriba
    int x, y;  // Coordenadas del disparo
    
    // Contabilizar las veces de todados los barcos
    int portavionesTocado = 0;	// 4 unidades
    int bombarderosTocado = 0;	// 3 unidades
    int fragatasTocado = 0;		// 2 unidades
    int barcosHundidos = 0;		// 3 barcos
    
    char* estadoBarco = "";		// Tocado, Hundido
    
    
    int ultimoPID = 0;
    int ultimoX= 0;
    int ultimoY = 0;
    char ultimoResultado[10];
    int pid;
    char resultado[10];
    
    
    while (juegoAcabado == 0) {
    
	// Esperar un tiempo aleatorio de 0 a 10 segundos entre cada disparo
	int tiempoEspera = rand() % 10;
	sleep(tiempoEspera);
	
	// Intentar bloquear el semáforo para escritura
	while (sem_trywait(semaphore) != 0) {
	    sleep(1);  // Esperar 1 segundo si no se pudo bloquear el semáforo
	}
	
	// Se mira cual fue el último disparo de este proceso en el archivo disparos.txt
	FILE *archivoDisparos = fopen("disparos.txt", "r");
	while (!feof(archivoDisparos)) {
	    fscanf(archivoDisparos, "%d:%d,%d:%s\n", &pid, &ultimoX, &ultimoY, ultimoResultado);

	    if (pid == getpid()) {
		ultimoPID = pid;
		x = ultimoX;
		y = ultimoY;
		strcpy(resultado, ultimoResultado);
	    }
	}
    
    	// Se especifican las cordenadas de ataque
	if (direccionDisparo == 0) {
	    // Coordenadas Aleatorias
	    x = rand() % BOARD_SIZE;
	    y = rand() % BOARD_SIZE;
	} else if (direccionDisparo == 1) {
	    // Hacia la derecha
	    y++;
	} else if (direccionDisparo == 2) {
	    // Hacia abajo
	    x++;
	} else if (direccionDisparo == 3) {
	    // Hacia la izquierda
	    y--;
	} else if (direccionDisparo == 4) {
	    // Hacia arriba
	    x--;
	} else {
	    direccionDisparo = 0;
	    continue;
	}

	if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
	    // Si las coordenadas están fuera del tablero, Cambiamos la dirección del siguiente disparo
	    direccionDisparo++;
	    continue;
	}
	
	while (tableroContrincante[x][y] == '-' || tableroContrincante[x][y] == 'X') {
	    // Coordenadas Aleatorias
	    x = rand() % BOARD_SIZE;
	    y = rand() % BOARD_SIZE;
	}
	

	// Verificar si el disparo fue exitoso
	if (tableroContrincante[x][y] == '2') {
	    // Marcamos el disparo como exitoso en el tablero del contrincante
	    tableroContrincante[x][y] = 'X';
	    fragatasTocado++;
	    
	    if (fragatasTocado == 2) {
	    	estadoBarco = "HUNDIDO";
	    	direccionDisparo = 0;	// Siguiente disparo es aleatorio
	    	barcosHundidos++;
	    } else {
	    	estadoBarco = "TOCADO";
	    	direccionDisparo = 1; 	// Siguente desparo hacia la derecha
	    }
	    
	    // Escribir el disparo en el archivo de disparos
	    FILE *archivoDisparos = fopen("disparos.txt", "a");
	    if (archivoDisparos != NULL) {
		fprintf(archivoDisparos, "%d:%d,%d:%s\n", getpid(), x, y, estadoBarco);
		fclose(archivoDisparos);
	    }
	    
	    
	} else if (tableroContrincante[x][y] == '3') {
	    // Marcamos el disparo como exitoso en el tablero del contrincante
	    tableroContrincante[x][y] = 'X';
	    bombarderosTocado++;
	    
	    if (bombarderosTocado == 3) {
	    	estadoBarco = "HUNDIDO";
	    	direccionDisparo = 0;	// Siguiente disparo es aleatorio
	    	barcosHundidos++;
	    } else {
	    	estadoBarco = "TOCADO";
	    	direccionDisparo = 1;	// Siguente desparo hacia la derecha
	    }
	    
	    // Escribir el disparo en el archivo de disparos
	    FILE *archivoDisparos = fopen("disparos.txt", "a");
	    if (archivoDisparos != NULL) {
		fprintf(archivoDisparos, "%d:%d,%d:%s\n", getpid(), x, y, estadoBarco);
		fclose(archivoDisparos);
	    }
	
	
	} else if (tableroContrincante[x][y] == '4') {
	    // Marcamos el disparo como exitoso en el tablero del contrincante
	    tableroContrincante[x][y] = 'X';
	    portavionesTocado++;
	    	
	    
	    if (portavionesTocado == 4) {
	    	estadoBarco = "HUNDIDO";
	    	direccionDisparo = 0; // Siguiente disparo es aleatorio
	    	barcosHundidos++;
	    } else {
	    	estadoBarco = "TOCADO";
	    	direccionDisparo = 1;	// Siguente desparo hacia la derecha
	    }
	    
	    // Escribir el disparo en el archivo de disparos
	    FILE *archivoDisparos = fopen("disparos.txt", "a");
	    if (archivoDisparos != NULL) {
		fprintf(archivoDisparos, "%d:%d,%d:%s\n", getpid(), x, y, estadoBarco);
		fclose(archivoDisparos);
	    }
	
	
	} else {
      	     disparoExitoso = 0;
      	     direccionDisparo = 0;
      	     
	     // Marcamos el disparo como agua en el tablero del contrincante
	    tableroContrincante[x][y] = '-';
	    
	    // Escribir el disparo en el archivo de disparos
	    FILE *archivoDisparos = fopen("disparos.txt", "a");
	    if (archivoDisparos != NULL) {
		fprintf(archivoDisparos, "%d:%d,%d:AGUA\n", getpid(), x, y);
		fclose(archivoDisparos);
	    }

	    
	}
	
	// Si se han hundido todos los barcos, la partida acaba
	if (barcosHundidos == 1) {
		juegoAcabado = 1;
	}
	
	// Desbloquear el semáforo para escritura
	sem_post(semaphore);

	printf("\nPID %d:\n", getpid());
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
		    printf("%c", tableroContrincante[i][j]);
		}
	printf("\n");
	}

	// Esperar al menos 2 segundos antes del próximo disparo
	sleep(2);
    }
}



void cargarTablero(const char* archivo, char tablero[BOARD_SIZE][BOARD_SIZE]) 
{

    FILE* archivoTablero = fopen(archivo, "r");
    if (archivoTablero == NULL) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fscanf(archivoTablero, " %c", &tablero[i][j]);
            //printf("%c", tablero[i][j]);
        }
        //printf("\n");
    }

    fclose(archivoTablero);
}



int main()
{
	char tableroJugador1[BOARD_SIZE][BOARD_SIZE];
	char tableroJugador2[BOARD_SIZE][BOARD_SIZE];
	
	// Cargar tableros
	cargarTablero("tablero_jugador1.txt", tableroJugador1);
	cargarTablero("tablero_jugador2.txt", tableroJugador2);
	
	
	// Crear los semáforos
	sem_t *semaphore = sem_open("disparos_semaphore", O_CREAT, 0644, 1);
	if (semaphore == SEM_FAILED) {
		printf("Error al crear el semáforos.\n");
		return 1;
	}
	
	pid_t pid1, pid2;
	
	// Crear el primer proceso hijo (atacante 1)
	pid1 = fork();
	if (pid1 < 0) {
		printf("Error al crear el primer proceso hijo.\n");
		return 1;
	} else if (pid1 == 0) {
		//sleep(2);
		// Código del primer proceso hijo (atacante 1)
		realizarDisparos(tableroJugador1, tableroJugador2, semaphore);
		
		//printf("\nJugador 2\n");
		//for (int i = 0; i < BOARD_SIZE; i++) {
		//	for (int j = 0; j < BOARD_SIZE; j++) {
		//	    printf("%c", tableroJugador2[i][j]);
		//	}
		//	printf("\n");
		//}
	
		return 0;
	}
	
	// Crear el segundo proceso hijo (atacante 2)
	pid2 = fork();
	if (pid2 < 0) {
		printf("Error al crear el segundo proceso hijo.\n");
		return 1;
	} else if (pid2 == 0) {
		// Código del segundo proceso hijo (atacante 2)
		realizarDisparos(tableroJugador2, tableroJugador1, semaphore);
		
		//printf("\nJugador 1\n");
		//for (int i = 0; i < BOARD_SIZE; i++) {
		//	for (int j = 0; j < BOARD_SIZE; j++) {
		//	    printf("%c", tableroJugador1[i][j]);
		//	}
		//	printf("\n");
		//}
		
		return 0;
	}
	
	// Esperar a que ambos procesos hijos terminen
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	
	// Cerrar y eliminar los semáforos
	sem_close(semaphore);
	sem_unlink("disparos_semaphore");
	
	

	
	return 0;
}
