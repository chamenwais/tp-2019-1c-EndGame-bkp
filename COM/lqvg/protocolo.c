#include "protocolo.h"

void prot_enviar_int(int i,int sock){
	enviar(sock,&i,sizeof(i));
}

int prot_recibir_int(int sock){
	int i;
	recibir(sock,&i,sizeof(i));
	return i;
}

void prot_enviar_select(char *nom_tabla, uint16_t key, int socket){
	t_paquete* paquete = crear_paquete(SELECT);
	agregar_string_a_paquete(paquete, nom_tabla, strlen(nom_tabla)+1);
	agregar_uint16_t_a_paquete(paquete, key);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_select prot_recibir_select(int tamanio_paq, int socket){
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_nom_tabla, key;
	int desplazamiento = 0;
	memcpy(&tamanio_nom_tabla, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* nom_tabla=malloc(tamanio_nom_tabla);
	memcpy(nom_tabla, buffer+desplazamiento, tamanio_nom_tabla);
	desplazamiento+=tamanio_nom_tabla;
	memcpy(&key, buffer+desplazamiento, sizeof(int));
	tp_select param_select=malloc(sizeof(t_select));
	param_select->nom_tabla=nom_tabla;
	param_select->key=key;
	free(buffer);
	return param_select;
}
