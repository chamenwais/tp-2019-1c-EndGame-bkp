/*
 * Contexto.c
 *
 *  Created on: 8 abr. 2019
 *      Author: utnso
 *      Variables globales y contexto general
 */
#include "Contexto.h"

/* Variables globales*/
char* PUERTO_ESCUCHA;
char* IP_FILESYSTEM;
int PUERTO_FILESYSTEM;
int TAMANIO_MEMORIA;
int NUMERO_MEMORIA;
int TAMANIO_VALUE;

t_conexion_cliente conexiones_cliente[MAX_CLIENTES];
t_log* g_logger;
t_config* g_config;
