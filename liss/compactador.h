/*
 * compactador.h
 *
 *  Created on: 17 may. 2019
 *      Author: franco
 */

#ifndef LISS_COMPACTADOR_H_
#define LISS_COMPACTADOR_H_

#ifndef _XOPEN_SOURCE
#define XOPEN_SOURCE 500 //para ftw
#define _GNU_SOURCE //tmb
#endif

#include "variablesGlobales.h"
#include <stdbool.h>
#include <commons/log.h>
#include <commons/string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <ftw.h>
//#include "funcionesAuxiliares.h"

void compactarNuevasTablas();
int crearCompactadorDeTablas(const char*, const struct stat*, int, struct FTW*);
void* compactadorTabla(char*);
void* crearCompactadorLissandra();
int lanzarCompactador();

#endif /* LISS_COMPACTADOR_H_ */
