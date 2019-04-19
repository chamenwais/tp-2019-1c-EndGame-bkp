/*
 * funcionesKernel.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "funcionesKernel.h"

void inicializarLogKernel(){
	LOG_KERNEL = log_create("Kernel.log","Kernel",true,LOG_LEVEL_INFO);
	log_info(LOG_KERNEL,"Log del Kernel iniciado");
	return;
}

int levantarConfiguracionInicialDelKernel(){

	char* ubicacionDelArchivoDeConfiguracion;
	ubicacionDelArchivoDeConfiguracion="kernel.config";

	t_config* configuracion = config_create(ubicacionDelArchivoDeConfiguracion);

	if(configuracion!=NULL){
		log_info(LOG_KERNEL,"El archivo de configuracion existe");
	}else{
		log_error(LOG_KERNEL,"No existe el archivo de configuracion en: %s",ubicacionDelArchivoDeConfiguracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	log_info(LOG_KERNEL,"Abriendo el archivo de configuracion del Kernel");

	//Recupero el IP de la Memoria
	if(!config_has_property(configuracion,"IP_MEMORIA")) {
		log_error(LOG_KERNEL,"No esta el IP_MEMORIA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	char* ipMemoria = config_get_string_value(configuracion,"IP_MEMORIA");
	configKernel.ipMemoria = malloc(strlen(ipMemoria)+1);
	strcpy(configKernel.ipMemoria, ipMemoria);
	log_info(LOG_KERNEL,"IP_MEMORIA del archivo de configuracion del Kernel recuperado: %s",
			configKernel.ipMemoria);
	//Recupero el puerto de la memoria
	if(!config_has_property(configuracion,"PUERTO_MEMORIA")) {
		log_error(LOG_KERNEL,"No esta el PUERTO_MEMORIA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.puertoMemoria = config_get_int_value(configuracion, "PUERTO_MEMORIA");
	log_info(LOG_KERNEL,"PUERTO_MEMORIA del archivo de configuracion del Kernel recuperado: %d",
		configKernel.puertoMemoria);
	//Recupero el quantum
	if(!config_has_property(configuracion,"QUANTUM")) {
		log_error(LOG_KERNEL,"No esta el QUANTUM en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.quantum = config_get_int_value(configuracion,"QUANTUM");
	log_info(LOG_KERNEL,"Quantum del archivo de configuracion del Kernel recuperado: %d",
			configKernel.quantum);

	//Recupero el valor de multiprocesamiento
	if(!config_has_property(configuracion,"MULTIPROCESAMIENTO")) {
		log_error(LOG_KERNEL,"No esta el MULTIPROCESAMIENTO en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.multiprocesamiento = config_get_int_value(configuracion,"MULTIPROCESAMIENTO");
	log_info(LOG_KERNEL,"Multiprocesamiento del archivo de configuracion del Kernel recuperado: %d",
			configKernel.multiprocesamiento);

	//Recupero el tiempo refresh metadata
	if(!config_has_property(configuracion,"REFRESH_METADATA")) {
		log_error(LOG_KERNEL,"No esta el REFRESH_METADATA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.refreshMetadata = config_get_int_value(configuracion,"REFRESH_METADATA");
	log_info(LOG_KERNEL,"refresh metadata del archivo de configuracion del Kernel recuperado: %d",
			configKernel.refreshMetadata);

	//Recupero el tiempo de retardo del ciclo
		if(!config_has_property(configuracion,"RETARDO_CICLO")) {
			log_error(LOG_KERNEL,"No esta el RETARDO_CICLO en el archivo de configuracion");
			config_destroy(configuracion);
			log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
			return EXIT_FAILURE;
			}
		configKernel.retardoCiclo = config_get_int_value(configuracion,"RETARDO_CICLO");
		log_info(LOG_KERNEL,"retardo_ciclo del archivo de configuracion del Kernel recuperado: %d",
				configKernel.retardoCiclo);

	config_destroy(configuracion);
	log_info(LOG_KERNEL,"Configuracion del Kernel recuperada exitosamente");

	return EXIT_SUCCESS;
	}

int conectarse_con_memoria(void){
	log_info(LOG_KERNEL, "Conectandose a la primera memoria en ip %s y puerto %i", configKernel.ipMemoria, configKernel.puertoMemoria);
	int socket_mem = conectarseA(configKernel.ipMemoria, configKernel.puertoMemoria);
	//validar que el socket sea <0
	enviarHandshake(KERNEL, MEMORIA, socket_mem);

	return socket_mem;
}

