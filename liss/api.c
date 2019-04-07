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
			if((strcmp(instruccion[0],"select")==0) || (strcmp(instruccion[0],"SELECT")==0)){
				if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)){
					printf("Voy a hacer un select por consola de la tabla %s, con la key %d\n",instruccion[1],atoi(instruccion[2]));
					consolaSelect(instruccion[1],atoi(instruccion[2]));
				}else{
					printf("Faltan parametros para poder hacer un select\n");
					}
			}else{
			if((strcmp(instruccion[0],"insert")==0) || (strcmp(instruccion[0],"INSERT")==0)){
				if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)
						&&(instruccion[4]!=NULL)){
					printf("Voy a hacer un insert por consola de la tabla %s, con la key %d, el value %s, y el timestamp %d\n",
							instruccion[1],atoi(instruccion[2]),instruccion[3],atoi(instruccion[4]));
					consolaInsert(instruccion[1],atoi(instruccion[2]),instruccion[3],atoi(instruccion[4]));
				}else{
					printf("Faltan parametros para poder hacer un insert\n");
					}
			}else{
				if((strcmp(instruccion[0],"create")==0) || (strcmp(instruccion[0],"CREATE")==0)){
					if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)
						&&(instruccion[4]!=NULL)){
						printf("Voy a hacer un create por consola de la tabla %s, del tipo de consitencia %s, con %d particiones, y tiempo de compactacion de %d\n",
								instruccion[1],instruccion[2],atoi(instruccion[3]),atoi(instruccion[4]));
						consolaCreate(instruccion[1],instruccion[2],atoi(instruccion[3]),atoi(instruccion[4]));
					}else{
						printf("Faltan parametros para poder hacer un create\n");
						}
			}else{
				if((strcmp(instruccion[0],"describe")==0) || (strcmp(instruccion[0],"DESCRIBE")==0)){
					if((instruccion[1]!=NULL)){
						printf("Voy a hacer un describe por consola de la tabla %s\n", instruccion[1]);
						consolaDescribe(instruccion[1]);
					}else{
						printf("Faltan parametros para poder hacer un describe\n");
						}
			}else{
				if((strcmp(instruccion[0],"drop")==0) || (strcmp(instruccion[0],"DROP")==0)){
					if((instruccion[1]!=NULL)){
						printf("Voy a hacer un drop de la tabla %s\n", instruccion[1]);
						consolaDrop(instruccion[1]);
					}else{
						printf("Faltan parametros para poder hacer un drop\n");
					}
			}else{
				printf("Comando desconocido\n");
				}}}}}}
			free(instruccion);
			}
		free(linea);
		free(ubicacionDelPunteroDeLaConsola);
	}//Cierre del while(1)
	return ret;
}

int esperarAQueTermineLaConsola(){
	pthread_join( threadConsola, NULL);
	log_info(LOGGERFS,"Hilo de consola finalizado");
	return EXIT_SUCCESS;
}

char** parser_instruccion(char* linea){
	char** instruccion = string_split(linea, " ");
	return instruccion;
}

int consolaSelect(char* nombreDeLaTabla,int key){
	return EXIT_SUCCESS;
}

int consolaInsert(char* nombreDeLaTabla,int key,char* valor,int timestamp){
	return EXIT_SUCCESS;
}

int consolaCreate(char* nombreDeLaTabla,char* tipoDeConsistencia,int numeroDeParticiones,int tiempoDeCompactacion){
	return EXIT_SUCCESS;
}

int consolaDescribe(char* nombreDeLaTabla){
	return EXIT_SUCCESS;
}

int consolaDrop(char* nombreDeLaTabla){
	return EXIT_SUCCESS;
}
