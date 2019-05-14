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
	}
	if(string_equals_ignore_case(comando, "insert")){
		puts("Vamos a mandar un insert");
		mandar_insert(receiver_fd);
	}
	if(string_equals_ignore_case(comando, "create")){
		puts("Vamos a mandar un create");
		mandar_create(receiver_fd);
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
	enviarCabecera(server_fd, CREATE, 16);
}

void mandar_describe(int server_fd){
	enviarCabecera(server_fd, DESCRIBE, 12);
}

void mandar_drop(int server_fd){
	enviarCabecera(server_fd, DROP, 1);
}
