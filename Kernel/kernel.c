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
	inicializarListas();
	if(levantarConfiguracionInicialDelKernel()==EXIT_SUCCESS){
	configurar_signals();
	int socket_primera_memoria = conectarse_con_memoria(configKernel.ipMemoria, configKernel.puertoMemoria);
	if(lanzarConsola()==EXIT_SUCCESS){
	if(lanzarPlanificador()){//TODO
	if(esperarAQueTermineLaConsola()==EXIT_FAILURE){
		liberarRecursos();
	}}}}}
	return EXIT_SUCCESS;
}
