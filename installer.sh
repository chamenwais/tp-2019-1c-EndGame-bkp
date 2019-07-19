#!/bin/bash
if [! -d "/usr/include/commons/"]; then
	echo "Instalando las commons"
	git clone https://github.com/sisoputnfrba/so-commons-library.git
	cd so-commons-library
	sudo make uninstall
	sudo make clean
	sudo make all
	sudo make install
	cd ..
fi
if [! -d "/usr/include/readline/"]; then
	echo "Instalando readline"
	sudo apt-get install libreadline6 libreadline6-dev
	sleep 10
fi
if [! -f "/usr/include/pthread.h" ]; then
    echo "Instalando pthread"
    sudo apt-get install libpthread-stubs0-dev
	sleep 10
fi
if [! -d "/usr/include/CUnit/"]; then
	echo "Instalando CUnit"
	sudo apt-get install libcunit1-dev libcunit1
	sleep 10
fi
echo "Instalando biblioteca propia COM"
cd COM
sudo make clean
sudo make all
sudo make install
cd ..
echo "Compilando liss"
cd tp-2019-1c-EndGame/liss/Debug
sudo make clean
sudo make all
cd ../..
echo "Compilando Memoria"
cd Memoria/Debug
sudo make clean
sudo make all
cd ../..
echo "Compilando Kernel"
cd Kernel/Debug
sudo make clean
sudo make all
cd ../../..
if [! -d "/1C2019-Scripts-lql-entrega"]; then
	echo "Descargando scripts"
	git clone https://github.com/sisoputnfrba/1C2019-Scripts-lql-checkpoint.git
fi
sudo ./lanzadorProceso.sh
