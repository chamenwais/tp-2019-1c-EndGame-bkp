/*
 * variablesGlobales.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "variablesGlobales.h"

t_configuracionDelKernel configKernel;
t_log* LOG_KERNEL;
pthread_t threadConsola;
pthread_t threadPlanif;
pthread_t threadMemo;
t_list* listaNew;
t_list* listaReady;
t_list* listaExec;
t_list* listaExit;
t_list* listaMemConectadas;

int inicializarVariablesGlobales(){


	return EXIT_SUCCESS;
}

int liberarRecursos(){

	return EXIT_SUCCESS;
}

