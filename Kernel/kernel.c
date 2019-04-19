/*
 * kernel.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "kernel.h"

int main(int argc,char** argv) {
	if(inicializarVariablesGlobales()==EXIT_SUCCESS){
	inicializarLogKernel();
	if(levantarConfiguracionInicialDelKernel()==EXIT_SUCCESS){
	int socket_memoria = conectarse_con_memoria();
	if(lanzarConsola()==EXIT_SUCCESS){
	if(esperarAQueTermineLaConsola()==EXIT_FAILURE){
		liberarRecursos();
	}}}}
	return EXIT_SUCCESS;
}
