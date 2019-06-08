#!/bin/bash
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make uninstall
sudo make clean
sudo make all
sudo make install
cd ..
cd COM
make clean
make all
sudo make install
sudo apt-get install libreadline6 libreadline6-dev
sleep 15
sudo apt-get install libpthread-stubs0-dev
sleep 15
cd tp-2019-1c-EndGame/liss/Debug
make clean
make all
cd ../..
sudo apt-get install libcunit1-dev libcunit1
sleep 15
cd Memoria/Debug
make clean
make all
cd ../..
cd Kernel/Debug
make clean
make all
cd ../..
git clone https://github.com/sisoputnfrba/1C2019-Scripts-lql-checkpoint.git
mkdir lissandra-checkpoint
cp tp-2019-1c-EndGame/liss/Debug/Metadata lissandra-checkpoint
./lanzadorProceso.sh
