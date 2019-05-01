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
#include <sys/types.h>      // Para crear los sockets
#include <sys/inotify.h>
#include <pthread.h>
#include <netdb.h> 			// Para getaddrinfo
#include <unistd.h> 		// Para close(socket)
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <errno.h>
#include <lqvg/com.h>
#include <lqvg/protocolo.h>
#include <time.h>

/* Claves archivo de configuracion*/
#define CLAVE_CONFIG_PUERTO_ESCUCHA "puerto_escucha"
#define CLAVE_CONFIG_IP_FILESYSTEM "ip_filesystem"
#define CLAVE_CONFIG_PUERTO_FILESYSTEM "puerto_filesystem"
#define CLAVE_CONFIG_TAMANIO_MEMORIA "tamanio_memoria"
#define CLAVE_CONFIG_NUMERO_MEMORIA "numero_memoria"
#define CLAVE_CONFIG_IP_SEEDS "ip_seeds"
#define CLAVE_CONFIG_PUERTO_SEEDS "puerto_seeds"
#define CLAVE_CONFIG_RETARDO_ACCESO_MEMORIA "retardo_acceso_memoria"
#define CLAVE_CONFIG_RETARDO_ACCESO_FILESYSTEM "retardo_acceso_filesystem"
#define CLAVE_CONFIG_TIEMPO_JOURNAL "tiempo_journal"
#define CLAVE_CONFIG_TIEMPO_GOSSIPING "tiempo_gossiping"
#define PATH_LOG "/home/utnso/memoria.log"
#define DIRECTORIO_CONFIG_DEFAULT "../"
#define NOMBRE_ARCH_CONFIG_DEFAULT "memoria.config"
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
extern char** IPS_SEEDS;
extern char** PUERTOS_SEEDS;
extern int RETARDO_ACCESO_MEMORIA;
extern int RETARDO_ACCESO_FILESYSTEM;
extern long TIEMPO_JOURNAL;
extern long TIEMPO_GOSSIPING;
extern char* MEMORIA_PRINCIPAL;
extern t_list * seeds;

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

struct path_archivo_conf {
	char * directorio;
	char * nombre_archivo;
};
typedef struct path_archivo_conf t_path_archivo_conf;

extern t_conexion_cliente conexiones_cliente[MAX_CLIENTES];
extern t_log* g_logger;
extern t_config* g_config;

#endif /* CONTEXTO_H_ */
