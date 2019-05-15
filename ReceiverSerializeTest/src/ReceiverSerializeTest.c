/*
 ============================================================================
 Name        : ReceiverSerializeTest.c
 Author      : Tamara-Francisco
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <lqvg/com.h>
#include <lqvg/protocolo.h>
#include <pthread.h>

void recibir_select(int, int);
void recibir_insert(int, int);

void procesarSelect(int,t_cabecera);
void* procesarMensaje(void*);

#define METODO 2 //@@Metodo a usar, el 2 es el mas nuevo

int main(int argc, char ** argv) {
	puts("Comienza el test de recibimientos de info serializada");
	int escucha_fd=escucharEn(10101);
	int sender_fd=aceptarConexion(escucha_fd);
	t_cabecera cabecera=recibirCabecera(sender_fd);

	if(METODO == 1){
	switch(cabecera.tipoDeMensaje){
		case SELECT:
			printf("Va a llegar un SELECT de %d bytes\n", cabecera.tamanio);
			recibir_select(cabecera.tamanio, sender_fd);
			break;
		case INSERT:
			printf("Va a llegar un INSERT de %d bytes\n", cabecera.tamanio);
			recibir_insert(cabecera.tamanio, sender_fd);
			break;
		case CREATE:
			printf("Va a llegar un CREATE de %d bytes\n", cabecera.tamanio);
			break;
		case DESCRIBE:
			printf("Va a llegar un DESCRIBE de %d bytes\n", cabecera.tamanio);
			break;
		case DROP:
			printf("Va a llegar un DROP de %d bytes\n", cabecera.tamanio);
			break;
		default:
			break;

	}
	}
	else if(METODO == 2){
	datos_iniciales* p = (datos_iniciales*)malloc(sizeof(datos_iniciales));
	p->cabecera.tamanio=cabecera.tamanio;
	p->cabecera.tipoDeMensaje = cabecera.tipoDeMensaje;
	p->cliente = sender_fd;

	pthread_t* thread_procesar = (pthread_t*) malloc(sizeof(pthread_t));

	pthread_create( thread_procesar, NULL, procesarMensaje, (void*)p);
	pthread_join(*thread_procesar,NULL);
	free(thread_procesar);
	}


	close(sender_fd);
	close(escucha_fd);

	return EXIT_SUCCESS;
}

void* procesarMensaje(void* args){

	datos_iniciales* p = (datos_iniciales*) args;


	printf("[LissServer] Recibi cod_mensaje %d desde %d tamanio %d\n",p->cabecera.tipoDeMensaje,p->cliente,p->cabecera.tamanio);
	switch(p->cabecera.tipoDeMensaje){

	case SELECT:

		procesarSelect(p->cliente,p->cabecera);

		break;

	case INSERT:



		break;
	case CREATE:



		break;
	case DESCRIBE:



		break;
	case DROP:



		break;
	default:
		break;
	}
	free(p);
	pthread_exit(0);
}

void procesarSelect(int cliente, t_cabecera cabecera){

	tp_select seleccion = prot_recibir_select(cabecera.tamanio, cliente);
	printf("ProcesarSelect: nombre tabla = %s , value = %hu\n",seleccion->nom_tabla, seleccion->key);
	char* value = (char*)malloc(7);
	strncpy(value,"hellou",7);

	//pido el value al fs

	if (value == NULL){
		prot_enviar_error(TABLA_NO_EXISTIA,cliente);
		printf("[LissServer] Enviada respuesta de select a %d con error= TABLA_NO_EXISTIA\n",cliente);

	} else{
		prot_enviar_respuesta_select(value,cliente);
		printf("[LissServer] Enviada respuesta de select a %d con value= %s\n",cliente,value);
	}
	free(seleccion->nom_tabla);
	free(seleccion);
	free(value);

}

void recibir_select(int tamanio, int cliente_fd){
	tp_select parametros_select=prot_recibir_select(tamanio, cliente_fd);
	printf("Tabla %s\n", parametros_select->nom_tabla);
	printf("Key %hu\n", parametros_select->key);
	free(parametros_select->nom_tabla);
	free(parametros_select);
}

void recibir_insert(int tamanio, int cliente_fd){
	tp_insert parametros_insert=prot_recibir_insert(tamanio, cliente_fd);
	printf("Tabla %s\n", parametros_insert->nom_tabla);
	printf("Key %hu\n", parametros_insert->key);
	printf("Value %s\n", parametros_insert->value);
	printf("Timestamp %ld\n", parametros_insert->timestamp);
	free(parametros_insert->nom_tabla);
	free(parametros_insert->value);
	free(parametros_insert);
}
