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
	if(lanzarConsola()==EXIT_SUCCESS){
	if(esperarAQueTermineLaConsola()==EXIT_FAILURE){
		liberarRecursos();
	}}}}
	return EXIT_SUCCESS;
}
