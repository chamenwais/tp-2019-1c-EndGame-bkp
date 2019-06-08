#!/bin/bash
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make uninstall
sudo make clean
sudo make all
sudo make install
cd ..
sudo apt-get install libreadline6 libreadline6-dev
cd COM
make clean
make all
sudo make install
cd tp-2019-1c-EndGame/liss/Debug
make clean
make all
cd ../..
sudo apt-get install libcunit1-dev libcunit1-doc libcunit1
cd Memoria/Debug
make clean
make all
cd ../..
cd Kernel/Debug
make clean
make all
cd ../..
