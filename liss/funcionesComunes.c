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

	return resultado;
}

int crearDirectorioParaLaTabla(char* nombreDeLaTabla){
	// Crear el directorio para dicha tabla.
	return EXIT_SUCCESS;
}

int crearMetadataParaLaTabla(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion){
	// Crear el archivo Metadata asociado al mismo.
	// Grabar en dicho archivo los parámetros pasados por el request.
	return EXIT_SUCCESS;
}

int crearArchivosBinariosYAsignarBloques(char* nombreDeLaTabla,
		int numeroDeParticiones){
	// Crear los archivos binarios asociados a cada partición de la tabla
	// y asignar a cada uno un bloque
	return EXIT_SUCCESS;
}

int create(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion){
	if(exiteLaTabla(nombreDeLaTabla)==false){
		crearDirectorioParaLaTabla(nombreDeLaTabla);
		crearMetadataParaLaTabla(nombreDeLaTabla,tipoDeConsistencia,
				numeroDeParticiones,tiempoDeCompactacion);
		crearArchivosBinariosYAsignarBloques(nombreDeLaTabla,numeroDeParticiones);
	}else{
		log_error(LOGGERFS,"Se esta intentando crear una tabla con un nombre que ya existia: %s", nombreDeLaTabla);
		printf("Se esta intentando crear una tabla con un nombre que ya existia: %s", nombreDeLaTabla);
		return TABLA_YA_EXISTIA;
		}
	return TABLA_CREADA;
}
