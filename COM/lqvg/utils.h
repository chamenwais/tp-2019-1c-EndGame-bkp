/*
 * utils.h
 *
 *  Created on: 22 may. 2019
 *      Author: utnso
 */

#ifndef LQVG_UTILS_H_
#define LQVG_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/time.h>

char * obtener_value_a_insertar(char *);
t_list * obtener_lista_lineas_desde_archivo(char *);
t_list * obtener_lista_linea(char * linea_original);
char* conocer_ip_propia();

#endif /* LQVG_UTILS_H_ */
