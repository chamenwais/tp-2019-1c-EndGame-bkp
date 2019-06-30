/*
 * variablesGlobales.h
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#ifndef VARIABLESGLOBALES_H_
#define VARIABLESGLOBALES_H_

#ifndef _XOPEN_SOURCE
#define XOPEN_SOURCE 500 //para ftw
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE //tmb
#endif

#include <ftw.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
//#include "fs.h"
#include "../COM/lqvg/com.h"
//#include "funcionesAuxiliares.h"

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
	enum MENSAJES resultado;
} t_nodoDeLaTabla;
typedef t_nodoDeLaTabla* tp_nodoDeLaTabla;

typedef struct definicionHiloDeDumpeo {
	int tiempoDeSleep;
	char* nombreDeLaTabla;
	pthread_attr_t attr;
	pthread_t thread;
} t_hiloDeDumpeo;
typedef t_hiloDeDumpeo* tp_hiloDeDumpeo;

typedef struct tablaDeFS {//Para la lista de tablas del fs
	pthread_mutex_t* mutexTabla;
	char* nombreTabla;
} t_tablaDeFS;
typedef t_tablaDeFS* tp_tablaDeFS;

extern t_metadataDelFS metadataDelFS;
extern t_configuracionDelFS configuracionDelFS;
extern t_log* LOGGERFS;
extern char* directorioConLaMetadata;
extern char* archivoDeBitmap;
extern char* archivoDeLaMetadata;
extern char* pathDeMontajeDelPrograma;
extern char* nombreArchivoInfoMsBloqueada;
extern pthread_t threadConsola, threadCompactador, threadDumps, threadMonitoreadorDeArchivos;
extern pthread_mutex_t mutexVariableTiempoDump, mutexVariableRetardo, mutexBitmap,
	mutexEstadoDeFinalizacionDelSistema, mutexDeLaMemtable, mutexDeDump;
extern t_bitarray *bitmap;
extern int sizeDelBitmap;
extern char * srcMmap;
extern char * bufferArchivo;
extern pthread_t threadServer;
extern t_list* memTable;
extern t_list* dumpTables;
extern bool estadoDeFinalizacionDelSistema;

int inicializarVariablesGlobales();
void liberarRecursos();
int vaciarMemTable();
int vaciarDumpTable();
//-+-Para operaciones sobre la lista que contiene todas las tablas del fs
pthread_mutex_t* bloquearTablaFS(char* nombreTabla);//bloquea el mutex correspondiente y te lo devuelve para que cuando termines lo desbloquees,
													//si la tabla no existe devuelve NULL
void desbloquearTablaFS(pthread_mutex_t* mutexTabla);
bool agregarAListaDeTablasFS(char* nuevaTabla);//devuelve true si agrego la tabla o false si ya existe
bool eliminarDeListaDeTablasFS(char* tablaABorrar);//devuelve true si la elimino o false si no existe

t_list* bloquearListaDeTablasFS();//te devuelve la tabla "maestra" por si queres x ej ver todas sus tablas, SOLO PARA LECTURA, el formato de cada elemento es tp_tablaDeFS
void desbloquearListaDeTablasFS();//desbloquear la tabla en cuanto termines tu busqueda, estas bloqueando todo el fs

void buscarTablasYaCreadasFS();//solo se usa al inicio del programa
//-+-
#endif /* VARIABLESGLOBALES_H_ */
