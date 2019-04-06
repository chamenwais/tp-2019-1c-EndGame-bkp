/*
 * api.h
 *
 *  Created on: 5 abr. 2019
 *      Author: utnso
 */

#ifndef API_H_
#define API_H_

#include <pthread.h>
#include <readline/readline.h>
#include "lissandra.h"

int lanzarConsola();
void *funcionHiloConsola(void *arg);

#endif /* SRC_API_H_ */
