/*
 * fs.c
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#include "fs.h"

void inicializarLogDelFS(){
	LOGGERFS = log_create("FileSystem.log","FileSystem",true,LOG_LEVEL_INFO);
	log_info(LOGGERFS,"Log del FileSystem iniciado");
	return;
}

int obtenerPathDeMontajeDelPrograma(int argc,char** argv){
	pathDeMontajeDelPrograma=string_new();
	if(argc>1){
		log_info(LOGGERFS,"Path de trabajo recibido por parametro");
		string_append(&pathDeMontajeDelPrograma, argv[1]);
		string_append(&pathDeMontajeDelPrograma, "/");
	}else{
		log_info(LOGGERFS,"El path de trabajo es el local");
		}
	return EXIT_SUCCESS;
	}

int levantarConfiguracionInicialDelFS(){
	/* Ejemplo de los datos a levantar:
	 * PUERTO_ESCUCHA=5003
	 * PUNTO_MONTAJE=/home/utnso/endGame/tp-2019-1c-EndGame/liss/Debug
	 * RETARDO=500
	 * TAMAÑO_VALUE=4
	 * TIEMPO_DUMP=5000*/

	char* pathCompleto;
	pathCompleto=string_new();
	string_append(&pathCompleto, pathDeMontajeDelPrograma);
	string_append(&pathCompleto, "configuracionFS.cfg");

	t_config* configuracion = config_create(pathCompleto);

	if(configuracion!=NULL){
		log_info(LOGGERFS,"El archivo de configuracion existe");
	}else{
		log_error(LOGGERFS,"No existe el archivo de configuracion en: %s",pathCompleto);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	log_info(LOGGERFS,"Abriendo el archivo de configuracion del FS, su ubicacion es: %s",pathCompleto);

	//Recupero el puerto de escucha
	if(!config_has_property(configuracion,"PUERTO_ESCUCHA")) {
		log_error(LOGGERFS,"No esta el PUERTO_ESCUCHA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.puertoEscucha = config_get_int_value(configuracion,"PUERTO_ESCUCHA");
	log_info(LOGGERFS,"Puerto de escucha del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.puertoEscucha);
	//Recupero el punto de montaje
	if(!config_has_property(configuracion,"PUNTO_MONTAJE")) {
		log_error(LOGGERFS,"No esta el PUNTO_MONTAJE en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	char* puntoDeMontaje = config_get_string_value(configuracion,"PUNTO_MONTAJE");
	configuracionDelFS.puntoDeMontaje = malloc(strlen(puntoDeMontaje)+1);
	strcpy(configuracionDelFS.puntoDeMontaje,puntoDeMontaje);
	log_info(LOGGERFS,"Punto de montaje del archivo de configuracion del FS recuperado: \"%s\"",
			configuracionDelFS.puntoDeMontaje);
	//Recupero el retardo
	if(!config_has_property(configuracion,"RETARDO")) {
		log_error(LOGGERFS,"No esta el RETARDO en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.retardo = config_get_int_value(configuracion,"RETARDO");
	log_info(LOGGERFS,"Retardo del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.retardo);

	//Recupero el tamaño value
	if(!config_has_property(configuracion,"TAMAÑO_VALUE")) {
		log_error(LOGGERFS,"No esta el TAMAÑO_VALUE en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.sizeValue = config_get_int_value(configuracion,"TAMAÑO_VALUE");
	log_info(LOGGERFS,"Tamaño value del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.sizeValue);

	//Recupero el tiempo dump
	if(!config_has_property(configuracion,"TIEMPO_DUMP")) {
		log_error(LOGGERFS,"No esta el TIEMPO_DUMP en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.tiempoDump = config_get_int_value(configuracion,"TIEMPO_DUMP");
	log_info(LOGGERFS,"Tiempo dump del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.tiempoDump);

	config_destroy(configuracion);
	log_info(LOGGERFS,"Configuracion del FS recuperada exitosamente");

	return EXIT_SUCCESS;
	}

int levantarMetadataDelFS(){
	/* Ejemplo de los datos a levantar:
	BLOCK_SIZE=64
	BLOCKS=5192
	MAGIC_NUMBER=LISSANDRA*/

	directorioConLaMetadata=string_new();
	string_append(&directorioConLaMetadata, configuracionDelFS.puntoDeMontaje);
	string_append(&directorioConLaMetadata, "/Metadata/Metadata.bin");
	t_config* metadataConfig = config_create(directorioConLaMetadata);

	if(metadataConfig!=NULL){
		log_info(LOGGERFS,"El archivo de de metadata del FS existe");
	}else{
		log_error(LOGGERFS,"No existe el archivo de metadata en: %s",directorioConLaMetadata);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	log_info(LOGGERFS,"Abriendo el archivo de metadata del FS, su ubicacion es: %s",directorioConLaMetadata);

	//Recupero el block size
	if(!config_has_property(metadataConfig,"BLOCK_SIZE")) {
		log_error(LOGGERFS,"No esta el BLOCK_SIZE en el archivo de metadata");
		config_destroy(metadataConfig);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	metadataDelFS.blockSize = config_get_int_value(metadataConfig,"BLOCK_SIZE");
	log_info(LOGGERFS,"Block size de la metadata del FS recuperado: %d",
			metadataDelFS.blockSize);

	//Recupero la cantidad de bloques
	if(!config_has_property(metadataConfig,"BLOCKS")) {
		log_error(LOGGERFS,"No esta el valor BLOCKS en el archivo de metadata");
		config_destroy(metadataConfig);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	metadataDelFS.blocks = config_get_int_value(metadataConfig,"BLOCKS");
	log_info(LOGGERFS,"Blocks de la metadata del FS recuperado: %d",
			metadataDelFS.blocks);


	//Recupero el magic number
	if(!config_has_property(metadataConfig,"MAGIC_NUMBER")) {
		log_error(LOGGERFS,"No esta el MAGIC_NUMBER en el archivo de configuracion");
		config_destroy(metadataConfig);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	char* magicNumberTemp = config_get_string_value(metadataConfig,"MAGIC_NUMBER");
	metadataDelFS.magicNumber = malloc(strlen(magicNumberTemp)+1);
	strcpy(metadataDelFS.magicNumber,magicNumberTemp);
	log_info(LOGGERFS,"Magic number de la metadata recuperada: \"%s\"",
			metadataDelFS.magicNumber);

	config_destroy(metadataConfig);
	log_info(LOGGERFS,"Configuracion del FS recuperada exitosamente");

	return EXIT_SUCCESS;
	}

int actualizarTiempoDump(int tiempoDump){
	//La variable global "configuracionDelFS.tiempoDump" solo debe ser modificada por medio
	//de esta funcion
	pthread_mutex_lock(&mutexVariableTiempoDump);
	configuracionDelFS.tiempoDump=tiempoDump;
	pthread_mutex_unlock(&mutexVariableTiempoDump);
	return EXIT_SUCCESS;
	}

int obtenerTiempoDump(){
	//La variable global "configuracionDelFS.tiempoDump" solo debe ser leida por medio
	//de esta funcion
	int tiempoDump;
	pthread_mutex_lock(&mutexVariableTiempoDump);
	tiempoDump=configuracionDelFS.tiempoDump;
	pthread_mutex_unlock(&mutexVariableTiempoDump);
	return tiempoDump;
	}

int actualizarRetardo(int retardo){
	//La variable global "configuracionDelFS.retardo" solo debe ser modificada por medio
	//de esta funcion
	pthread_mutex_lock(&mutexVariableRetardo);
	configuracionDelFS.retardo=retardo;
	pthread_mutex_unlock(&mutexVariableRetardo);
	return EXIT_SUCCESS;
	}

int obtenerRetardo(){
	//La variable global "configuracionDelFS.retardo" solo debe ser leida por medio
	//de esta funcion
	int retardo;
	pthread_mutex_lock(&mutexVariableRetardo);
	retardo=configuracionDelFS.retardo;
	pthread_mutex_unlock(&mutexVariableRetardo);
	return retardo;
	}

int imprimirMetadataDelFS(){

	return EXIT_SUCCESS;
	}

int imprimirConfiguracionDelFS(){
	printf("Puerto de escucha: %d\n",configuracionDelFS.puertoEscucha);
	printf("Punto de montaje: \"%s\"\n",configuracionDelFS.puntoDeMontaje);
	printf("Retardo: %d\n",obtenerRetardo());
	printf("Size value: %d\n",configuracionDelFS.sizeValue);
	printf("Tiempo de dump: %d\n",obtenerTiempoDump());
	return EXIT_SUCCESS;
}
