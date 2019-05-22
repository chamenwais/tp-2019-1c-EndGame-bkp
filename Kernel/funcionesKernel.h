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
int conectarse_con_memoria(int ip, int puerto);
void configurar_signals(void);
void captura_signal(int);
void logger(int tipo_esc, int tipo_log, const char* mensaje, ...);
void escribir_por_pantalla(int tipo_esc, int tipo_log, char* console_buffer,
		char* log_colors[8], char* msj_salida);
void definir_nivel_y_loguear(int tipo_esc, int tipo_log, char* msj_salida);
int inicializarListas();
t_operacion parsear(char * linea);
//void conocer_pool_memorias(int socket);



#endif /* FUNCIONESKERNEL_H_ */
