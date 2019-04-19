/*
 * variablesGlobales.h
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#ifndef VARIABLESGLOBALES_H_
#define VARIABLESGLOBALES_H_

#include <commons/log.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct definicionConfiguracionDelKernel {
	char* ipMemoria; //de la primera memoria q se conecta
	int puertoMemoria;
	int quantum;
	int multiprocesamiento; //cantidad de scripts corriendo a la vez
	int refreshMetadata;
	int retardoCiclo;
} t_configuracionDelKernel;


extern t_configuracionDelKernel configKernel;
extern t_log* LOG_KERNEL;
extern pthread_t threadConsola;

int inicializarVariablesGlobales();
int liberarRecursos();

#endif /* VARIABLESGLOBALES_H_ */
