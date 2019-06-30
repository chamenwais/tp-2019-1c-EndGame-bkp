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

void crear_proceso(char* linea) {
	logger(escribir_loguear, l_info, "Ingresa al Kernel el pedido %s", linea);
	tp_lql_pcb nuevo_LQL = crear_PCB_sin_file(linea);
	pthread_mutex_lock(&mutex_New);
	list_add(listaNew, nuevo_LQL); //agregar LQL a cola de NEW
	logger(escribir_loguear, l_debug,
			"Se agrega el nuevo LQL a la cola de NEW");
	pthread_mutex_unlock(&mutex_New);
	sem_post(&NEW); //habilito PLP
}

int selectConsola(char* linea){
	crear_proceso(linea);
	return EXIT_SUCCESS;
}

int insertConsola(char* linea){
	crear_proceso(linea);
	return EXIT_SUCCESS;
}

int insertConsolaNoTime(char* linea){
	crear_proceso(linea);
	return EXIT_SUCCESS;
}

int createConsola(char* linea){
	crear_proceso(linea);
	return EXIT_SUCCESS;
}

int describeConsola(char* linea){
	crear_proceso(linea);
	return EXIT_SUCCESS;
}

int describeConsolaAll(char *linea){
	crear_proceso(linea);
	return EXIT_SUCCESS;
}

int dropConsola(char* linea){
	crear_proceso(linea);
	return EXIT_SUCCESS;
}


void journalConsola(char* linea){
	int max = list_size(listaMemConectadas);
	int i;
	for (i = 0; i < max; ++i) {
		tp_memo_del_pool_kernel memo = list_get(listaMemConectadas, i);
		logger(escribir_loguear, l_debug, "Se le solicita a la memoria %i un Journal", memo->numero_memoria);
		prot_enviar_journal(memo->socket);
		enum MENSAJES respuesta = prot_recibir_respuesta_journal(memo->socket);
		if(respuesta == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "La memoria %i realizo un JOURNAL correctamente", memo->numero_memoria);
		}else{
			logger(escribir_loguear, l_info, "Error al realizar el JOURNAL en la memoria %i", memo->numero_memoria);
		}
	}
}

int runConsola(char* path){
	logger(escribir_loguear, l_info, "Ingresa al Kernel un archivo LQL en el path %s", path);
	tp_lql_pcb nuevo_LQL = crear_PCB(path); //crea el PCB con path y ultima linea parseada
	pthread_mutex_lock(&mutex_New);
	list_add(listaNew, nuevo_LQL);//agregar LQL a cola de NEW
	logger(escribir_loguear, l_debug, "Se agrega el nuevo LQL a la cola de NEW");
	pthread_mutex_unlock(&mutex_New);
	sem_post(&NEW);//habilito PLP

	return EXIT_SUCCESS;
}

void metricsConsola(){
	//metrics
}

int addConsola(int memnum, char* criterio){
	logger(escribir_loguear, l_info, "Se va a agregar el criterio %s a la memoria %i", criterio, memnum);
	if(existeMemoria(memnum)){
		tp_memo_del_pool_kernel memoria = buscar_memorias_segun_numero(listaMemConectadas, memnum);
		if((strcmp(criterio, "SC"))==0) {
			pthread_mutex_lock(&mutex_SC);
			list_add(listaSC, memoria);
			pthread_mutex_unlock(&mutex_SC);
			printf("Se agrego la memoria %i al criterio SC\n", memnum);
		}else if((strcmp(criterio, "EC"))==0){
			pthread_mutex_lock(&mutex_EC);
			list_add(listaEC, memoria);
			pthread_mutex_unlock(&mutex_EC);
			printf("Se agrego la memoria %i al criterio EC\n", memnum);
		}else if((strcmp(criterio, "HC"))==0){
			pthread_mutex_lock(&mutex_HC);
			list_add(listaHC, memoria);
			pthread_mutex_unlock(&mutex_HC);
			printf("Se agrego la memoria %i al criterio HC\n", memnum);
			//TODO hacer un journal para cada memoria de la lista
		}else{
			printf("Pifiaste el criterio amigue");
		}
	}else{
		logger(escribir_loguear, l_error, "La memoria %i no esta conectada", memnum);
	}
	return EXIT_SUCCESS;
}

