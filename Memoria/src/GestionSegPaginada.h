/*
 * GestionSegPaginada.h
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#ifndef SRC_GESTIONSEGPAGINADA_H_
#define SRC_GESTIONSEGPAGINADA_H_

#include "Contexto.h"

/* Esta sería la estructura de un marco en la MP
 *
struct stru_marco{
	long timestamp;
	uint16_t key;
	char value[TAMANIO_VALUE];
};
typedef struct stru_marco t_marco;*/

int obtener_tamanio_marco();

int obtener_cantidad_marcos_en_MP(int );

#endif /* SRC_GESTIONSEGPAGINADA_H_ */
