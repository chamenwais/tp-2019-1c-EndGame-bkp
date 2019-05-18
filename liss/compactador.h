/*
 * compactador.h
 *
 *  Created on: 17 may. 2019
 *      Author: franco
 */

#ifndef LISS_COMPACTADOR_H_
#define LISS_COMPACTADOR_H_

#include "variablesGlobales.h"
#include <commons/log.h>
#include <sys/types.h>
//#include <commons/collections/list.h>

void* crearCompactadorLissandra();
int lanzarCompactador();

#endif /* LISS_COMPACTADOR_H_ */
