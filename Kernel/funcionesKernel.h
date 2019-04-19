/*
 * funcionesKernel.h
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESKERNEL_H_
#define FUNCIONESKERNEL_H_

#include <stdlib.h>
#include "kernel.h"
#include "variablesGlobales.h"
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

void inicializarLogKernel();
int inicializarVariablesGlobales();
int levantarConfiguracionInicialDelKernel();
int conectarse_con_memoria(void);



#endif /* FUNCIONESKERNEL_H_ */
