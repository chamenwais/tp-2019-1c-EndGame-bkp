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
#include <commons/bitarray.h>
#include <commons/string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

void inicializarLogDelFS();
int obtenerPathDeMontajeDelPrograma(int argc,char** argv);
int levantarConfiguracionInicialDelFS();
int levantarMetadataDelFS();
int actualizarTiempoDump(int tiempoDump);
int obtenerTiempoDump();
int actualizarRetardo(int retardo);
int obtenerRetardo();
int obtenerBloqueLibreDelBitMap();
int levantarBitMap();
int ocuparBloqueDelBitmap(int numeroDeBloque);
int bajarADiscoBitmap();
int imprimirEstadoDelBitmap();

#endif /* FS_H_ */
