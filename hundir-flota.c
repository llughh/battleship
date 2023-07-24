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

int tamanioTablero; // Variable global para almacenar el tamaño del tablero


void realizarDisparos(char tableroPropio[tamanioTablero][tamanioTablero], char tableroContrincante[tamanioTablero][tamanioTablero], sem_t *semaphore) 
{
    // Generar una semilla aleatoria única basada en el ID del proceso hijo
    unsigned int seed = getpid()*10000;
    srand(seed);

    int juegoAcabado = 0;
    int disparoExitoso = 0;
    int direccionDisparo = 1;  // 1: derecha, 2: abajo, 3: izquierda, 4: arriba
    // Comprobar si es posible estas direcciones de disparo
    int posibleX= 0;
    int posibleY = 0;
    
    int x, y;  // Coordenadas del disparo
    
    // Contabilizar las veces de todados los barcos
    int portavionesTocado = 0;	// 4 unidades
    int bombarderosTocado = 0;	// 3 unidades
    int fragatasTocado = 0;		// 2 unidades
    int barcosHundidos = 0;		// 3 barcos
    
    char* estadoBarco = "";		// Tocado, Hundido
    
    // Saber el ultimo movimiento de este proceso
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
	FILE *archivoDisparos = fopen("disparos.txt", "r+");
	while (fscanf(archivoDisparos, "%d:%d,%d:%s\n", &pid, &ultimoX, &ultimoY, ultimoResultado) == 4) {
	    
	    if (strcmp(ultimoResultado, "WINS") == 0) {
	        printf("\n%d GAME OVER\n", getpid());
	        
	    	// Escribir GAME OVER en disparos.txt
	        if (archivoDisparos != NULL) {
		    fprintf(archivoDisparos, "%d:-1,-1:GAME OVER\n", getpid());
	        }
	        // Desbloquear el semáforo para escritura
		sem_post(semaphore);
	         
	        // Se para este proceso
		exit(0);
	    }

	    if (pid == getpid()) {
		ultimoPID = pid;
		x = ultimoX;
		y = ultimoY;
		strcpy(resultado, ultimoResultado);
	    }
	}
	fclose(archivoDisparos);
	
	//printf("\nAnterior disparo: (%d, %d), %s", x, y, resultado);
	
	// Si el disparo anterior es tocado entonces se dispara hacia los lados
	if (strcmp(resultado, "TOCADO") == 0) {
		direccionDisparo = 1;
		disparoExitoso = 0;
		
		while (disparoExitoso == 0) {
		    // Hacia la derecha
		    if (direccionDisparo == 1) {
		        posibleX = x;
		        posibleY = y + 1;
		    // Hacia abajo
		    } else if (direccionDisparo == 2) {
			posibleX = x + 1;
			posibleY = y;
		    // Hacia la izquierda
		    } else if (direccionDisparo == 3) {
			posibleX = x;
			posibleY = y -1;
		    // Hacia arriba
		    } else if (direccionDisparo == 4) {
			posibleX = x - 1;
			posibleY = y;
		    } else {
			direccionDisparo = 1;
		    }
		    
		    // Si las coordenadas están fuera del tablero, Cambiamos la dirección del siguiente disparo
		    if (posibleX < 0 || posibleX >= tamanioTablero || posibleY < 0 || posibleY >= tamanioTablero) {
		    	direccionDisparo++;
		    	
		    } else {
		        // Se sale del bucle
		    	disparoExitoso = 1;
		    	
		    	x = posibleX;
		    	y = posibleY;
		    }
		}
	// Si el disparo anterior es agua o hundido el siguiente disparo es aleatorio
	} else {
	    x = rand() % tamanioTablero;
	    y = rand() % tamanioTablero;
	}
	
	
	// Mientras las coordenadas hayan sido elegidas con anterioridad, no se eligirán.
	while (tableroContrincante[x][y] == '-' || tableroContrincante[x][y] == 'X') {
	    // Coordenadas Aleatorias
	    x = rand() % tamanioTablero;
	    y = rand() % tamanioTablero;
	}
	

	// Verificar si el disparo fue exitoso en una fragata
	if (tableroContrincante[x][y] == '2') {
	    // Marcamos el disparo como exitoso en el tablero del contrincante
	    tableroContrincante[x][y] = 'X';
	    fragatasTocado++;
	    
	    if (fragatasTocado == 2) {
	    	estadoBarco = "HUNDIDO";
	    	barcosHundidos++;
	    } else {
	    	estadoBarco = "TOCADO";
	    }
	    
	    // Escribir el disparo en el archivo de disparos
	    FILE *archivoDisparos = fopen("disparos.txt", "a");
	    if (archivoDisparos != NULL) {
		fprintf(archivoDisparos, "%d:%d,%d:%s\n", getpid(), x, y, estadoBarco);
		fclose(archivoDisparos);
	    }
	    
	// Disparo en bombardero
	} else if (tableroContrincante[x][y] == '3') {
	    // Marcamos el disparo como exitoso en el tablero del contrincante
	    tableroContrincante[x][y] = 'X';
	    bombarderosTocado++;
	    
	    if (bombarderosTocado == 3) {
	    	estadoBarco = "HUNDIDO";
	    	barcosHundidos++;
	    } else {
	    	estadoBarco = "TOCADO";
	    }
	    
	    // Escribir el disparo en el archivo de disparos
	    FILE *archivoDisparos = fopen("disparos.txt", "a");
	    if (archivoDisparos != NULL) {
		fprintf(archivoDisparos, "%d:%d,%d:%s\n", getpid(), x, y, estadoBarco);
		fclose(archivoDisparos);
	    }
	
	// Disparo en portaaviones
	} else if (tableroContrincante[x][y] == '4') {
	    // Marcamos el disparo como exitoso en el tablero del contrincante
	    tableroContrincante[x][y] = 'X';
	    portavionesTocado++;
	    	
	    
	    if (portavionesTocado == 4) {
	    	estadoBarco = "HUNDIDO";
	    	barcosHundidos++;
	    } else {
	    	estadoBarco = "TOCADO";
	    }
	    
	    // Escribir el disparo en el archivo de disparos
	    FILE *archivoDisparos = fopen("disparos.txt", "a");
	    if (archivoDisparos != NULL) {
		fprintf(archivoDisparos, "%d:%d,%d:%s\n", getpid(), x, y, estadoBarco);
		fclose(archivoDisparos);
	    }
	
	// Disparo en agua
	} else {
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
	if (barcosHundidos == 3) {
		printf("\n%d WINS\n", getpid());
		
		// Escribir el ganador en el archivo
	        FILE *archivoDisparos = fopen("disparos.txt", "a");
	        if (archivoDisparos != NULL) {
		    fprintf(archivoDisparos, "%d:-1,-1:WINS\n", getpid());
		    fclose(archivoDisparos);
	        }
		juegoAcabado = 1;
		
		// Desbloquear el semáforo
		sem_post(semaphore);
		 
		// Se para este proceso
		exit(0);
	}
	
	// Desbloquear el semáforo para escritura
	sem_post(semaphore);

	// Enseñar al usuario el tablero del contricante
	printf("\nPID %d:\n", getpid());
	for (int i = 0; i < tamanioTablero; i++) {
		for (int j = 0; j < tamanioTablero; j++) {
		    printf("%c", tableroContrincante[i][j]);
		}
	printf("\n");
	}

	// Esperar al menos 2 segundos antes del próximo disparo
	sleep(2);
    }
}



