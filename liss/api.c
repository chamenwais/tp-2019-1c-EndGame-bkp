/*
 * api.c
 *
 *  Created on: 5 abr. 2019
 *      Author: utnso
 */

#include "api.h"

int lanzarConsola(){
	sleep(2);
	log_info(LOGGERFS,"Iniciando hilo de consola");
	int resultadoDeCrearHilo = pthread_create( &threadConsola, NULL,
			funcionHiloConsola, "Hilo consola");
	if(resultadoDeCrearHilo){
		log_error(LOGGERFS,"Error al crear el hilo de la consola, return code: %d",
				resultadoDeCrearHilo);
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
	log_info(LOGGERFS,"Consola lista");
	printf("Si necesita saber las funciones que hay disponibles llame a la funcion \"man\"\n");
	while(1){
		linea = readline("$ ");
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
				setearEstadoDeFinalizacionDelSistema(true);
				return ret;
			}else{
			if((strcmp(instruccion[0],"select")==0) || (strcmp(instruccion[0],"SELECT")==0)){
				if((instruccion[1]!=NULL)&&(instruccion[2]!=NULL)){
					printf("Voy a hacer un select por consola de la tabla %s, con la key %d\n",instruccion[1],atoi(instruccion[2]));
					consolaSelect(instruccion[1],atoi(instruccion[2]));
				}else{
					if(instruccion[1]==NULL){
					printf("Faltan parametros para poder hacer un select\n");
					}
				}
			}else{
			if((strcmp(instruccion[0],"insert")==0) || (strcmp(instruccion[0],"INSERT")==0)){
				char** instruccionesAux = string_split(linea, "\"");
				/* De: INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]
				 * Me queda separado en:
				 * INSERT [NOMBRE_TABLA] [KEY]
				 * [VALUE]
				 * [Timestamp]
				 * */
				char** instrucionLocal = string_split(instruccionesAux[0], " ");
				/* De: INSERT [NOMBRE_TABLA] [KEY]
				 * Me queda separado en:
				 * INSERT
				 * [NOMBRE_TABLA]
				 * [KEY]
				 * */
				if((instrucionLocal[1]!=NULL)&&(instrucionLocal[2]!=NULL)&&(instruccionesAux[1]!=NULL)
						&&(instruccionesAux[2]!=NULL)){
					printf("Voy a hacer un insert por consola de la tabla %s, con la key %d, el value %s, y el timestamp %d\n",
							instrucionLocal[1],atoi(instrucionLocal[2]),instruccionesAux[1],atoi(instruccionesAux[2]));
					consolaInsert(instrucionLocal[1],atoi(instrucionLocal[2]),instruccionesAux[1],atoi(instruccionesAux[2]));
				}else{
					if((instrucionLocal[1]!=NULL)&&(instrucionLocal[2]!=NULL)&&
							(instruccionesAux[1]!=NULL)&&(instruccionesAux[2]==NULL)){
						printf("Voy a hacer un insert por consola de la tabla %s, con la key %d, el value %s, y sin timestamp\n",
								instrucionLocal[1],atoi(instrucionLocal[2]),instruccionesAux[1]);
						consolaInsertSinTime(instrucionLocal[1],atoi(instrucionLocal[2]),instruccionesAux[1]);
					}else{
						printf("Faltan parametros para poder hacer un insert\n");}
					}
				for(int j=0;instruccionesAux[j]!=NULL;j++)
					free(instruccionesAux[j]);
				free(instruccionesAux);
				for(int j=0;instrucionLocal[j]!=NULL;j++)
					free(instrucionLocal[j]);
				free(instrucionLocal);
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
						consolaDescribeDeTabla(instruccion[1]);
					}else{
						printf("Voy a hacer un describe por consola de todas las tablas\n");
						consolaDescribe(obtenerTodosLosDescriptores());
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
				if(strcmp(instruccion[0],"config")==0){
					imprimirConfiguracionDelSistema();
			}else{
				if(strcmp(instruccion[0],"man")==0){
					man();
			}else{
				if(strcmp(instruccion[0],"reloadconfig")==0){
					reloadConfig();
			}else{
				if(strcmp(instruccion[0],"bitmap")==0){
					imprimirEstadoDelBitmap();
			}else{
				if(strcmp(instruccion[0],"existeLaTabla")==0){
					exiteLaTabla(instruccion[1]);
			}else{
				if(strcmp(instruccion[0],"pmemtable")==0){
					imprimirMemtableEnPantalla();
			}else{
				if((strcmp(instruccion[0],"dumpear")==0)&&(instruccion[1]!=NULL)){
					pthread_mutex_lock(&mutexDeDump);
					list_iterate(memTable, dumpearAEseNodo);
					vaciarMemTable();
					memTable=list_create();
					pthread_mutex_unlock(&mutexDeDump);
			}else{
				printf("Comando desconocido\n");
				}}}}}}}}}}}}}
			for(int j=0;instruccion[j]!=NULL;j++)
				free(instruccion[j]);
			free(instruccion);
			}
		free(linea);
	}//Cierre del while(1)
	return ret;
}

