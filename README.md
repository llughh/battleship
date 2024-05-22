# Battleship

Este repositorio contiene una implementación del clásico juego de "Hundir la Flota" en C, donde un proceso padre maneja el juego y dos procesos hijos actúan como los atacantes.

## Contenido del Repositorio

- `hundir-flota.c`: Código fuente del juego.
- `disparos.txt`: Registro de los disparos realizados.
- `tablero1` y `tablero2`: Tableros preconfigurados.
- `linux.sh`: Script en bash para compilar y ejecutar el juego.

## Requisitos

- Compilador GCC.
- Sistema operativo Linux.

## Cómo Usar

1. Clona el repositorio:
    ```bash
    git clone https://github.com/llughh/battleship.git
    ```
2. Navega al directorio del proyecto:
    ```bash
    cd battleship
    ```
3. Compila el código:
    ```bash
    ./linux.sh
    ```
4. Ejecuta el juego:
    ```bash
    ./hundir-flota
    ```

## Funcionamiento

- El proceso padre inicializa el juego con tableros preconfigurados.
- Dos procesos hijos realizan disparos aleatorios por turnos.
- Los disparos se registran en `disparos.txt`.

