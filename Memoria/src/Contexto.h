/*
 * Contexto.h
 *
 *  Created on: 8 abr. 2019
 *      Author: utnso
 *      Variables globales y contexto general
 */

#ifndef CONTEXTO_H_
#define CONTEXTO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/log.h>    // Para Logger
#include <commons/config.h> // Para Archivo de configuración
#include <commons/string.h> // Para manejo de strings
#include <signal.h>			// Para manejo de señales
#include <stdarg.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>      // Para crear los sockets
#include <netdb.h> 			// Para getaddrinfo
#include <unistd.h> 		// Para close(socket)
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <errno.h>
#include <lqvg/com.h>
#include <lqvg/protocolo.h>


/* Claves archivo de configuracion*/
#define CLAVE_CONFIG_PUERTO_ESCUCHA "puerto_escucha"
#define CLAVE_CONFIG_IP_FILESYSTEM "ip_filesystem"
#define CLAVE_CONFIG_PUERTO_FILESYSTEM "puerto_filesystem"
#define CLAVE_CONFIG_TAMANIO_MEMORIA "tamanio_memoria"
#define CLAVE_CONFIG_NUMERO_MEMORIA "numero_memoria"
#define MAX_CLIENTES 20
#define STDIN 0
#define TRUE 1
#define MAX_LINEA 255
#define NO_SOCKET -1

/* Variables globales*/
extern char * PUERTO_ESCUCHA;
extern char* IP_FILESYSTEM;
extern int PUERTO_FILESYSTEM;
extern int TAMANIO_MEMORIA;
extern int NUMERO_MEMORIA;

extern int TAMANIO_VALUE;

#define LFS "LFS"
#define _KERNEL "KERNEL"

/*** Enums modo de log***/
enum tipo_logueo {
	escribir,
	loguear,
	escribir_loguear,
	l_trace,
	l_debug,
	l_info,
	l_warning,
	l_error
};

struct conexion_cliente {
	int pid;
	int socket;
	struct sockaddr_in addres;
};
typedef struct conexion_cliente t_conexion_cliente;

extern t_conexion_cliente conexiones_cliente[MAX_CLIENTES];
extern t_log* g_logger;
extern t_config* g_config;

#endif /* CONTEXTO_H_ */