bool existeMemoria(int numero){
	bool coincideElNumero(void* nodo){
			if(((tp_memo_del_pool_kernel) nodo)->numero_memoria == numero){
				return true;
				}
				return false;
			}
	return list_any_satisfy(listaMemConectadas, coincideElNumero);
}

tp_lql_pcb crear_PCB(char* path){
	tp_lql_pcb nuevo_LQL = calloc(1, sizeof(t_lql_pcb));
	logger(escribir_loguear, l_info, "Se crea el PCB para el LQL en el path %s\n", path);
	nuevo_LQL->path = malloc(strlen(path)+1);
	strcpy(nuevo_LQL->path, path);
	nuevo_LQL->lista = obtener_lista_lineas_desde_archivo(nuevo_LQL->path);
	return nuevo_LQL;
}

tp_lql_pcb crear_PCB_sin_file(char* linea){
	tp_lql_pcb nuevo_LQL = calloc(1, sizeof(t_lql_pcb));
	char* path = string_itoa(path_api);
	logger(escribir_loguear, l_info, "Se crea el PCB para el LQL en el path %s pedido por consola\n", path);
	nuevo_LQL->path = malloc(strlen(path)+1);
	strcpy(nuevo_LQL->path, path);
	nuevo_LQL->lista = obtener_lista_linea(linea);
	path_api++;
	return nuevo_LQL;
}

int man(){
	printf("Mostrando funciones disponibles de la consola:\n");
	printf("1) \"exit\" finaliza el programa\n");
	printf("2) SELECT [NOMBRE_TABLA] [KEY]\n");
	printf("3) INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]\n");
	printf("4) CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]\n");
	printf("5) DESCRIBE [NOMBRE_TABLA]\n");
	printf("6) DESCRIBE, da la info de todas las tablas\n");
	printf("7) DROP [NOMBRE_TABLA]\n");
	printf("8) \"reloadconfig\", recarga la configuracion del los archivos al sistema\n");
	printf("9) \"METRICS\", informa las metricas actuales del Kernel\n");
	printf("10) ADD MEMORY [NUMERO] TO [CRITERIO]\n");
	return EXIT_SUCCESS;
}

int reloadConfig(){ //actualiza quantum, sleep y metadata_refresh del arch de config
	//Actualiza los datos lissandra con las modificaciones que se le hayan hecho a los achivos de configuracion
	/* Solamente se pueden actualizar los valores:
	 * retardo
	 * tiempo_dump
	 * en tiempo de ejecucion
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

	//Recupero el tiempo dump
	if(!config_has_property(configuracion,"TIEMPO_DUMP")) {
		log_error(LOGGERFS,"No esta el TIEMPO_DUMP en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	int tiempoDump;
	tiempoDump = config_get_int_value(configuracion,"TIEMPO_DUMP");
	actualizarTiempoDump(tiempoDump);
	log_info(LOGGERFS,"Tiempo dump del archivo de configuracion del FS recuperado: %d",
			obtenerTiempoDump());

	//Recupero el retardo
	if(!config_has_property(configuracion,"RETARDO")) {
		log_error(LOGGERFS,"No esta el RETARDO en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	int retardo;
	retardo = config_get_int_value(configuracion,"RETARDO");
	actualizarRetardo(retardo);
	log_info(LOGGERFS,"Retardo del archivo de configuracion del FS recuperado: %d",
			obtenerRetardo());

	config_destroy(configuracion);
	log_info(LOGGERFS,"Configuracion del FS recuperada exitosamente");*/

	return EXIT_SUCCESS;
}


