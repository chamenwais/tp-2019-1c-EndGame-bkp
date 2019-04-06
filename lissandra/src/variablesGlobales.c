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

void inicializarVariablesGlobales(){
	configuracionDelFS.puertoEscucha=-1;
	configuracionDelFS.puntoDeMontaje=NULL;
	configuracionDelFS.retardo=-1;
	configuracionDelFS.sizeValue=-1;
	configuracionDelFS.tiempoDump-1;
	metadataDelFS.blockSize=-1;
	metadataDelFS.blocks=-1;
	metadataDelFS.magicNumber=NULL;
	LOGGERFS=NULL;
	directorioConLaMetadata=NULL;
	pathDeMontajeDelPrograma=NULL;
	return;
}

void liberarRecursos(){
	log_info(LOGGERFS,"Liberando recursos");

	return;
}
