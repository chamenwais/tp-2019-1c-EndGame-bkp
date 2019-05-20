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

void realizar_select(char * nombre_tabla, int key);
void loguear_value_por_pantalla(char * value);
char * pedir_value_a_liss(char * nombre_tabla, int key);


#endif /* SRC_MANEJOCONEXIONESYAPI_H_ */
