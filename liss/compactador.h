/*
 * compactador.h
 *
 *  Created on: 17 may. 2019
 *      Author: franco
 */

#ifndef LISS_COMPACTADOR_H_
#define LISS_COMPACTADOR_H_

//#ifndef _XOPEN_SOURCE
#define XOPEN_SOURCE 500 //para ftw
#define _GNU_SOURCE //tmb
//#endif

//#include "variablesGlobales.h"
#include <stdbool.h>
#include <commons/log.h>
#include <commons/string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <ftw.h>
#include <stdio.h>
//#include "dump.h"
//#include "funcionesAuxiliares.h"
#include <time.h>

#include "lissandra.h"
#include "variablesGlobales.h"

typedef struct TKV {
	unsigned timeStamp;
	uint16_t key;
	char* value;
} t_tkv;
typedef t_tkv* tp_tkv;

void guardarMilisegundosBloqueada(char*,int,bool);
char* getNextTemp();
char* convertirTKVsAString(t_list*);
void liberarTKV(tp_tkv);
void liberarListaTKV(t_list*);
t_list* cargarTimeStampKeyValue(char*);
t_list* crearTempsParaBins(char*);
char* crearTempParaTmpcs(t_list*);
t_list* compararBinsContraTmpcs(t_list*,char*);
void compactarNuevasTablas();
int crearCompactadorDeTablas(const char*, const struct stat*, int, struct FTW*);
void* compactadorTabla(char*);
void* crearCompactadorLissandra();
int lanzarCompactador();

#endif /* LISS_COMPACTADOR_H_ */
