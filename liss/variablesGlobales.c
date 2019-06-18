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
pthread_t threadConsola, threadCompactador, threadDumps;
pthread_mutex_t mutexVariableTiempoDump, mutexVariableRetardo, mutexBitmap,
	mutexEstadoDeFinalizacionDelSistema, mutexDeLaMemtable, mutexDeDump;
t_bitarray *bitmap;
int sizeDelBitmap;
char * srcMmap;
char * bufferArchivo;
t_list* memTable;
t_list* dumpTables;
bool estadoDeFinalizacionDelSistema;

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
	if(pthread_mutex_init(&mutexEstadoDeFinalizacionDelSistema, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexEstadoDeFinalizacionDelSistema");
		return EXIT_FAILURE;
	}else{
		setearEstadoDeFinalizacionDelSistema(false);
		}
	if(pthread_mutex_init(&mutexDeLaMemtable, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexDeLaMemtable");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexDeDump, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexDeOperacionCritica");
		return EXIT_FAILURE;
		}

	bitmap=NULL;
	sizeDelBitmap=-1;
	bufferArchivo=NULL;
	archivoDeBitmap=NULL;
	archivoDeLaMetadata=NULL;
	memTable=list_create();
	dumpTables=list_create();
	return EXIT_SUCCESS;
}

void liberarRecursos(){
	log_info(LOGGERFS,"Liberando recursos");

	if(configuracionDelFS.puntoDeMontaje!=NULL)free(configuracionDelFS.puntoDeMontaje);
	if(metadataDelFS.magicNumber!=NULL)free(metadataDelFS.magicNumber);
	free(pathDeMontajeDelPrograma);
	free(directorioConLaMetadata);
	free(archivoDeBitmap);
	free(archivoDeLaMetadata);
	bajarADiscoBitmap();
	log_info(LOGGERFS,"Destruyendo el bitarray");
	pthread_mutex_lock(&mutexBitmap);
	bitarray_destroy(bitmap);
	//free(srcMmap);
	free(bufferArchivo);
	vaciarMemTable();
	vaciarDumpTable();
	log_destroy(LOGGERFS);
	pthread_mutex_destroy(&mutexVariableRetardo);
	pthread_mutex_destroy(&mutexVariableTiempoDump);
	pthread_mutex_destroy(&mutexDeLaMemtable);
	pthread_mutex_destroy(&mutexBitmap);
	pthread_mutex_destroy(&mutexDeDump);
	pthread_mutex_destroy(&mutexEstadoDeFinalizacionDelSistema);
	printf("Memoria liberada, programa finalizado\n");
	return;
}

int vaciarMemTable(){
	void destruirTabla(void* nodoDeLaMemtable){
		void vaciarNodos(void* nodoDeUnaTabla){
			free(((tp_nodoDeLaTabla)nodoDeUnaTabla)->value);
			free((tp_nodoDeLaTabla)nodoDeUnaTabla);
			}
		if(!list_is_empty(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla)){
			log_info(LOGGERFS,"Liberando la tabla: %s",
				((tp_nodoDeLaMemTable)nodoDeLaMemtable)->nombreDeLaTabla);
			list_iterate(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla,
				vaciarNodos);
			free(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->nombreDeLaTabla);
			list_destroy(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla);
			free((tp_nodoDeLaMemTable)nodoDeLaMemtable);
			}
		}
	log_info(LOGGERFS,"Liberando memtable");
	if(memTable!=NULL){
		if(!list_is_empty(memTable)){
			log_info(LOGGERFS,"La memtable tiene datos asi q paso a liberarla");
			list_iterate(memTable,destruirTabla);
		}else{
			log_info(LOGGERFS,"La memtable no tenia datos asi q no libero nada");
			}
		list_destroy(memTable);
	}else{
		log_error(LOGGERFS,"La memtable esta NULL");
		}

	log_info(LOGGERFS,"Memtable liberada");
	return EXIT_SUCCESS;
}

int vaciarDumpTable(){
	//implementar
	return EXIT_SUCCESS;
}
