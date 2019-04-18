/*
 * funcionesComunes.h
 *
 *  Created on: 18 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESCOMUNES_H_
#define FUNCIONESCOMUNES_H_

#include "lissandra.h"
#include "../COM/lqvg/com.h"
#include <stdbool.h>

bool exiteLaTabla(char* nombreDeLaTabla);
int crearDirectorioParaLaTabla(char* nombreDeLaTabla);
int crearMetadataParaLaTabla(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion);
int crearArchivosBinariosYAsignarBloques(char* nombreDeLaTabla,
		int numeroDeParticiones);
int create();

#endif /* FUNCIONESCOMUNES_H_ */
