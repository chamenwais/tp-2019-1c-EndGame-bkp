/*
 ============================================================================
 Name        : Memoria.c
 Author      : Tamara-Francisco
 Version     :
 Copyright   : Your copyright notice
 Description : Proceso memoria
 ============================================================================
 */
#include "Memoria.h"

int main(int argc, char ** argv) {

	iniciar_logger();
	iniciar_config(argc,argv);
	leer_config();
	configurar_signals();

	logger(escribir_loguear, l_debug,"Me meto en un bucle infinito...");
	while(true){
		//TODO Este bucle infinito será reemplazado por el hilo de conexión y el de la consola
	}

	return EXIT_SUCCESS;
}
