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
	if (stat(directorioDeLaTabla, &st) == -1){
		log_info(LOGGERFS,"La tabla %s no existe", directorioDeLaTabla);
		resultado=false;
	}else{
		log_info(LOGGERFS,"La tabla %s ya existe", directorioDeLaTabla);
		resultado=true;
		}
	return resultado;
}

int crearDirectorioParaLaTabla(char* nombreDeLaTabla){
	// Crear el directorio para dicha tabla.
	char* directorioDeLaTabla=string_new();
	string_append(&directorioDeLaTabla, configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeLaTabla, "/Tables");
	mkdir(directorioDeLaTabla,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	string_append(&directorioDeLaTabla, "/");
	string_append(&directorioDeLaTabla, nombreDeLaTabla);
	mkdir(directorioDeLaTabla,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGERFS,"Directorio %s creado", directorioDeLaTabla);
	return EXIT_SUCCESS;
}

int crearMetadataParaLaTabla(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion){
	// Crear el archivo Metadata asociado al mismo.
	// Grabar en dicho archivo los parámetros pasados por el request.
	// Ejemplo de archivo de metadata:
	// CONSISTENCY=SC
	// PARTITIONS=4
	// COMPACTION_TIME=60000
	char* nombreDelArchivo=string_new();
	string_append(&nombreDelArchivo, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivo, "/Tables/");
	string_append(&nombreDelArchivo, nombreDeLaTabla);
	string_append(&nombreDelArchivo, "/Metadata");
	log_info(LOGGERFS,"Voy a crear el archivo %s con la metadata", nombreDelArchivo);

	FILE * archivoTemp = fopen(nombreDelArchivo,"w");
	fclose(archivoTemp);

	t_config* configuracion = config_create(nombreDelArchivo);
	config_set_value(configuracion, "CONSISTENCY", tipoDeConsistencia);
	config_set_value(configuracion, "PARTITIONS", string_itoa(numeroDeParticiones));
	config_set_value(configuracion, "COMPACTION_TIME", string_itoa(tiempoDeCompactacion));
	config_save(configuracion);
	config_destroy(configuracion);
	log_info(LOGGERFS,"Archivo %s con la metadata creado", nombreDelArchivo);
	free(nombreDelArchivo);
	return EXIT_SUCCESS;
}

int crearArchivosBinariosYAsignarBloques(char* nombreDeLaTabla,
		int numeroDeParticiones){
	// Crear los archivos binarios asociados a cada partición de la tabla
	// y asignar a cada uno un bloque
	FILE * archivoTemp;
	char* nombreDelBinario;
	int bloqueLibre;
	t_config* configuracion;
	char* cadenaTemp;

	for(int i=1;i<=numeroDeParticiones;i++){
		nombreDelBinario=string_new();
		string_append(&nombreDelBinario, configuracionDelFS.puntoDeMontaje);
		string_append(&nombreDelBinario, "/Tables/");
		string_append(&nombreDelBinario, nombreDeLaTabla);
		string_append(&nombreDelBinario, "/");
		string_append(&nombreDelBinario, string_itoa(i));
		string_append(&nombreDelBinario, ".bin");
		log_info(LOGGERFS,"Creando el archivo binario %s", nombreDelBinario);
		archivoTemp = fopen(nombreDelBinario,"w");
		fclose(archivoTemp);
		log_info(LOGGERFS,"Escribiendo en el archivo binario %s", nombreDelBinario);

		bloqueLibre=obtenerBloqueLibreDelBitMap();

		if(bloqueLibre!=-1){
			ocuparBloqueDelBitmap(bloqueLibre);
			// ejemplo del formato de cada bin
			// SIZE=250
			// BLOCKS=[40,21,82,3]
			configuracion = config_create(nombreDelBinario);
			config_set_value(configuracion, "SIZE", string_itoa(0));
			cadenaTemp=string_new();
			string_append(&cadenaTemp, "[");
			string_append(&cadenaTemp, string_itoa(bloqueLibre));
			string_append(&cadenaTemp, "]");
			config_set_value(configuracion, "PARTITIONS", cadenaTemp);
			config_save(configuracion);
			log_info(LOGGERFS,"Archivo %s binario creado", nombreDelBinario);
			config_destroy(configuracion);
			free(cadenaTemp);
			free(nombreDelBinario);
		}else{
			log_error(LOGGERFS,"No hay mas bloques libres");
			return EXIT_FAILURE;
			}

		}
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
		printf("Se esta intentando crear una tabla con un nombre que ya existia: %s\n", nombreDeLaTabla);
		return TABLA_YA_EXISTIA;
		}
	return TABLA_CREADA;
}
