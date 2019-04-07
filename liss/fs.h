/*
 * fs.h
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#ifndef FS_H_
#define FS_H_d

#include <stdlib.h>
#include "lissandra.h"
#include "variablesGlobales.h"
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

void inicializarLogDelFS();
int obtenerPathDeMontajeDelPrograma(int argc,char** argv);
int levantarConfiguracionInicialDelFS();
int levantarMetadataDelFS();

#endif /* FS_H_ */
