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
char* directorioConLaMetadata;//el directorio
char* archivoDeBitmap;
char* archivoDeLaMetadata;//el archivo
char* pathDeMontajeDelPrograma;
pthread_t threadConsola, threadCompactador;
pthread_mutex_t mutexVariableTiempoDump, mutexVariableRetardo, mutexBitmap;
t_bitarray *bitmap;
int sizeDelBitmap;
char * srcMmap;
char * bufferArchivo;
t_list* memTable;

pthread_t threadServer;//thread para el server de lissandra


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
	if(pthread_mutex_init(&mutexBitmap, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexBitmap");
		return EXIT_FAILURE;
		}
	bitmap=NULL;
	sizeDelBitmap=-1;
	bufferArchivo=NULL;
	archivoDeBitmap=NULL;
	archivoDeLaMetadata=NULL;
	memTable=list_create();;
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
	free(archivoDeBitmap);
	free(archivoDeLaMetadata);
	bajarADiscoBitmap();
	log_info(LOGGERFS,"Destruyendo el bitarray");
	bitarray_destroy(bitmap);
	//free(srcMmap);
	free(bufferArchivo);
	list_destroy(memTable);
	log_destroy(LOGGERFS);
	printf("Memoria liberada, programa finalizado\n");
	return;
}
