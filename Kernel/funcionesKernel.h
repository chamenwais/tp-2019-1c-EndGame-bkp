/*
 * funcionesKernel.h
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESKERNEL_H_
#define FUNCIONESKERNEL_H_

#include <stdlib.h>
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
void conocer_pool_memorias();
void enviar_handshake(int socket);
void* funcionHiloRequest(void* pcb);
int lanzarPlanificador();
int inicializarSemaforos();
void realizar_operacion(t_operacion resultado_del_parseado, tp_lql_pcb pcb);
void operacion_select(char* nombre_tabla, int key, tp_lql_pcb pcb);
void operacion_insert(char* nombre_tabla, int key, char* value, tp_lql_pcb pcb);
void operacion_create(char* nombre_tabla, int tipo_consistencia, int num_particiones, int compaction_time, tp_lql_pcb pcb);
void operacion_describe(char* nombre_tabla, tp_lql_pcb pcb);
void operacion_drop(char* nombre_tabla, tp_lql_pcb pcb);
void operacion_journal();
void operacion_add(int num_memoria, int tipo_consistencia, tp_lql_pcb pcb);
int decidir_memoria_a_utilizar(char *nombre_tabla);
int lanzarPCP();
void* funcionHiloPCP();
void* funcionHiloPLP();
int lanzarHiloRequest(tp_lql_pcb pcb);
bool existeTabla(char* tabla);



#endif /* FUNCIONESKERNEL_H_ */
