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
#include "../COM/lqvg/protocolo.h"
#include "variablesGlobales.h"
#include <stdbool.h>
#include <stdint.h>

bool exiteLaTabla(char* nombreDeLaTabla);
int create(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion);
int drop(char* nombreDeLaTabla);
t_metadataDeLaTabla describe(char* nombreDeLaTabla);
t_describeAll_rta describeAll();//en funciones auxiliares esta la func
								//liberarYDestruirTablaDeMetadata para liberar la lista q tiene dentro t_describeAll_rta
int insert(char* nombreDeLaTabla, uint16_t key, char* value, long timeStamp);
int insertSinTime(char* nombreDeLaTabla, uint16_t key, char* value);
tp_nodoDeLaTabla selectf(char* nombreDeLaTabla, uint16_t key);

#endif /* FUNCIONESCOMUNES_H_ */
