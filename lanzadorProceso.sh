#!/bin/bash

echo "Elegí el proceso que vas a correr"
echo "1: LFS, 2: Memoria, 3:Kernel, 4: Salir"
read seleccion
if [ $seleccion == 1 ]; then
	echo "Lanzando Lissandra filesystem.."
	./tp-2019-1c-EndGame/liss/Debug/liss
elif [ $seleccion == 2 ]; then
	echo "Lanzando Memoria.."
	echo "Elegí la conf necesaria"
	echo "Prueba mínima: 1, no hay otro"
	read prueba
	if [ $prueba == 1 ]; then
		./tp-2019-1c-EndGame/Memoria/Debug/Memoria /home/utnso/tp-2019-1c-EndGame/Memoria/memoriaPruebaMin.config
	fi
elif [ $seleccion == 3 ]; then
	echo "Lanzando Kernel.."
	./tp-2019-1c-EndGame/Kernel/Debug/Kernel
else 
	exit
fi
