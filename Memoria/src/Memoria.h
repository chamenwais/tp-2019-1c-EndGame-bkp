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
int comunicarse_con_lissandra(void);
void escuchar_clientes(int, int);
int clasificar_conexion_cerrada(int, int, int);
void loguear_cerrar_conexion(int, int, int, int);
void validar_conexion_con_lissandra(int);
int agregar_conexion_lista_clientes(int,struct sockaddr_in);
struct sockaddr_in crear_direccion_cliente();

#endif /* MEMORIA_H_ */