int esperarPorHilos(){
	log_info(LOGGERFS,"Esperando a threadConsola");
	pthread_join( threadConsola, NULL);
	log_info(LOGGERFS,"Hilo de consola finalizado");
	log_info(LOGGERFS,"Esperando a threadDumps");
	pthread_join( threadDumps, NULL);
	log_info(LOGGERFS,"Hilo de threadDumps finalizado");
	log_info(LOGGERFS,"Esperando a threadServer");
	pthread_join( threadServer, NULL);
	log_info(LOGGERFS,"Hilo de threadServer finalizado");
	log_info(LOGGERFS,"Esperando a threadCompactador");
	pthread_join( threadCompactador, NULL);
	log_info(LOGGERFS,"Hilo de threadCompactador finalizado");
	log_info(LOGGERFS,"Esperando a threadMonitoreadorDeArchivos");
	pthread_join( threadMonitoreadorDeArchivos, NULL);
	log_info(LOGGERFS,"Hilo de threadMonitoreadorDeArchivos finalizado");
	log_info(LOGGERFS,"Todos los hilos han finalizado");
	return EXIT_SUCCESS;
}

char** parser_instruccion(char* linea){
	char** instruccion = string_split(linea, " ");
	return instruccion;
}

int consolaSelect(char* nombreDeLaTabla,uint16_t key){
	tp_nodoDeLaTabla nodo = selectf(nombreDeLaTabla, key);
	if(nodo!=NULL){
		if(nodo->resultado==KEY_NO_EXISTE ){
			log_info(LOGGERFS,"No hay ningun valor de esa key(%d) en la tabla seleccionada(%s)",
				key, nombreDeLaTabla);
			printf("No hay ningun valor de esa key(%d) en la tabla seleccionada(%s)\n",
				key, nombreDeLaTabla);
		}else{
			if(nodo->resultado==KEY_OBTENIDA){
				log_info(LOGGERFS,"Resultado del select, Value: %s, Timpestamp: %d, para la key %d, de la tabla %s",
					nodo->value, nodo->timeStamp, key, nombreDeLaTabla);
				printf("Resultado del select, Value: %s, Timpestamp: %d, para la key %d, de la tabla %s\n",
					nodo->value, nodo->timeStamp, key, nombreDeLaTabla);
			}else{
				log_error(LOGGERFS,"Resultado insaperado, alerta!!!");
			}
		}
		//free(nodo->value);
		//free(nodo);
	}else{
		log_error(LOGGERFS,"Resultado insaperado, alerta!!!, resultado del selectf==NULL");
	}
	return EXIT_SUCCESS;
}

int consolaInsert(char* nombreDeLaTabla,uint16_t key,char* valor,long timestamp){
	insert(nombreDeLaTabla,key,valor,timestamp);
	return EXIT_SUCCESS;
}

int consolaInsertSinTime(char* nombreDeLaTabla,uint16_t key,char* valor){
	insertSinTime(nombreDeLaTabla,key,valor);
	return EXIT_SUCCESS;
}

int consolaCreate(char* nombreDeLaTabla,char* tipoDeConsistencia,int numeroDeParticiones,int tiempoDeCompactacion){
	log_info(LOGGERFS,"Haciendo un create por consola");
	int resultado = (create(nombreDeLaTabla, tipoDeConsistencia, numeroDeParticiones, tiempoDeCompactacion));
	log_info(LOGGERFS,"Create de consola finalizado con el valor: %d", resultado);
	if(resultado==TABLA_CREADA)
		log_info(LOGGERFS,"Tabla creada exitosamente");
	return resultado;
}

void imprimirMetadataDescribeAll(tp_describe_rta unaMetadata){
	printf("Info de la tabla: %s\n",unaMetadata->nombre);
	printf("Numero de particiones: %d\n",unaMetadata->particiones);
	printf("Tipo de consistencia: %s\n",unaMetadata->consistencia);
	printf("Tiempo de compactacion: %d\n",unaMetadata->tiempoDeCompactacion);
	printf("\n");
}

int consolaDescribe(t_list* descriptores){
	log_info(LOGGERFS,"Haciendo un describe por consola de todas las tablas");
	if(descriptores==NULL){
		log_info(LOGGERFS,"No se hizo describe porque no hay tablas en el fs");
		return EXIT_SUCCESS;//es EXIT_FAILURE que no haya tablas?
	} else{
		list_iterate(descriptores,imprimirMetadataDescribeAll);
		liberarYDestruirTablaDeMetadata(descriptores);
		return EXIT_SUCCESS;
	}
}

