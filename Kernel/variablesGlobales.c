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
pthread_t threadRequest;
pthread_t threadPCP;
t_list* listaNew;
t_list* listaReady;
t_list* listaExec;
t_list* listaExit;
t_list* listaMemConectadas;
t_list* listaTablasCreadas;
pthread_mutex_t mutex_New;
pthread_mutex_t mutex_Ready;
pthread_mutex_t mutex_Exec;
pthread_mutex_t mutex_Exit;
pthread_mutex_t mutex_MemConectadas;
pthread_mutex_t mutexDePausaDePlanificacion;
int quantum;

int inicializarVariablesGlobales(){


	return EXIT_SUCCESS;
}

int liberarRecursos(){

	return EXIT_SUCCESS;
}

