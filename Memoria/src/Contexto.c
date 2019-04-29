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
char** IPS_SEEDS;
char** PUERTOS_SEEDS;
int RETARDO_ACCESO_MEMORIA;
int RETARDO_ACCESO_FILESYSTEM;
long TIEMPO_JOURNAL;
long TIEMPO_GOSSIPING;
char * MEMORIA_PRINCIPAL;

t_conexion_cliente conexiones_cliente[MAX_CLIENTES];
t_log* g_logger;
t_config* g_config;
