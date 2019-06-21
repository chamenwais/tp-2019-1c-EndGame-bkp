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
t_list* listaEC;
t_list* listaSC;
t_list* listaHC;
pthread_mutex_t mutex_New;
pthread_mutex_t mutex_Ready;
pthread_mutex_t mutex_Exec;
pthread_mutex_t mutex_Exit;
pthread_mutex_t mutex_MemConectadas;
pthread_mutex_t mutex_EC;
pthread_mutex_t mutex_HC;
pthread_mutex_t mutex_SC;

sem_t NEW;
sem_t READY;

int quantum;
int retardo;
int path_api = 0;

int inicializarVariablesGlobales(){

	return EXIT_SUCCESS;
}

void terminar_programa(int codigo_finalizacion){
	logger(escribir_loguear, l_warning,"Se va a finalizar el kernel.");
	log_destroy(LOG_KERNEL);
	config_destroy(configKernel);
	//destruirListas();
	apagar_semaforos();

	exit(codigo_finalizacion);
}

