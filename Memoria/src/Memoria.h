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
#include "Consola.h"

void stdin_no_bloqueante(void);
int atender_al_kernel(int serv_socket);
void inicializar_conexiones_cliente(void);
void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int));
int comunicarse_con_lissandra(void);
void escuchar_clientes(int, int);

#endif /* MEMORIA_H_ */
