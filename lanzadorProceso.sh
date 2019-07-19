#!/bin/bash

echo "Elegí el proceso que vas a correr"
echo "1: LFS, 2: Memoria, 3:Kernel, 4: Salir"
read seleccion
if [ $seleccion == 1 ]; then
	echo "Asegurate de tener bien configurado Lissandra filesystem, luego presiona una tecla para lanzarlo"
	read algo
	./tp-2019-1c-EndGame/liss/Debug/liss
elif [ $seleccion == 2 ]; then
	echo "Elegí la configuracion deseada, antes de seguir asegurate de haber configurado las ips"
	echo "Prueba base, memoria 1: elegi 1"
	echo "Prueba base, memoria 2: elegi 2"
	echo "Prueba de Stress, memoria 1: elegi 9"
	echo "Prueba de Stress, memoria 2: elegi 2"
	read prueba
	echo "Lanzando Memoria.."
	if [ $prueba == 1 ]; then
		./tp-2019-1c-EndGame/Memoria/Debug/Memoria /tp-2019-1c-EndGame/Memoria/memoria1PruebaBase.config
	fi
	if [ $prueba == 2 ]; then
		./tp-2019-1c-EndGame/Memoria/Debug/Memoria /tp-2019-1c-EndGame/Memoria/memoria2PruebaBase.config
	fi
	if [ $prueba == 9 ]; then
		./tp-2019-1c-EndGame/Memoria/Debug/Memoria /tp-2019-1c-EndGame/Memoria/memoria1PruebaBase.config
	fi
	if [ $prueba == 2 ]; then
		./tp-2019-1c-EndGame/Memoria/Debug/Memoria /tp-2019-1c-EndGame/Memoria/memoria2PruebaBase.config
	fi
	else 
		exit
	fi
elif [ $seleccion == 3 ]; then
	echo "Elegí la configuracion deseada, antes de seguir asegurate de haber configurado las ips"
	echo "Prueba base: elegi 1"
	echo "Prueba de stress: elegi 5"
	read prueba
	if [ $prueba == 1 ]; then
		echo "Lanzando Kernel.."
		./tp-2019-1c-EndGame/Kernel/Debug/Kernel /tp-2019-1c-EndGame/Kernel/kernelPruebaBase.config
	fi
	else 
		exit
	fi
else 
	exit
fi
