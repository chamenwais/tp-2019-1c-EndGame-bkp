/*
 * variablesGlobales.h
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#ifndef VARIABLESGLOBALES_H_
#define VARIABLESGLOBALES_H_

#include <commons/log.h>

typedef struct definicionConfiguracionDelFS {
	int puertoEscucha;
	char* puntoDeMontaje;
	int retardo; //de cada operacion realizada
	int sizeValue; //tamaño maximo de un value en bytes
	int tiempoDump; //cada cuanto se realiza el proceso dump
} t_configuracionDelFS;

typedef struct definicionMetadataDelFS {
	int blockSize;
	int blocks;
	char* magicNumber;
} t_metadataDelFS;

extern t_metadataDelFS metadataDelFS;
extern t_configuracionDelFS configuracionDelFS;
extern t_log* LOGGERFS;
extern char* directorioConLaMetadata;
extern char* pathDeMontajeDelPrograma;
extern pthread_t threadConsola;

void inicializarVariablesGlobales();
void liberarRecursos();

#endif /* VARIABLESGLOBALES_H_ */