void *funcionHiloConsola(void *arg){
		char * linea;
		char *ret="Cerrando hilo";
		char** instruccion;
		char* ubicacionDelPunteroDeLaConsola;
		logger(escribir_loguear, l_warning,"Consola o Sinsola... aqui vamos!");
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
					logger(escribir_loguear, l_warning,"Chau chau adios consola");
					return ret;
				}else{
				if((strcmp(instruccion[0],"select")==0) || (strcmp(instruccion[0],"SELECT")==0)){
					if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)){
						printf("Voy a hacer un select por consola de la tabla %s, con la key %d\n",instruccion[1],atoi(instruccion[2]));
						selectConsola(linea);
					}else{
						printf("Faltan parametros para poder hacer un select\n");
						}
				}else{
				if((strcmp(instruccion[0],"insert")==0) || (strcmp(instruccion[0],"INSERT")==0)){
					if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)
							&&(instruccion[4]!=NULL)){
						printf("Voy a hacer un insert por consola de la tabla %s, con la key %d, el value %s, y el timestamp %d\n",
								instruccion[1],atoi(instruccion[2]),instruccion[3],atoi(instruccion[4]));
						insertConsola(linea);
					}else{
						if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&
								(instruccion[3]!=NULL)&&(instruccion[4]==NULL)){
							printf("Voy a hacer un insert por consola de la tabla %s, con la key %d, el value %s, y sin timestamp\n",
									instruccion[1],atoi(instruccion[2]),instruccion[3]);
							insertConsolaNoTime(linea);
						}else{
							printf("Faltan parametros para poder hacer un insert\n");}
						}
				}else{
					if((strcmp(instruccion[0],"create")==0) || (strcmp(instruccion[0],"CREATE")==0)){
						if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)&&(instruccion[3]!=NULL)
							&&(instruccion[4]!=NULL)){
							printf("Voy a hacer un create por consola de la tabla %s, del tipo de consitencia %s, con %d particiones, y tiempo de compactacion de %d\n",
									instruccion[1],instruccion[2],atoi(instruccion[3]),atoi(instruccion[4]));
							createConsola(linea);
						}else{
							printf("Faltan parametros para poder hacer un create\n");
							}
				}else{
					if((strcmp(instruccion[0],"describe")==0) || (strcmp(instruccion[0],"DESCRIBE")==0)){
						if((instruccion[1]!=NULL)){
							printf("Voy a hacer un describe por consola de la tabla %s\n", instruccion[1]);
							describeConsola(linea);
						}else{
							printf("Voy a hacer un describe por consola de todas las tablas\n");
							describeConsolaAll(linea);
							}
				}else{
					if((strcmp(instruccion[0],"drop")==0) || (strcmp(instruccion[0],"DROP")==0)){
						if((instruccion[1]!=NULL)){
							printf("Voy a hacer un drop de la tabla %s\n", instruccion[1]);
							dropConsola(linea);
						}else{
							printf("Faltan parametros para poder hacer un drop\n");
						}
				}else{
					if((strcmp(instruccion[0],"journal")==0) || (strcmp(instruccion[0], "JOURNAL")==0)){
						printf("Voy a enviar journal a todas las memorias");
						journalConsola(linea);
				}else{
					if(strcmp(instruccion[0],"man")==0){
						man();
				}else{
					if(strcmp(instruccion[0],"reloadConfig")==0){
						reloadConfig();
				}else{
					if((strcmp(instruccion[0],"run")==0) || (strcmp(instruccion[0], "RUN")==0)){
						if((instruccion[1]!=NULL)){
							printf("Voy a ejecutar el LQL en el path %s\n", instruccion[1]);
							runConsola(instruccion[1]);
						}else{
								printf("Te comiste el path\n");
							}
				}else{
					if((strcmp(instruccion[0],"metrics")==0) || (strcmp(instruccion[0], "METRICS")==0)){
						metricsConsola();
				}else{
					if((strcmp(instruccion[0],"add")==0) || (strcmp(instruccion[0], "ADD")==0)){
						if((strcmp(instruccion[1], "MEMORY")==0)&&(instruccion[2]!=NULL)&&((strcmp(instruccion[3], "TO"))==0)
							&&(instruccion[4]!=NULL)){
						printf("Voy a hacer ADD a la memoria %i, en el criterio %s\n",
								atoi(instruccion[2]),instruccion[4]);
						addConsola(atoi(instruccion[2]),instruccion[4]);
						}else{
							printf("Faltaron parametros en el ADD\n");
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