void crearCopiaBatalla(pid_t hijo1, pid_t hijo2) {
    // Obtener la fecha y hora actual
    time_t tiempoActual;
    time(&tiempoActual);
    struct tm* fechaHora = localtime(&tiempoActual);

    // Crear el nombre del archivo con el formato deseado
    char nombreArchivo[100];
    sprintf(nombreArchivo, "batalla_%d_vs_%d.%04d%02d%02d_%02d%02d.txt",
            hijo1, hijo2,
            fechaHora->tm_year + 1900, fechaHora->tm_mon + 1, fechaHora->tm_mday,
            fechaHora->tm_hour, fechaHora->tm_min);

    // Copiar el archivo de la batalla
    FILE* archivoBatallaOriginal = fopen("disparos.txt", "r");
    FILE* archivoBatallaCopia = fopen(nombreArchivo, "w");
    // Código para copiar el contenido del archivo original al archivo de copia

    // Cerrar los archivos después de la copia
    fclose(archivoBatallaOriginal);
    fclose(archivoBatallaCopia);
}



void cargarTablero(const char* archivo, char tablero[tamanioTablero][tamanioTablero]) 
{

    FILE* archivoTablero = fopen(archivo, "r");
    if (archivoTablero == NULL) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    for (int i = 0; i < tamanioTablero; i++) {
        for (int j = 0; j < tamanioTablero; j++) {
            fscanf(archivoTablero, " %c", &tablero[i][j]);
        }
    }

    fclose(archivoTablero);
}



void limpiarArchivo (const char* archivo){
	// Se limpia el archivo de disparos.txt
	FILE* archivoDisparos = fopen(archivo, "w");
	if (archivoDisparos == NULL) {
	    printf("Error al abrir el archivo para limpiarlo.\n");
	    return;
	}
	fclose(archivoDisparos);  // Cierra el archivo después de sobrescribirlo
}



int obtenerTamanioTablero() {
	FILE *archivoTablero = fopen("tablero_jugador1.txt", "r");
	if (archivoTablero == NULL) {
		printf("Error al abrir el archivo tablero.txt\n");
		return 0;
	}

	tamanioTablero = 0;
	char c;

	// Determinar el número de filas contando los saltos de línea
	while ((c = fgetc(archivoTablero)) != EOF) {
	    if (c == '\n') {
		tamanioTablero++;
	    }
	}

	fclose(archivoTablero);
    	return tamanioTablero;
}



int main()
{

	// Obtener el tamaño del tablero desde el archivo
	tamanioTablero = obtenerTamanioTablero();
	
	//printf("%d\n", tamanioTablero);
    
	char tableroJugador1[tamanioTablero][tamanioTablero];
	char tableroJugador2[tamanioTablero][tamanioTablero];
	
	// Cargar tableros
	cargarTablero("tablero_jugador1.txt", tableroJugador1);
	cargarTablero("tablero_jugador2.txt", tableroJugador2);
	
	// Se limpia el archivo de disparos.txt
	limpiarArchivo("disparos.txt");
	
	
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
		
		
		return 0;
	}
	
	// Esperar a que ambos procesos hijos terminen
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	
	// Cerrar y eliminar los semáforos
	sem_close(semaphore);
	sem_unlink("disparos_semaphore");
	
	// Creamos una copia de la batalla en otro archivo
	crearCopiaBatalla(pid1, pid2);

	
	return 0;
}
