/*
 * dump.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "dump.h"



int dump(char* nombreDeLaTabla){

	void dumpearTabla(void* nodo) {

	}

	bool esMiNodo(void* nodo) {
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		}

	tp_nodoDeLaMemTable nodoDeLaMem = list_remove_by_condition(memTable,esMiNodo);
	log_info(LOGGERFS,"Voy a dumpear la tabla", nodoDeLaMem->nombreDeLaTabla);
	t_metadataDeLaTabla metadataDeLaTabla = obtenerMetadataDeLaTabla(nodoDeLaMem->nombreDeLaTabla);
	char* nombreDelArchivoTemp=buscarNombreDelTempParaDumpear(nodoDeLaMem->nombreDeLaTabla);
	crearElTemp(nombreDelArchivoTemp);
	list_iterate(nodoDeLaMem->listaDeDatosDeLaTabla,dumpearTabla);
	log_info(LOGGERFS,"Tabla %s dumpeada",nombreDeLaTabla);
	liberarMemoriaDelNodo(nombreDeLaTabla);
	return EXIT_SUCCESS;
}

int liberarMemoriaDelNodo(char* liberarMemoriaDelNodo){
	return EXIT_SUCCESS;
}

int crearElTemp(char* nombreDelArchivo){
	log_info(LOGGERFS,"Voy a crear el archivo %s", nombreDelArchivo);
	FILE * archivoTemp = fopen(nombreDelArchivo,"w");
	fclose(archivoTemp);
	t_config* configuracion = config_create(nombreDelArchivo);
	config_set_value(configuracion, "SIZE", "0");
	config_set_value(configuracion, "PARTITIONS", "[]");
	config_save(configuracion);
	config_destroy(configuracion);
	log_info(LOGGERFS,"Archivo %s creado", nombreDelArchivo);
	return EXIT_SUCCESS;
}

char* buscarNombreDelTempParaDumpear(char* nombreDeLaTabla){
	char* aux=string_new();
	string_append(&aux, configuracionDelFS.puntoDeMontaje);
	string_append(&aux, "/Tables/");
	string_append(&aux, nombreDeLaTabla);
	string_append(&aux, "/");
	string_append(&aux, nombreDeLaTabla);
	char* pathDelTemp;
	//FILE* archivo;
	bool encontrado=true;
	for(int i=0;encontrado==true;i++){
		pathDelTemp=string_new();
		string_append(&pathDelTemp, aux);
		string_append(&pathDelTemp, string_itoa(i));
		string_append(&pathDelTemp, ".tmp");
		log_info(LOGGERFS,"Viendo si existe el archivo %s", pathDelTemp);
		encontrado=existeElArchivo(pathDelTemp);
		if(encontrado==true){
			log_error(LOGGERFS,"El nombre %s ya esta usado en otro dump", pathDelTemp);
		}else{
			log_info(LOGGERFS,"Encontrado el nombre del proximo archivo temp: %s para el dump", pathDelTemp);
			}
		}
	log_info(LOGGERFS,"Encontrado el nombre del proximo archivo temp: %s para el dump", pathDelTemp);
	return pathDelTemp;
}
