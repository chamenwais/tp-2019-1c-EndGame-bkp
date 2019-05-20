/*
 * dump.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "dump.h"

int dump(){

	void dumpearTabla(void* nodo) {
		//!strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		t_metadataDeLaTabla metadataDeLaTabla = obtenerMetadataDeLaTabla(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla);
		char* directorioDeLaTabla=string_new();
		string_append(&directorioDeLaTabla, configuracionDelFS.puntoDeMontaje);
		string_append(&directorioDeLaTabla, "/Tables/");
		string_append(&directorioDeLaTabla, ((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla);

	}

	list_iterate(memTable,dumpearTabla);
	vaciarMemTable();
	memTable=list_create();
	return EXIT_SUCCESS;
}
