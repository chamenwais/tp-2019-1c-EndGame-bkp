/*
 * api.c
 *
 *  Created on: 5 abr. 2019
 *      Author: utnso
 */

#include "api.h"

int lanzarConsola(){
	log_info(LOGGERFS,"Iniciando hilo de consola");
	int resultadoDeCrearHilo = pthread_create( &threadConsola, NULL, funcionHiloConsola, "Hilo consola");
	if(resultadoDeCrearHilo){
		log_error(LOGGERFS,"Error al crear el hilo, return code: %d",resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
	}else{
		log_info(LOGGERFS,"La consola se creo exitosamente");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

void *funcionHiloConsola(void *arg){
	char * linea;
	char *ret="Cerrando hilo";
	char** instruccion;
	char* ubicacionDelPunteroDeLaConsola;
	log_info(LOGGERFS,"Consola lista");
	printf("\n");
	while(1){
		ubicacionDelPunteroDeLaConsola=string_new();
		string_append(&ubicacionDelPunteroDeLaConsola,"$ ");
		linea = readline(ubicacionDelPunteroDeLaConsola);
		if(strlen(linea)>0){
			add_history(linea);
			instruccion=NULL;
			instruccion = parser_instruccion(linea);
			if(instruccion[0] == NULL) continue;
			if(strcmp(instruccion[0],"exit")==0){//Mata al hilo de la consola
				for(int p=0;instruccion[p]!=NULL;p++) free(instruccion[p]);
				free(instruccion);
				free(linea);
				log_info(LOGGERFS,"Cerrando consola");
				return ret;
			}else{
				printf("Comando desconocido\n");
				}
			free(instruccion);
			}
		free(linea);
		free(ubicacionDelPunteroDeLaConsola);
	}//Cierre del while(1)
	return ret;
}


