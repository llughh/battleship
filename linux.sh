#!/bin/bash

#opciones del menu
a="Editar tamaño tablero"
b="Editar barcos"
c="Compilar juego"
d="Lanzar juego"
e="Comprobar archivos"
f="Salir"

echo -e "Menu"
sleep 2

PS3="Selecciona una opcion: "

function menu_principal(){

select menu in "$a" "$b" "$c" "$d" "$e" "$f"
do 
case $menu in
$a)
tablero1="tablero_jugador1.txt"
tablero2="tablero_jugador2.txt"
clear

echo -e "Abriendo configuracion del tamañano del tablero"
echo -e "Los cambios se veran reflejados en ambos tableros"
echo -e "Por favor que sea cuadrado (5x5 - 6x6 - etc)"
echo -e "Siga el formato de poner 0 y seguido un espacio"
echo -e "Si no cumple esto, el juego no podra funcionar"

sleep 10

if [ -f "$tablero1" ]; then
    nano "$tablero1"
    cp "$tablero1" "$tablero2"
else
    echo "No existe el fichero"
fi

clear
echo "Se cambio el tamaño del tablero"
sleep 2
menu_principal
;;
$b)
tablero1="tablero_jugador1.txt"
tablero2="tablero_jugador2.txt"

clear

echo -e "Abriendo configuracion de los barcos del tablero 1"
echo -e "Inserte 3 barcos - 22 - 333 - 4444"
echo -e "Puede ponerlo en vertical u horizaontal, pero nunca en diagonal"
echo -e "Sustituya donde hay un 0 por valor del barco dependiendo su tamaño"
echo -e "Si no cumple esto, el juego no podra funcionar"


sleep 10

if [ -f "$tablero1" ]; then
    nano "$tablero1"
else
    echo "No existe el fichero"
fi

clear

echo -e "Abriendo configuracion de los barcos del tablero 2"
echo -e "Inserte 3 barcos - 22 - 333 - 4444"
echo -e "Puede ponerlo en vertical u horizaontal, pero nunca en diagonal"
echo -e "Sustituya donde hay un 0 por valor del barco dependiendo su tamaño"
echo -e "Si no cumple esto, el juego no podra funcionar"

sleep 10

if [ -f "$tablero2" ]; then
    nano "$tablero2"
else
    echo "No existe el fichero"
fi

clear
sleep 2
echo -e "Barcos configurados"
sleep 2
menu_principal
;;
$c)
echo -e "Compilando juego"
gcc hundir-flota.c -o hundir-flota -pthread

duration=3 
interval=0.1  
total_width=50

total_iterations=$(bc <<< "$duration / $interval")

print_progress_bar() {
    local current_iteration=$1
    local progress=$((current_iteration * total_width / total_iterations))
    local percent=$((current_iteration * 100 / total_iterations))
    local bar=$(printf "%-${progress}s" | tr ' ' '=')

    printf "\r[%-${total_width}s] %d%%" "$bar" "$percent"
}

for ((iteration = 0; iteration <= total_iterations; iteration++)); do
    print_progress_bar $iteration
    sleep $interval
done
echo
echo -e "Compilacion terminada"
sleep 2
menu_principal
;;
$d)
echo -e "Cargando juego"
duration=3  
interval=0.1  
total_width=50 

total_iterations=$(bc <<< "$duration / $interval")

print_progress_bar() {
    local current_iteration=$1
    local progress=$((current_iteration * total_width / total_iterations))
    local percent=$((current_iteration * 100 / total_iterations))
    local bar=$(printf "%-${progress}s" | tr ' ' '=')

    printf "\r[%-${total_width}s] %d%%" "$bar" "$percent"
}

for ((iteration = 0; iteration <= total_iterations; iteration++)); do
    print_progress_bar $iteration
    sleep $interval
done
sleep 1.2
clear
echo -e "Iniciando juego"

./hundir-flota
sleep 2
menu_principal
;;
$e)
echo -e "Buscando archivos en la carpeta"

duration=3  
interval=0.1  
total_width=50 

total_iterations=$(bc <<< "$duration / $interval")

print_progress_bar() {
    local current_iteration=$1
    local progress=$((current_iteration * total_width / total_iterations))
    local percent=$((current_iteration * 100 / total_iterations))
    local bar=$(printf "%-${progress}s" | tr ' ' '=')

    printf "\r[%-${total_width}s] %d%%" "$bar" "$percent"
}

for ((iteration = 0; iteration <= total_iterations; iteration++)); do
    print_progress_bar $iteration
    sleep $interval
done
echo
echo -e "Los archivos disponibles son: "
ls
sleep 2
menu_principal
;;
$f)
sleep 2
echo -e "Saliendo del programa"
exit
;;
*)
sleep 2
echo -e "No es una opcion valida"
sleep 2
echo -e "Limpiando menu"
sleep 2
clear 
menu_principal
;;

esac
done
}

menu_principal
