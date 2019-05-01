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
	// 1) Verificar que la tabla exista en el file system.
	// 2) Eliminar directorio y todos los archivos de dicha tabla.

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
	/* La operación Describe permite obtener la Metadata de una tabla en particular.
	 *	1) Verificar que la tabla exista en el file system.
	 *	2) Leer el archivo Metadata de dicha tabla.
	 *	3) Retornar el contenido del archivo.
	 */
	t_metadataDeLaTabla metadata=obtenerMetadataDeLaTabla(nombreDeLaTabla);
	return metadata;
}

int insert(char* nombreDeLaTabla, uint16_t key, char* value, unsigned timeStamp){
	/* Ejemplo: INSERT TABLA1 3 “Mi nombre es Lissandra” 1548421507
	 * Pasos:
	 * 1) Verificar que la tabla exista en el file system. En caso que no exista,
	 * informa el error y continúa su ejecución.
	 * 2) Obtener la metadata asociada a dicha tabla.
	 * 3) Verificar si existe en memoria una lista de datos a dumpear.
	 * De no existir, alocar dicha memoria.
	 * 4) El parámetro Timestamp es opcional. En caso que un request no lo
	 * provea (por ejemplo insertando un valor desde la consola), se usará
	 * el valor actual del Epoch UNIX.
	 * 5)Insertar en la memoria temporal del punto anterior una nueva entrada que
	 * contenga los datos enviados en la request.
	 */
	if(exiteLaTabla(nombreDeLaTabla)==false){
		log_error(LOGGERFS,"Se esta intentando hace un insert de una tabla que no existe %s", nombreDeLaTabla);
		printf("Se esta intentando insertar una tabla que no existe %s\n", nombreDeLaTabla);
		return TABLA_NO_EXISTIA;
	}else{
		t_metadataDeLaTabla metadataDeLaTabla=obtenerMetadataDeLaTabla(nombreDeLaTabla);
		if(verSiExisteListaConDatosADumpear(nombreDeLaTabla)==false){
			aLocarMemoriaParaLaTabla(nombreDeLaTabla);
			}
		int resultadoDelInsert = hacerElInsertPosta(nombreDeLaTabla, key, value, timeStamp);
		return resultadoDelInsert;
		}
}
