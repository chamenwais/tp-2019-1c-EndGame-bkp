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
		int elemento = list_get(lista,i);
		if(elemento>mayor)mayor=elemento;
	}
	return mayor;
}


void* fdDestroyer(void * elemento){
	free((int*)elemento);
}

//funcion que maneja la creacion y uso del server
void* crearServerLissandra(){
	//------------------------------@Agregar mensajes de error!!!

	fd_conocidos.lista = list_create();//@no se donde borro esta lista

	log_info(LOGGERFS,"Iniciando server de lissandra");

	int escuchador = escucharEn(configuracionDelFS.puertoEscucha);//creo puerto de escucha

	log_info(LOGGERFS,"[LissServer] Escuchando en el puerto: %d",configuracionDelFS.puertoEscucha);

	fd_set maestro;//creo filedescriptor principal
	FD_ZERO(&maestro);
	FD_SET(escuchador,&maestro);

	list_add(fd_conocidos.lista,escuchador);//agrego el fd escuchador, que es el que va a escuchar nuevas solicitudes de conexion

	struct timeval tiempo_espera;
	tiempo_espera.tv_sec=10;//@Cuanto es un buen tiempo?
	tiempo_espera.tv_usec=0;


	while(1){//loop del select

		fd_set copia_maestro;//creo copia para que select no destruya los datos del maestro
		memcpy(&copia_maestro,&maestro,sizeof(copia_maestro));

		select(list_mayor_int(fd_conocidos.lista)+1,&copia_maestro,NULL,NULL,&tiempo_espera);


		if(FD_ISSET(escuchador,&copia_maestro)){//aceptar nueva conexion

			int cliente_nuevo = aceptarConexion(escuchador);//@checkear si hubo error

			log_info(LOGGERFS,"[LissServer] Recibida request de conexion desde %d",cliente_nuevo);

			if(recibirHandshake(LISSANDRA,MEMORIA,cliente_nuevo)==0){
				log_error(LOGGERFS,"[LissServer] No se pudo hacer handshake con %d",cliente_nuevo);
			}
			else{//handshake, @de momento lo pongo aca
				log_info(LOGGERFS,"[LissServer] Handshake con %d realizado, enviando VALUE",cliente_nuevo);

				prot_enviar_int(configuracionDelFS.sizeValue,cliente_nuevo);

				list_add(fd_conocidos.lista,cliente_nuevo);
				FD_SET(cliente_nuevo,&maestro);//agrego el nuevo cliente a los fd que conoce el maestro
			}
		}
		else{//recibir mensaje de un cliente

			for(int i =1; i < list_size(fd_conocidos.lista);i++){//arranca de 1 xq en 0 siempre esta el escuchador

				int cliente = list_get(fd_conocidos.lista,i);



				if(FD_ISSET(cliente,&copia_maestro)){
					char msg[50]="";
					int bytes_recibidos = recv(cliente,msg,sizeof(msg),NULL);

					log_info(LOGGERFS,"[LissServer] Recibi algo desde %d",cliente);

					if(bytes_recibidos<=0){//remover este cliente
						log_info(LOGGERFS,"[LissServer] El cliente %d se ha desconectado",cliente);
						cerrarConexion(cliente);
						FD_CLR(cliente,&maestro);

						bool* _fdID(void * elem){//inner function para remover el fd que cierro
							return (*((int*)elem)) == cliente;
						}

						list_remove_and_destroy_by_condition(fd_conocidos.lista,_fdID,fdDestroyer);
					}
					else{
						char rta[10]="Hola!\n";
						send(cliente,rta,sizeof(rta),NULL);
						//procesar msg
					}

				}

			}
		}



	}

	//cerrarConexion(servidor); en algun momento
	//destruir toda la lista
	//pthread_exit(0);
}//@Agregar todo al log

