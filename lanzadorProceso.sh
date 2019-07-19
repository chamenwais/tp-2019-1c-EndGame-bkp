#!/bin/bash

echo "Elegí el proceso que vas a correr"
echo "1: LFS, 2: Memoria, 3: Kernel, 4: Salir"
read seleccion
if [ $seleccion == 1 ]; then
	echo "Asegurate de tener bien configurado Lissandra filesystem, luego presiona una tecla para lanzarlo"
	read algo
	./liss/Debug/liss
elif [ $seleccion == 2 ]; then
	echo "Elegí la configuracion deseada, antes de seguir asegurate de haber configurado las ips"
	echo "Prueba base, memoria 1: elegi 1"
	echo "Prueba base, memoria 2: elegi 2"
	echo "Prueba de Stress, memoria 1: elegi 9"
	echo "Prueba de Stress, memoria 2: elegi 10"
	echo "Prueba de Stress, memoria 3: elegi 11"
	echo "Prueba de Stress, memoria 4: elegi 12"
	echo "Prueba de Stress, memoria 5: elegi 13"
	read prueba
	echo "Lanzando Memoria.."
	if [ $prueba == 1 ]; then
		./Memoria/Debug/Memoria ./Memoria/memoria1PruebaBase.config
	fi
	if [ $prueba == 2 ]; then
		./Memoria/Debug/Memoria ./Memoria/memoria2PruebaBase.config
	fi
	if [ $prueba == 9 ]; then
		./Memoria/Debug/Memoria ./Memoria/memoria1PruebaStress.config
	fi
	if [ $prueba == 10 ]; then
		./Memoria/Debug/Memoria ./Memoria/memoria2PruebaStress.config
	fi
	if [ $prueba == 11 ]; then
		./Memoria/Debug/Memoria ./Memoria/memoria3PruebaStress.config
	fi
	if [ $prueba == 12 ]; then
		./Memoria/Debug/Memoria ./Memoria/memoria4PruebaStress.config
	fi
	if [ $prueba == 13 ]; then
		./Memoria/Debug/Memoria ./Memoria/memoria5PruebaStress.config
	fi
elif [ $seleccion == 3 ]; then
	echo "Elegí la configuracion deseada, antes de seguir asegurate de haber configurado las ips"
	echo "Prueba base: elegi 1"
	echo "Prueba de stress: elegi 5"
	read prueba
	if [ $prueba == 1 ]; then
		echo "Lanzando Kernel.."
		./Kernel/Debug/Kernel ./Kernel/kernelPruebaBase.config
	fi
	if [ $prueba == 5 ]; then
		echo "Lanzando Kernel.."
		./Kernel/Debug/Kernel ./Kernel/kernelPruebaStress.config
	fi
else 
	exit
fi
