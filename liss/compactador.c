/*
 * compactador.c
 *
 *  Created on: 17 may. 2019
 *      Author: franco
 */
#include "compactador.h"

int lanzarCompactador(){

	log_info(LOGGERFS,"Iniciando hilo de compactador de lissandra");
	int resultado = pthread_create( &threadCompactador, NULL, crearCompactadorLissandra, NULL);
	pthread_detach(threadCompactador);

	if(resultado){
		log_error(LOGGERFS,"Error al crear hilo del compactador de lissandra, return code: %d",
				resultado);
		exit(EXIT_FAILURE);
	}else{
		log_info(LOGGERFS,"Hilo compactador de lissandra creado exitosamente");
		return EXIT_SUCCESS;
	}
}

void* crearCompactadorLissandra(){
	log_info(LOGGERFS,"Iniciando compactador de lissandra");

	log_info(LOGGERFS,"[Compactador]Compactador listo");

	while(0){

	}

	log_info(LOGGERFS,"[Compactador]Finalizando compactador");
	pthread_exit(0);
}
