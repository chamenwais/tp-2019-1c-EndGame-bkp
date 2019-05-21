/*
 * dump.h
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#ifndef DUMP_H_
#define DUMP_H_

#include "lissandra.h"

int dump(char* nombreDeLaTabla);
char* buscarNombreDelTempParaDumpear(char* nombreDeLaTabla);
int liberarMemoriaDelNodo(char* liberarMemoriaDelNodo);
int crearElTemp(char* nombreDelArchivo);

#endif /* DUMP_H_ */
