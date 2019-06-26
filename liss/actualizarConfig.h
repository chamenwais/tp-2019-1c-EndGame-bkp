/*
 * actualizarConfig.h
 *
 *  Created on: 26 jun. 2019
 *      Author: utnso
 */

#ifndef ACTUALIZARCONFIG_H_
#define ACTUALIZARCONFIG_H_

#include "lissandra.h"
#include "funcionesComunes.h"
#include <sys/types.h>
#include <sys/inotify.h>

int lanzarMonitoreadorDeArchivo();
int funcionMonitorDeArchivos();


#endif /* ACTUALIZARCONFIG_H_ */
