/*
 * lissandra.h
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include "actualizarConfig.h"
#include "api.h"
#include "variablesGlobales.h"
#include "funcionesComunes.h"
#include "funcionesAuxiliares.h"
#include <signal.h> //para detectar señal de cierre del programa
#include "fs.h"
#include "dump.h"
#include "server.h"
#include "compactador.h"

void captura_signal(int signo);
void configurar_signals(void);

#endif /* LISSANDRA_H_ */
