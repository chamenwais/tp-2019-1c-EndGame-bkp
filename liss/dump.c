/*
 * dump.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "dump.h"

int dump(){

	void dumpearTabla(void* nodo) {
		!strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		}

	list_iterate(memTable,dumpearTabla);
	vaciarMemTable();
	memTable=list_create();
	return EXIT_SUCCESS;
}
