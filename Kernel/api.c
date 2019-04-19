/*
 * api.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "api.h"

int lanzarConsola(){
/*	log_info(LOG_KERNEL,"Iniciando hilo de consola");
	int resultadoDeCrearHilo = pthread_create( &threadConsola, NULL,
			funcionHiloConsola, "Hilo consola");
	if(resultadoDeCrearHilo){
		log_error(LOG_KERNEL,"Error al crear el hilo de la consola, return code: %d",
				resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
	}else{
		log_info(LOG_KERNEL,"La consola se creo exitosamente");
		return EXIT_SUCCESS;
	}
	return EXIT_SUCCESS;*/
return EXIT_SUCCESS;
}

int esperarAQueTermineLaConsola(){

	return EXIT_SUCCESS;
}
