/*
 * ManejoConexionesyAPI.h
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */

#ifndef SRC_MANEJOCONEXIONESYAPI_H_
#define SRC_MANEJOCONEXIONESYAPI_H_

#include "Contexto.h"
#include "Utilidades.h"
#include "GestionSegPaginada.h"

void realizar_select(char *, int);
void loguear_value_por_pantalla(char *);
tp_select_rta pedir_value_a_liss(char *, uint16_t);

#endif /* SRC_MANEJOCONEXIONESYAPI_H_ */
