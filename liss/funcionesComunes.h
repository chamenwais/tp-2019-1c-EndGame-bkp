/*
 * funcionesComunes.h
 *
 *  Created on: 18 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESCOMUNES_H_
#define FUNCIONESCOMUNES_H_

#include "lissandra.h"
#include "funcionesAuxiliares.h"
#include "../COM/lqvg/com.h"
#include "variablesGlobales.h"
#include <stdbool.h>

bool exiteLaTabla(char* nombreDeLaTabla);
int create(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion);
int drop(char* nombreDeLaTabla);
t_metadataDeLaTabla describe(char* nombreDeLaTabla);

#endif /* FUNCIONESCOMUNES_H_ */
