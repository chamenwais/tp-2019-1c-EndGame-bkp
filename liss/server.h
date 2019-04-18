/*
 * server.h
 *
 *  Created on: 13 abr. 2019
 *      Author: utnso
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

void* crearServerLissandra();
int lanzarServer();



#endif /* LISS_SERVER_H_ */
