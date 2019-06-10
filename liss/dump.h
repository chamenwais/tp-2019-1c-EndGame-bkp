/*
 * dump.h
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#ifndef DUMP_H_
#define DUMP_H_

#include "lissandra.h"
#include "variablesGlobales.h"
#include <dirent.h>

int dump(char* nombreDeLaTabla);
char* buscarNombreDelTempParaDumpear(char* nombreDeLaTabla);
int liberarMemoriaDelNodo(char* liberarMemoriaDelNodo);
//int lanzarDumps();
void hiloDeDumpeo(tp_hiloDeDumpeo hiloDeDumpeo);
int lanzarHiloParaLaTablaDeDumpeo(char* nombreDeLaTabla);
int insertarDatosEnElBloque(char* cadenaAInsertar,int bloqueActual);
int crearElTemp(char* nombreDelArchivo,char* bloques,int size);

#endif /* DUMP_H_ */
