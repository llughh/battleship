#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>

#define BOARD_SIZE 10


void realizarDisparos(char tableroPropio[BOARD_SIZE][BOARD_SIZE], char tableroContrincante[BOARD_SIZE][BOARD_SIZE], sem_t *semaphore) 
{
    // Inicializamos la semilla para generar números aleatorios
    srand(time(NULL));

    int disparoExitoso = 1;
    int direccionDisparo = 0;  // 1: derecha, 2: abajo, 3: izquierda, 4: arriba
    int x, y;  // Coordenadas del disparo

    while (disparoExitoso == 1) {
    	// Esperar un tiempo aleatorio entre 0 y 10 segundos
        int tiempoEspera = rand() % 10;
        sleep(tiempoEspera);
        
        // Intentar bloquear el semáforo para escritura
        while (sem_trywait(semaphore) != 0) {
            sleep(1);  // Esperar 1 segundo si no se pudo bloquear el semáforo
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

        // Verificar si el disparo fue exitoso
        if (tableroContrincante[x][y] == '1') {
            // Marcamos el disparo como exitoso en el tablero del contrincante
            tableroContrincante[x][y] = 'X';  
            
            direccionDisparo = 1;
            
            printf("Direccion: %d\n", direccionDisparo);
            
            // Continuar con las acciones correspondientes al barco hundido, si aplica
        } else {
      	     disparoExitoso = 0;
      	     direccionDisparo = 0;
      	     
	     // Marcamos el disparo como agua en el tablero del contrincante
            tableroContrincante[x][y] = '-';  
            
            // Escribir el disparo en el archivo de disparos
            FILE *archivoDisparos = fopen("disparos.txt", "a");
            if (archivoDisparos != NULL) {
            	fprintf(archivoDisparos, "Jugador: %d - Fila: %d - Columna: %d\n", getpid(), x, y);
             	fclose(archivoDisparos);
            }

	    // Desbloquear el semáforo para escritura
	    sem_post(semaphore);
	    
	    // Esperar al menos 2 segundos antes del próximo disparo
            sleep(2);
            
        }
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
	
	
	// Crear los semáforos para cada jugador
	sem_t *semaphore_jugador1 = sem_open("disparos_semaphore_jugador1", O_CREAT, 0644, 1);
	sem_t *semaphore_jugador2 = sem_open("disparos_semaphore_jugador2", O_CREAT, 0644, 1);
	if (semaphore_jugador1 == SEM_FAILED || semaphore_jugador2 == SEM_FAILED) {
		printf("Error al crear los semáforos.\n");
		return 1;
	}
	
	
	pid_t pid1, pid2;
	
	// Crear el primer proceso hijo (atacante 1)
	pid1 = fork();
	if (pid1 < 0) {
		printf("Error al crear el primer proceso hijo.\n");
		return 1;
	} else if (pid1 == 0) {
		// Código del primer proceso hijo (atacante 1)
		realizarDisparos(tableroJugador1, tableroJugador2, semaphore_jugador1);
		return 0;
	}
	
	// Crear el segundo proceso hijo (atacante 2)
	pid2 = fork();
	if (pid2 < 0) {
		printf("Error al crear el segundo proceso hijo.\n");
		return 1;
	} else if (pid2 == 0) {
		// Código del segundo proceso hijo (atacante 2)
		realizarDisparos(tableroJugador2, tableroJugador1, semaphore_jugador2);
		
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
			    printf("%c", tableroJugador1[i][j]);
			}
			printf("\n");
		}
		return 0;
	}
	
	
	
	// Cerrar y eliminar los semáforos
	sem_close(semaphore_jugador1);
	sem_close(semaphore_jugador2);
	sem_unlink("disparos_semaphore_jugador1");
	sem_unlink("disparos_semaphore_jugador2");

	
	return 0;
}
