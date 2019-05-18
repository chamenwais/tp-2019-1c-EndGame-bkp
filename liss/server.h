/*
 * server.h
 *
 *  Created on: 13 abr. 2019
 *      Author: franco
 */

#ifndef LISS_SERVER_H_
#define LISS_SERVER_H_

#include "variablesGlobales.h"
#include "../COM/lqvg/com.h"
#include "../COM/lqvg/protocolo.h"
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "funcionesComunes.h"

void procesarSelect(int,t_cabecera);
void procesarInsert(int,t_cabecera);
void procesarCreate(int,t_cabecera);
void procesarDescribe(int,t_cabecera);
void procesarDescribeAll(int);
void procesarDrop(int,t_cabecera);

void* procesarMensaje(void*);
void* crearServerLissandra();
int lanzarServer();
int list_mayor_int(t_list *);

#endif /* LISS_SERVER_H_ */
