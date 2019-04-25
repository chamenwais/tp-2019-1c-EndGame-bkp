/*
 * funcionesComunes.c
 *
 *  Created on: 18 abr. 2019
 *      Author: utnso
 */

#include "funcionesComunes.h"

bool exiteLaTabla(char* nombreDeLaTabla){
	// Verificar que la tabla no exista en el file system.
	// Por convención, una tabla existe si ya hay otra con el mismo nombre.
	// Para dichos nombres de las tablas siempre tomaremos sus valores en UPPERCASE (mayúsculas).
	bool resultado;
	char* directorioDeLaTabla=string_new();
	string_append(&directorioDeLaTabla, configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeLaTabla, "/Tables/");
	string_append(&directorioDeLaTabla, nombreDeLaTabla);
	struct stat st = {0};
	if(stat(directorioDeLaTabla, &st) == -1){
		log_info(LOGGERFS,"La tabla %s no existe", directorioDeLaTabla);
		resultado=false;
	}else{
		log_info(LOGGERFS,"La tabla %s ya existe", directorioDeLaTabla);
		resultado=true;
		}
	return resultado;
}

int create(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion){
	if(exiteLaTabla(nombreDeLaTabla)==false){
		crearDirectorioParaLaTabla(nombreDeLaTabla);
		crearMetadataParaLaTabla(nombreDeLaTabla,tipoDeConsistencia,
				numeroDeParticiones,tiempoDeCompactacion);
		if(crearArchivosBinariosYAsignarBloques(nombreDeLaTabla,numeroDeParticiones)==EXIT_SUCCESS){
			log_info(LOGGERFS,"La tabla %s se creo correctamente", nombreDeLaTabla);
		}else{
			log_error(LOGGERFS,"No se puedo crear la tabla %s", nombreDeLaTabla);
			}
		return TABLA_CREADA;
	}else{
		log_error(LOGGERFS,"Se esta intentando crear una tabla con un nombre que ya existia: %s", nombreDeLaTabla);
		printf("Se esta intentando crear una tabla con un nombre que ya existia: %s\n", nombreDeLaTabla);
		return TABLA_YA_EXISTIA;
		}
}


int drop(char* nombreDeLaTabla){
	// Pasos para hacerlo:
	// Verificar que la tabla exista en el file system.
	// Eliminar directorio y todos los archivos de dicha tabla.

	if(exiteLaTabla(nombreDeLaTabla)==false){
		log_error(LOGGERFS,"Se esta intentando borrar una tabla que no existe %s", nombreDeLaTabla);
		printf("Se esta intentando borrar una tabla que no existe %s\n", nombreDeLaTabla);
		return TABLA_NO_EXISTIA;
	}else{
		eliminarDirectorioYArchivosDeLaTabla(nombreDeLaTabla);
		log_info(LOGGERFS,"Se borro la tabla %s", nombreDeLaTabla);
		return TABLA_BORRADA;
		}
}

t_metadataDeLaTabla describe(char* nombreDeLaTabla){
	t_metadataDeLaTabla metadata=obtenerMetadataDeLaTabla(nombreDeLaTabla);
	return metadata;
}
