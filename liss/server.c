/*
 * server.c
 *
 *  Created on: 13 abr. 2019
 *      Author: utnso
 */
#include "server.h"


struct fds{
	t_list *lista;
}fd_conocidos;



//funcion para crear el hilo de server lissandra
int lanzarServer(){//@@crear una funcion que lance hilos de estos especiales seria mas lindo

	log_info(LOGGERFS,"Iniciando hilo de server lissandra");
	int resultadoDeCrearHilo = pthread_create( &threadServer, NULL, crearServerLissandra, (void*)NULL);
	pthread_detach(threadServer);
	if(resultadoDeCrearHilo){
		log_error(LOGGERFS,"Error al crear hilo de server lissandra, return code: %d",resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
	}else{
		log_info(LOGGERFS,"Hilo de server lissandra creado exitosamente");
		return EXIT_SUCCESS;
	}

}

int list_mayor_int(t_list *lista){
	int size=list_size(lista),mayor=-1;
	for (int i=0;i<size;i++){
		int *elemento = (int*)list_get(lista,i);
		if(*elemento>mayor)mayor=*elemento;
	}
	return mayor;
}


//funcion que maneja la creacion y uso del server
void* crearServerLissandra(){

	fd_conocidos.lista = list_create();//lista de file descriptors
	log_info(LOGGERFS,"Iniciando server de lissandra");

	int *escuchador = malloc(sizeof(int));//INFO: tengo que alocar espacio en memoria xq list_add no se crea el suyo propio
	*escuchador = escucharEn(configuracionDelFS.puertoEscucha);//creo puerto de escucha
	log_info(LOGGERFS,"[LissServer] Escuchando en el puerto: %d",configuracionDelFS.puertoEscucha);

	fd_set maestro;//creo filedescriptor principal
	FD_ZERO(&maestro);
	FD_SET(*escuchador,&maestro);

	list_add(fd_conocidos.lista,(void*)escuchador);//agrego el fd escuchador, que es el que va a escuchar nuevas solicitudes de conexion

	struct timeval tiempo_espera;
	tiempo_espera.tv_sec=10;//@Cuanto es un buen tiempo?
	tiempo_espera.tv_usec=0;

	fd_set copia_maestro;
	struct timeval copia_tiempo_espera;
	int select_result=0;

	while(1){//loop del select

		copia_maestro=maestro;//creo copia para que select no destruya los datos del maestro
		copia_tiempo_espera=tiempo_espera;
		//memcpy(&copia_maestro,&maestro,sizeof(copia_maestro));

		select_result = select(list_mayor_int(fd_conocidos.lista)+1,&copia_maestro,NULL,NULL,&copia_tiempo_espera);


		if(select_result == -1){
			log_error(LOGGERFS,"[LissServer] Error en el select");
		}
		else if (select_result == 0){
			//log_info(LOGGERFS,"[LissServer] Timeout del select");
			//@timeout
		}
		else if(FD_ISSET(*escuchador,&copia_maestro)){//aceptar nueva conexion

			int *cliente_nuevo = malloc(sizeof(int));
			*cliente_nuevo = aceptarConexion(*escuchador);//@checkear si hubo error, chequeo si ya hay conexion ahi?

			log_info(LOGGERFS,"[LissServer] Recibida request de conexion desde %d",*cliente_nuevo);

			if(recibirHandshake(LISSANDRA,MEMORIA,*cliente_nuevo)==0){
				//handshake incorrecto
				free(cliente_nuevo);
				log_error(LOGGERFS,"[LissServer] No se pudo hacer handshake con %d",*cliente_nuevo);
			}
			else{//handshake, @de momento lo pongo aca
				log_info(LOGGERFS,"[LissServer] Handshake con %d realizado, enviando VALUE",*cliente_nuevo);

				prot_enviar_int(configuracionDelFS.sizeValue,*cliente_nuevo);

				list_add(fd_conocidos.lista,(void*)cliente_nuevo);
				FD_SET(*cliente_nuevo,&maestro);//agrego el nuevo cliente a los fd que conoce el maestro
			}
		}

		else{//recibir mensaje de un cliente

			for(int i =1; i < list_size(fd_conocidos.lista);i++){//arranca de 1 xq en 0 siempre esta el escuchador

				int *cliente = (int*)list_get(fd_conocidos.lista,i);//no hacer free, es el valor en la tabla

				if(FD_ISSET(*cliente,&copia_maestro)){//mensaje del cliente

					t_cabecera cabecera=recibirCabecera(*cliente);//@necesito hacer free a cabecera?

					if(cabecera.tamanio == -1 )/*==5*/{//remover este cliente si se desconecta
						log_info(LOGGERFS,"[LissServer] El cliente %d se ha desconectado, cerrando conexion",*cliente);
						cerrarConexion(*cliente);
						FD_CLR(*cliente,&maestro);

						bool _fdID(void * elem){//inner function para remover el fd que cierro
							return (*((int*)elem)) == *cliente;
						}
						list_remove_and_destroy_by_condition(fd_conocidos.lista,_fdID,free);

					}
					else{ //procesar msg

						datos_iniciales* p = (datos_iniciales*)malloc(sizeof(datos_iniciales));
						p->cabecera.tamanio=cabecera.tamanio;
						p->cabecera.tipoDeMensaje = cabecera.tipoDeMensaje;
						p->cliente = *cliente;



						pthread_t* thread_procesar = (pthread_t*) malloc(sizeof(pthread_t));

						int resultadoDeCrearHilo = pthread_create( thread_procesar, NULL, procesarMensaje, (void*)p);
						pthread_detach(*thread_procesar);
						free(thread_procesar);
						if(resultadoDeCrearHilo)
							log_error(LOGGERFS,"Error al crear hilo de procesarMensaje, return code: %d",resultadoDeCrearHilo);


					}

				}

			}
		}



	}
	log_info(LOGGERFS,"[LissServer] Finalizando server");
	cerrarConexion(*escuchador);
	list_destroy_and_destroy_elements(fd_conocidos.lista,free);
	pthread_exit(0);
}

void* procesarMensaje(void* args){

	datos_iniciales* p = (datos_iniciales*) args;

	log_info(LOGGERFS,"[LissServer] Recibi cod_mensaje %d desde %d tamanio %d",p->cabecera.tipoDeMensaje,p->cliente,p->cabecera.tamanio);

	switch(p->cabecera.tipoDeMensaje){

	case SELECT:

		procesarSelect(p->cliente,p->cabecera);

		break;

	case INSERT:

		procesarInsert(p->cliente,p->cabecera);

		break;
	case CREATE:

		procesarCreate(p->cliente,p->cabecera);

		break;
	case DESCRIBE:
		//@@esta en 0 cuando no le paso nada??
		if(p->cabecera.tamanio!=0) procesarDescribe(p->cliente,p->cabecera);
		else procesarDescribeAll(p->cliente);

		break;
	case DROP:

		procesarDrop(p->cliente,p->cabecera);

		break;
	default:
		break;
	}
	free(p);//@@@solo libero p??
	pthread_exit(0);
}

void procesarSelect(int cliente, t_cabecera cabecera){

	tp_select seleccion = prot_recibir_select(cabecera.tamanio, cliente);

	char* value = selectf(seleccion->nom_tabla, seleccion->key);//pido el value al fs

	if(value != NULL){
		prot_enviar_respuesta_select(value,cliente);
		log_info(LOGGERFS,"[LissServer] Select: tabla= %s con value= %s",seleccion->nom_tabla,seleccion->key);
	}
	else{
		prot_enviar_error(TABLA_NO_EXISTIA,cliente);
		log_info(LOGGERFS,"[LissServer] Error Select: tabla= %d no existe",seleccion->nom_tabla);
	}
	free(seleccion->nom_tabla);
	free(seleccion);
	free(value);

}

void procesarInsert(int cliente, t_cabecera cabecera){

	tp_insert insercion = prot_recibir_insert(cabecera.tamanio, cliente);

	//@@puedo llegar a recibir insert sin timestamp?
	int result = insert(insercion->nom_tabla, insercion->key, insercion->value, insercion->timestamp);

	if (result == EXIT_SUCCESS){
		prot_enviar_respuesta_insert(cliente);
		log_info(LOGGERFS,"[LissServer] Insert: tabla= %s , value= %s",insercion->nom_tabla,insercion->value);
	} else {
		prot_enviar_error(TABLA_NO_EXISTIA,cliente);
		log_info(LOGGERFS,"[LissServer] Error Insert: tabla= %s no existe",insercion->nom_tabla,insercion->value);
	}

	free(insercion->nom_tabla);
	free(insercion->value);
	free(insercion);
}

void procesarCreate(int cliente, t_cabecera cabecera){

	tp_create creacion = prot_recibir_create(cabecera.tamanio,cliente);

	int result= create(creacion->nom_tabla, creacion->tipo_consistencia, creacion->numero_particiones,
			creacion->tiempo_compactacion);
	if(result == TABLA_CREADA){
		prot_enviar_respuesta_create(cliente);
		log_info(LOGGERFS,"[LissServer] Create: tabla= %s",creacion->nom_tabla);
	} else {
		prot_enviar_error(TABLA_YA_EXISTIA,cliente);
		log_info(LOGGERFS,"[LissServer] Error Create: tabla= %s ya existe",creacion->nom_tabla);
	}
	free(creacion->nom_tabla);
	free(creacion->tipo_consistencia);
	free(creacion);

}
void procesarDescribe(int cliente, t_cabecera cabecera){

	tp_describe descripcion = prot_recibir_describe(cabecera.tamanio,cliente);

	t_metadataDeLaTabla metadata = describe(descripcion->nom_tabla);
	if(metadata.consistencia!=NULL){
		prot_enviar_respuesta_describe(metadata.particiones,metadata.consistencia,metadata.tiempoDeCompactacion,cliente);
		log_info(LOGGERFS,"[LissServer] Describe: tabla= %s",descripcion->nom_tabla);
	} else {
		prot_enviar_error(TABLA_NO_EXISTIA,cliente);
		log_info(LOGGERFS,"[LissServer] Error Describe: tabla= %s no existe",descripcion->nom_tabla);
	}
	free(descripcion->nom_tabla);
	free(descripcion);
}

void procesarDescribeAll(int cliente){

	t_describeAll_rta descripciones;//=describeAll();

	if(descripciones.lista!=NULL){
		prot_enviar_respuesta_describeAll(descripciones,cliente);
		log_info(LOGGERFS,"[LissServer] DescribeAll");
	} else {
		prot_enviar_error(TABLA_NO_EXISTIA,cliente);
		log_info(LOGGERFS,"[LissServer] Error DescribeAll: no hay tablas en el fs");
	}

	//@@@@@@free de cada elemento y free de sus char* probablemente con otra funcion
}

void procesarDrop(int cliente, t_cabecera cabecera){

	tp_drop dropeo = prot_recibir_drop(cabecera.tamanio,cliente);

	int result = drop(dropeo->nom_tabla);

	if(result==TABLA_BORRADA){
		prot_enviar_respuesta_drop(cliente);
		log_info(LOGGERFS,"[LissServer] Drop: tabla= %s",dropeo->nom_tabla);
	} else{
		prot_enviar_error(TABLA_NO_EXISTIA,cliente);
		log_info(LOGGERFS,"[LissServer] Error Drop: tabla= %s no existe",dropeo->nom_tabla);
	}
	free(dropeo->nom_tabla);
	free(dropeo);
}