int consolaDescribeDeTabla(char* nombreDeLaTabla){
	log_info(LOGGERFS,"Haciendo un describe por consola de la tabla %s", nombreDeLaTabla);
	t_metadataDeLaTabla metadataDeLaTabla = describe(nombreDeLaTabla);

	if((metadataDeLaTabla.particiones!=-1)&&
			(metadataDeLaTabla.tiempoDeCompactacion!=-1)&&
			(metadataDeLaTabla.consistencia!=NULL)){
		log_info(LOGGERFS,"Info de la tabla recuperada");
		printf("Info de la tabla: %s\n",nombreDeLaTabla);
		printf("Numero de particiones: %d\n",metadataDeLaTabla.particiones);
		printf("Tipo de consistencia: %s\n",metadataDeLaTabla.consistencia);
		printf("Tiempo de compactacion: %d\n",metadataDeLaTabla.tiempoDeCompactacion);
		}
	free(metadataDeLaTabla.consistencia);
	return EXIT_SUCCESS;
}

int consolaDrop(char* nombreDeLaTabla){
	log_info(LOGGERFS,"Haciendo un drop por consola");
	if(drop(nombreDeLaTabla)==TABLA_BORRADA){
		log_info(LOGGERFS,"Tabla %s borrada exitosamente", nombreDeLaTabla);
		return EXIT_SUCCESS;
	}else{
		log_error(LOGGERFS,"No se pudo borrar la tabla %s", nombreDeLaTabla);
		return EXIT_FAILURE;
		}
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
	printf("8) \"config\", muestra por pantalla la configuracion actual de todo el sistema\n");
	printf("9) \"reloadconfig\", recarga la configuracion del los archivos al sistema\n");
	printf("10) \"bitmap\", imprime el estado de cada bloque del FS\n");
	printf("11) \"existeLaTabla\", te dice si la tabla existe o no\n");
	printf("12) \"pmemtable\", imprime los datos de la memtable en pantalla\n");
	printf("13) \"dumpear\" [NOMBRE DE LA TABLA], fuerza el dumpeo de la tabla\n");
	return EXIT_SUCCESS;
}

int imprimirConfiguracionDelSistema(){
	printf("Configuracion del sistema:\n");
	printf("Puerto de escucha: %d\n",configuracionDelFS.puertoEscucha);
	printf("Punto de montaje: \"%s\"\n",configuracionDelFS.puntoDeMontaje);
	printf("Retardo: %d\n",obtenerRetardo());
	printf("Tamaño maximo: %d\n",configuracionDelFS.sizeValue);
	printf("Tiempo dump: %d\n",obtenerTiempoDump());
	printf("Tamaño de los bloques: %d\n",metadataDelFS.blockSize);
	printf("Cantidad de bloques: %d\n",metadataDelFS.blocks);
	printf("Magic number: %s\n",metadataDelFS.magicNumber);
	printf("Archivo bitmap: %s\n",archivoDeBitmap);
	printf("Directorio con la metadata: %s\n",directorioConLaMetadata);
	printf("Archivo con la metadata: %s\n",archivoDeLaMetadata);
	return EXIT_SUCCESS;
	}

int reloadConfig(){
	//Actualiza los datos lissandra con las modificaciones que se le hayan hecho a los achivos de configuracion
	/* Solamente se pueden actualizar los valores:
	 * retardo
	 * tiempo_dump
	 * en tiempo de ejecucion*/
	char* pathCompleto;
	pathCompleto=string_new();
	string_append(&pathCompleto, configuracionDelFS.puntoDeMontaje);
	string_append(&pathCompleto, "/Configuracion");
	string_append(&pathCompleto, "/configuracionFS.cfg");

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
	log_info(LOGGERFS,"Configuracion del FS recuperada exitosamente");

	return EXIT_SUCCESS;
}

int imprimirMemtableEnPantalla(){

	void imprimirTabla(void* nodoDeLaMemtable){
		void imprimirValores(void* nodoDeUnaTabla){
			printf("Key: %d / Timestamp: %d / Value: %s\n",
					((tp_nodoDeLaTabla)nodoDeUnaTabla)->key,
					((tp_nodoDeLaTabla)nodoDeUnaTabla)->timeStamp,
					((tp_nodoDeLaTabla)nodoDeUnaTabla)->value);
			}
		if(!list_is_empty(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla)){
			printf("Nombre de la tabla: %s\n",
				((tp_nodoDeLaMemTable)nodoDeLaMemtable)->nombreDeLaTabla);
			list_iterate(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla,
				imprimirValores);
			}
	}
	pthread_mutex_lock(&mutexDeLaMemtable);
	if(!list_is_empty(memTable)){
		list_iterate(memTable,imprimirTabla);
		}
	pthread_mutex_unlock(&mutexDeLaMemtable);
	return EXIT_SUCCESS;
}
