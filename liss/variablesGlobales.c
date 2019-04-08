/*
 * variablesGlobales.c
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#include "variablesGlobales.h"

t_metadataDelFS metadataDelFS;
t_configuracionDelFS configuracionDelFS;
t_log* LOGGERFS;
char* directorioConLaMetadata;
char* pathDeMontajeDelPrograma;
pthread_t threadConsola;
pthread_mutex_t mutexVariableTiempoDump, mutexVariableRetardo;

int inicializarVariablesGlobales(){
	configuracionDelFS.puertoEscucha=-1;
	configuracionDelFS.puntoDeMontaje=NULL;
	configuracionDelFS.retardo=-1;
	configuracionDelFS.sizeValue=-1;
	configuracionDelFS.tiempoDump=-1;
	metadataDelFS.blockSize=-1;
	metadataDelFS.blocks=-1;
	metadataDelFS.magicNumber=NULL;
	LOGGERFS=NULL;
	directorioConLaMetadata=NULL;
	pathDeMontajeDelPrograma=NULL;
	if(pthread_mutex_init(&mutexVariableRetardo, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexVariableRetardo");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexVariableTiempoDump, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexVariableTiempoDump");
		return EXIT_FAILURE;
		}
	return EXIT_SUCCESS;
}

void liberarRecursos(){
	log_info(LOGGERFS,"Liberando recursos");
	pthread_mutex_destroy(&mutexVariableRetardo);
	pthread_mutex_destroy(&mutexVariableTiempoDump);
	if(configuracionDelFS.puntoDeMontaje!=NULL)free(configuracionDelFS.puntoDeMontaje);
	if(metadataDelFS.magicNumber!=NULL)free(metadataDelFS.magicNumber);
	free(pathDeMontajeDelPrograma);
	free(directorioConLaMetadata);
	log_destroy(LOGGERFS);
	printf("Memoria liberada, programa finalizado\n");
	return;
}
