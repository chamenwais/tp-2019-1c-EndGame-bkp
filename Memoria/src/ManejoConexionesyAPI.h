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
void realizar_insert(char *, long, uint16_t, char *);
void realizar_create(char *, char *, int, int);
void loguear_value_por_pantalla(char *);
tp_select_rta pedir_value_a_liss(char *, uint16_t);
void atender_create(int, int);
void atender_select(int, int);
void atender_insert(int, int);
void atender_drop(int, int);
void atender_describe(int, int);

#endif /* SRC_MANEJOCONEXIONESYAPI_H_ */
