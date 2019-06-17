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

tp_select_rta_a_kernel realizar_select(char *, int);
int realizar_insert(char *, long, uint16_t, char *);
enum MENSAJES realizar_create(char *, char *, int, int);
void loguear_value_por_pantalla(char *);
tp_select_rta_a_kernel pedir_value_a_liss(char *, uint16_t);
void atender_create(int, int);
void atender_select(int, int);
void atender_insert(int, int);
void atender_drop(int, int);
void atender_describe(int, int);
void atender_describe_de_todas_las_tablas(int);
void atender_describe_tabla_particular(tp_describe, int);
void atender_journal(int);
void realizar_describe_de_todas_las_tablas();
tp_describe_particular_rta_a_kernel realizar_describe_para_tabla_particular(char *);
void imprimir_informacion_tabla_particular(void *);
enum MENSAJES realizar_drop(char *);
void retornar_respuesta_al_kernel(enum MENSAJES, void(*enviador_respuesta_ok)(int), int);
void convertir_respuesta_select(tp_select_rta_a_kernel,tp_select_rta, enum MENSAJES);
void convertir_respuesta_describe_particular(tp_describe_particular_rta_a_kernel, tp_describe_rta, enum MENSAJES);
void liberar_rta_interna_select(tp_select_rta);

#endif /* SRC_MANEJOCONEXIONESYAPI_H_ */
