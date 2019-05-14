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

void recibir_select(int, int);
void recibir_insert(int, int);

int main(int argc, char ** argv) {
	puts("Comienza el test de recibimientos de info serializada");
	int escucha_fd=escucharEn(10101);
	int sender_fd=aceptarConexion(escucha_fd);
	t_cabecera cabecera=recibirCabecera(sender_fd);
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
	close(sender_fd);
	close(escucha_fd);

	return EXIT_SUCCESS;
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
