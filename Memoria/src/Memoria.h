/*
 * Memoria.h
 *
 *  Created on: 8 abr. 2019
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

/*** Includes ***/
#include "Contexto.h"
#include "Utilidades.h"
#include "GestionSegPaginada.h"
#include "Consola.h"
#include "../test/TestGeneral.h"
#include "ManejoConexionesyAPI.h"

void ejecutar_programa_memoria(int argc, char ** argv);
void stdin_no_bloqueante(void);
int atender_al_kernel(int serv_socket);
void inicializar_conexiones_cliente(void);
int comunicarse_con_lissandra(void);
void escuchar_clientes(int, int);
int clasificar_conexion_cerrada(int, int, int);
void loguear_cerrar_conexion(int, int, int, int, fd_set);
void validar_conexion_con_lissandra(int);
int agregar_conexion_lista_clientes(int,struct sockaddr_in);
struct sockaddr_in crear_direccion_cliente();
void iniciar_el_proceso_de_gossiping();
void iniciar_el_proceso_de_journaling();
void iniciar_la_memoria_principal();
char *reservar_total_memoria();
void *realizar_gossiping();
void *realizar_journaling();
void clasificar_y_atender_cabecera(int, int, int);

#endif /* MEMORIA_H_ */
