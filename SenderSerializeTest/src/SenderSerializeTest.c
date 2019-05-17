/*
 ============================================================================
 Name        : SenderSerializeTest.c
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
#include <time.h>

void mandar_select(int);
void mandar_insert(int);
void mandar_create(int);
void mandar_describe(int);
void mandar_drop(int);

#define METODO 2 //el 2 es el nuevo, usar otro numero para el metodo viejo

int main(int argc, char ** argv) {
	puts("Comienza el test de envíos de info serializada");
	char *comando;

	if (argc>1){
		comando=argv[1];
	} else {
		puts("Si no decís por parámetro qué querés enviar, no camina");
		return EXIT_FAILURE;
	}

	int receiver_fd=conectarseA("127.0.0.1",10101);
	if(string_equals_ignore_case(comando, "select")){
		puts("Vamos a mandar un select");
		mandar_select(receiver_fd);

		if(METODO == 2){//recibo la respuesta
		t_cabecera cabecera=recibirCabecera(receiver_fd);
		if(cabecera.tipoDeMensaje == REQUEST_SUCCESS){
			printf("Cabecera: tamanio= %d , tipomsg= %d\n",cabecera.tamanio,cabecera.tipoDeMensaje);

			tp_select_rta result = prot_recibir_respuesta_select(cabecera.tamanio,receiver_fd);
			printf("Resultado: valor= %s\n",result->value);
		} else printf("Error con codigo= %d\n",cabecera.tipoDeMensaje);
		}

	}
	if(string_equals_ignore_case(comando, "insert")){
		puts("Vamos a mandar un insert");
		mandar_insert(receiver_fd);

		if(METODO == 2){
			//No usar antes recibirCabecera!, prot_recibir_respuesta_insert se encarga
			enum MENSAJES respuesta = prot_recibir_respuesta_insert(receiver_fd);
			if(respuesta == REQUEST_SUCCESS){
			printf("Insert realizado correctamente: %d\n",respuesta);
			}else printf("Error con codigo: %d\n",respuesta);
		}
	}
	if(string_equals_ignore_case(comando, "create")){
		puts("Vamos a mandar un create");
		mandar_create(receiver_fd);

		if(METODO == 2){
			//No usar antes recibirCabecera!, prot_recibir_respuesta_create se encarga
			enum MENSAJES respuesta = prot_recibir_respuesta_create(receiver_fd);
			if(respuesta == REQUEST_SUCCESS){
			printf("Create realizado correctamente, cod_respuesta %d\n",respuesta);
			}else printf("Error con codigo: %d\n",respuesta);
		}
	}
	if(string_equals_ignore_case(comando, "describe")){
		puts("Vamos a mandar un describe");
		mandar_describe(receiver_fd);
	}
	if(string_equals_ignore_case(comando, "drop")){
		puts("Vamos a mandar un drop");
		mandar_drop(receiver_fd);
	}
	close(receiver_fd);
	return EXIT_SUCCESS;
}

void mandar_select(int server_fd){
	prot_enviar_select("TABLA53",78, server_fd);
}

void mandar_insert(int server_fd){
	prot_enviar_insert("TABLA64", 14, "La caída", (long)time(NULL), server_fd);
}

void mandar_create(int server_fd){
	prot_enviar_create("Tablita", "SC", 5,5000,server_fd);
}

void mandar_describe(int server_fd){
	enviarCabecera(server_fd, DESCRIBE, 12);
}

void mandar_drop(int server_fd){
	enviarCabecera(server_fd, DROP, 1);
}
