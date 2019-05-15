/*
 * variablesGlobales.h
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#ifndef VARIABLESGLOBALES_H_
#define VARIABLESGLOBALES_H_

#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

typedef struct definicionConfiguracionDelFS {
	int puertoEscucha;
	char* puntoDeMontaje;
	int retardo; //de cada operacion realizada, solo accesible/modificable con funcion
	int sizeValue; //tamaño maximo de un value en bytes
	int tiempoDump; //cada cuanto se realiza el proceso dump, solo accesible/modificable con funcion
} t_configuracionDelFS;

typedef struct definicionMetadataDeTabla {
	int particiones;
	char* consistencia;
	int tiempoDeCompactacion;
} t_metadataDeLaTabla;

typedef struct definicionMetadataDelFS {
	int blockSize;
	int blocks;
	char* magicNumber;
} t_metadataDelFS;

typedef struct definicionNodoDeLaMemTable {
	t_list* listaDeDatosDeLaTabla;
	char* nombreDeLaTabla;
} t_nodoDeLaMemTable;
typedef t_nodoDeLaMemTable* tp_nodoDeLaMemTable;

typedef struct definicionNodoDeLaTabla {
	uint16_t key;
	char* value;
	unsigned timeStamp;
} t_nodoDeLaTabla;
typedef t_nodoDeLaTabla* tp_nodoDeLaTabla;

extern t_metadataDelFS metadataDelFS;
extern t_configuracionDelFS configuracionDelFS;
extern t_log* LOGGERFS;
extern char* directorioConLaMetadata;
extern char* archivoDeBitmap;
extern char* archivoDeLaMetadata;
extern char* pathDeMontajeDelPrograma;
extern pthread_t threadConsola, threadCompactador;
extern pthread_mutex_t mutexVariableTiempoDump, mutexVariableRetardo, mutexBitmap;
extern t_bitarray *bitmap;
extern int sizeDelBitmap;
extern char * srcMmap;
extern char * bufferArchivo;
extern pthread_t threadServer;
extern t_list* memTable;

int inicializarVariablesGlobales();
void liberarRecursos();

#endif /* VARIABLESGLOBALES_H_ */
