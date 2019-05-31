/*
 * api.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "api.h"

int lanzarConsola(){
	logger(escribir_loguear, l_info, "Se va a abrir la consola del Kernel");
	int resultadoDeCrearHilo = pthread_create( &threadConsola, NULL,
			funcionHiloConsola, "Hilo consola");
	if(resultadoDeCrearHilo){
		logger(escribir_loguear, l_error, "Error al crear el hilo de la consola, return code: %d",
				resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
	}else{
		logger(escribir_loguear, l_info,"La consola se creo exitosamente");
		return EXIT_SUCCESS;
	}
	return EXIT_SUCCESS;
return EXIT_SUCCESS;
}

int esperarAQueTermineLaConsola(){
	pthread_join(threadConsola, NULL);
	logger(escribir_loguear, l_info, "Finaliza la consola");
	return EXIT_SUCCESS;
}

int selectConsola(char* nombreDeLaTabla,uint16_t key){
//TODO
	return EXIT_SUCCESS;
}

int insertConsola(char* nombreDeLaTabla,uint16_t key,char* valor,long timestamp){
	//insert()
	return EXIT_SUCCESS;
}

int insertConsolaNoTime(char* nombreDeLaTabla,uint16_t key,char* valor){
	//insertNoTime()
	return EXIT_SUCCESS;
}

int createConsola(char* nombreDeLaTabla,char* tipoDeConsistencia,int numeroDeParticiones,int tiempoDeCompactacion){
	//create()
	return EXIT_SUCCESS;
}

int describeConsola(char* nombreTabla){
	//describe
	return EXIT_SUCCESS;
}

int describeConsolaAll(){
	//describeAll()
	return EXIT_SUCCESS;
}

int dropConsola(char* nombreTabla){
	//drop
	return EXIT_SUCCESS;
}

int realoadConfig(){
	//actualiza quantum, sleep y metadata_refresh del arch de config
	return EXIT_SUCCESS;
}

int journalConsola(){
	//journal
	return EXIT_SUCCESS;
}

int runConsola(char* path){
	//run
	return EXIT_SUCCESS;
}

void metricsConsola(){
	//metrics
}

int addConsola(int memnum, char* criterio){
	//add
	return EXIT_SUCCESS;
}

void *funcionHiloConsola(void *arg){
		char * linea;
		char *ret="Cerrando hilo";
		char** instruccion;
		char* ubicacionDelPunteroDeLaConsola;
		logger(escribir_loguear, l_info,"Consola o Sinsola... aqui vamos!");
		//printf("Si necesita saber las funciones que hay disponibles llame a la funcion \"man\"\n");
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
					logger(escribir_loguear, l_info,"Chau chau adios consola");
					return ret;
				}else{
				if((strcmp(instruccion[0],"select")==0) || (strcmp(instruccion[0],"SELECT")==0)){
					if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)){
						printf("Voy a hacer un select por consola de la tabla %s, con la key %d\n",instruccion[1],atoi(instruccion[2]));
						selectConsola(instruccion[1],atoi(instruccion[2]));//chequear si la tabla la conoce, si no da error TODO
					}else{
						printf("Faltan parametros para poder hacer un select\n");
						}
				}else{
				if((strcmp(instruccion[0],"insert")==0) || (strcmp(instruccion[0],"INSERT")==0)){
					if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)
							&&(instruccion[4]!=NULL)){
						printf("Voy a hacer un insert por consola de la tabla %s, con la key %d, el value %s, y el timestamp %d\n",
								instruccion[1],atoi(instruccion[2]),instruccion[3],atoi(instruccion[4]));
						insertConsola(instruccion[1],atoi(instruccion[2]),instruccion[3],atoi(instruccion[4]));//chequear si la tabla la conoce, si no da error TODO
					}else{
						if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&
								(instruccion[3]!=NULL)&&(instruccion[4]==NULL)){
							printf("Voy a hacer un insert por consola de la tabla %s, con la key %d, el value %s, y sin timestamp\n",
									instruccion[1],atoi(instruccion[2]),instruccion[3]);
							insertConsolaNoTime(instruccion[1],atoi(instruccion[2]),instruccion[3]);//chequear si la tabla la conoce, si no da error TODO
						}else{
							printf("Faltan parametros para poder hacer un insert\n");}
						}
				}else{
					if((strcmp(instruccion[0],"create")==0) || (strcmp(instruccion[0],"CREATE")==0)){
						if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)
							&&(instruccion[4]!=NULL)){
							printf("Voy a hacer un create por consola de la tabla %s, del tipo de consitencia %s, con %d particiones, y tiempo de compactacion de %d\n",
									instruccion[1],instruccion[2],atoi(instruccion[3]),atoi(instruccion[4]));
							createConsola(instruccion[1],instruccion[2],atoi(instruccion[3]),atoi(instruccion[4]));
						}else{
							printf("Faltan parametros para poder hacer un create\n");
							}
				}else{
					if((strcmp(instruccion[0],"describe")==0) || (strcmp(instruccion[0],"DESCRIBE")==0)){
						if((instruccion[1]!=NULL)){
							printf("Voy a hacer un describe por consola de la tabla %s\n", instruccion[1]);
							describeConsola(instruccion[1]);
						}else{
							printf("Voy a hacer un describe por consola de todas las tablas\n");
							describeConsolaAll();
							}
				}else{
					if((strcmp(instruccion[0],"drop")==0) || (strcmp(instruccion[0],"DROP")==0)){
						if((instruccion[1]!=NULL)){
							printf("Voy a hacer un drop de la tabla %s\n", instruccion[1]);
							dropConsola(instruccion[1]);//chequear si la tabla la conoce, si no da error TODO
						}else{
							printf("Faltan parametros para poder hacer un drop\n");
						}
				}else{
					if((strcmp(instruccion[0],"journal")==0) || (strcmp(instruccion[0], "JOURNAL")==0)){
						printf("Voy a enviar journal a todas las memorias")
						journalConsola();
				}else{
					if(strcmp(instruccion[0],"man")==0){
						man();
				}else{
					if(strcmp(instruccion[0],"reloadconfig")==0){
						reloadConfig();
				}else{
					if((strcmp(instruccion[0],"run")==0) || (strcmp(instruccion[0], "RUN")==0)){
						if((instruccion[1]!=NULL)){
							printf("Voy a ejecutar el LQL en el path %s%n", instruccion[1]);
							runConsola(instruccion[1]);//TODO el file esta en el filesystem de la maquina
						}else{
								printf("Te comiste el path\n");
							}
				}else{
					if((strcmp(instruccion[0],"metrics")==0) || (strcmp(instruccion[0], "METRICS")==0)){
						metricsConsola();
				}else{
					if((strcmp(instruccion[0],"add")==0) || (strcmp(instruccion[0], "ADD")==0)){
						if((instruccion[1]=="MEMORY")&&(instruccion[2]!=NULL)&&(instruccion[3]=="TO")
							&&(instruccion[4]!=NULL)){
						printf("Voy a hacer ADD a la memoria %i, en el criterio %s\n",
								atoi(instruccion[2]),instruccion[4]);
						addConsola(atoi(instruccion[2]),instruccion[4]);
						}else{
							printf("Faltaron parametros en el ADD");
						}
				}else{
					printf("Comando desconocido\n");
					}}}}}}}}}}}}
				free(instruccion);
				}
			free(linea);
			free(ubicacionDelPunteroDeLaConsola);
		}//Cierre del while(1)
		return ret;

}

char** parser_instruccion(char* linea){
	char** instruccion = string_split(linea, " ");
	return instruccion;
}
