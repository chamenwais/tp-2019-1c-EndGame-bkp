#!/bin/bash
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make uninstall
sudo make clean
sudo make all
sudo make install
cd ..
cd COM
sudo make clean
sudo make all
sudo make install
sudo apt-get install libreadline6 libreadline6-dev
sleep 15
sudo apt-get install libpthread-stubs0-dev
sleep 15
cd tp-2019-1c-EndGame/liss/Debug
sudo make clean
sudo make all
cd ../..
sudo apt-get install libcunit1-dev libcunit1
sleep 15
cd Memoria/Debug
sudo make clean
sudo make all
cd ../..
cd Kernel/Debug
sudo make clean
sudo make all
cd ../..
git clone https://github.com/sisoputnfrba/1C2019-Scripts-lql-checkpoint.git
sudo mkdir lissandra-checkpoint
cp tp-2019-1c-EndGame/liss/Debug/Metadata lissandra-checkpoint
sudo ./lanzadorProceso.sh
