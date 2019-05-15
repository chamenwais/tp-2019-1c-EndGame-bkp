/*
 * funcionesAuxiliares.h
 *
 *  Created on: 24 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESAUXILIARES_H_
#define FUNCIONESAUXILIARES_H_

#include "lissandra.h"
#include "variablesGlobales.h"
#include "../COM/lqvg/com.h"
#include <stdbool.h>

int crearDirectorioParaLaTabla(char* nombreDeLaTabla);
int crearMetadataParaLaTabla(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion);
int crearArchivosBinariosYAsignarBloques(char* nombreDeLaTabla,
		int numeroDeParticiones);
int eliminarDirectorioYArchivosDeLaTabla(char* nombreDeLaTabla);
int eliminarDirectorio(char* nombreDeLaTabla);
int eliminarArchivoDeMetada(char* nombreDeLaTabla);
int liberarBloquesYParticiones(char* nombreDeLaTabla);
int bloquearTabla(char* nombreDeLaTabla);
int desbloquearTabla(char* nombreDeLaTabla);
t_metadataDeLaTabla obtenerMetadataDeLaTabla(char* nombreDeLaTabla);
bool verSiExisteListaConDatosADumpear(char* nombreDeLaTabla);
int aLocarMemoriaParaLaTabla(char* nombreDeLaTabla);
tp_nodoDeLaMemTable obtenerNodoDeLaMemtable(char* nodeName);
int hacerElInsertEnLaMemoriaTemporal(char* nombreDeLaTabla, uint16_t key, char* value, unsigned timeStamp);

#endif /* FUNCIONESAUXILIARES_H_ */
