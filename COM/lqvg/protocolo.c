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

void prot_enviar_respuesta_insert(int socket){
	t_paquete* paquete = crear_paquete(REQUEST_SUCCESS);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}
enum MENSAJES prot_recibir_respuesta_insert(int socket){
	t_cabecera cabecera = recibirCabecera(socket);
	return cabecera.tipoDeMensaje;
}

void prot_enviar_error(enum MENSAJES error, int socket){
	t_paquete* paquete = crear_paquete(error);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

void prot_enviar_create(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion,int socket){
	t_paquete* paquete = crear_paquete(CREATE);
	agregar_string_a_paquete(paquete, nombreDeLaTabla, strlen(nombreDeLaTabla)+1);
	agregar_string_a_paquete(paquete, tipoDeConsistencia, strlen(tipoDeConsistencia)+1);
	agregar_int_a_paquete(paquete, numeroDeParticiones);
	agregar_int_a_paquete(paquete, tiempoDeCompactacion);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_create prot_recibir_create(int tamanio_paq,int socket){
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_nom_tabla;
	int tamanio_tipo_consistencia;
	int numeroDeParticiones;
	int tiempoDeCompactacion;
	int desplazamiento = 0;
	memcpy(&tamanio_nom_tabla, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* nom_tabla=malloc(tamanio_nom_tabla);
	memcpy(nom_tabla, buffer+desplazamiento, tamanio_nom_tabla);
	desplazamiento+=tamanio_nom_tabla;
	memcpy(&tamanio_tipo_consistencia, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* tipo_consistencia=malloc(tamanio_tipo_consistencia);
	memcpy(tipo_consistencia, buffer+desplazamiento, tamanio_tipo_consistencia);
	desplazamiento+=tamanio_tipo_consistencia;
	memcpy(&numeroDeParticiones, buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(&tiempoDeCompactacion, buffer+desplazamiento, sizeof(int));
	tp_create param_create=malloc(sizeof(t_create));
	param_create->nom_tabla=nom_tabla;
	param_create->tipo_consistencia=tipo_consistencia;
	param_create->numero_particiones=numeroDeParticiones;
	param_create->tiempo_compactacion=tiempoDeCompactacion;
	free(buffer);
	return param_create;
}
void prot_enviar_respuesta_create(enum MENSAJES rta,int socket){
	t_paquete* paquete = crear_paquete(rta);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

enum MENSAJES prot_recibir_respuesta_create(int socket){
	t_cabecera cabecera = recibirCabecera(socket);
	//@@@@TERMINAR
	//cabecera.tipoDeMensaje
	return cabecera.tipoDeMensaje;
}

void prot_enviar_respuesta_select(char * value, int socket){
	t_paquete* paquete = crear_paquete(REQUEST_SUCCESS);
	agregar_string_a_paquete(paquete, value, strlen(value)+1);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_select_rta prot_recibir_respuesta_select(int tamanio_paq,int socket){
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_value;
	int desplazamiento = 0;
	memcpy(&tamanio_value, buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* value=malloc(tamanio_value);
	memcpy(value, buffer+desplazamiento, tamanio_value);
	tp_select_rta param_select_rta =malloc(sizeof(tp_select_rta));
	param_select_rta->value=value;
	free(buffer);
	return param_select_rta;
}

tp_select prot_recibir_select(int tamanio_paq, int socket){
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_nom_tabla;
	uint16_t key;
	int desplazamiento = 0;
	memcpy(&tamanio_nom_tabla, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* nom_tabla=malloc(tamanio_nom_tabla);
	memcpy(nom_tabla, buffer+desplazamiento, tamanio_nom_tabla);
	desplazamiento+=tamanio_nom_tabla;
	memcpy(&key, buffer+desplazamiento, sizeof(uint16_t));
	tp_select param_select=malloc(sizeof(t_select));
	param_select->nom_tabla=nom_tabla;
	param_select->key=key;
	free(buffer);
	return param_select;
}
//@@LA CABECERA ES ..._RTA PERO DSPS RETORNO EL COD_RTA
void prot_enviar_insert(char *nom_tabla, uint16_t key, char * value, long timestamp, int socket){
	t_paquete* paquete = crear_paquete(INSERT);
	agregar_string_a_paquete(paquete, nom_tabla, strlen(nom_tabla)+1);
	agregar_uint16_t_a_paquete(paquete, key);
	agregar_string_a_paquete(paquete, value, strlen(value)+1);
	agregar_long_a_paquete(paquete, timestamp);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_insert prot_recibir_insert(int tamanio_paq, int socket){
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_nom_tabla, tamanio_value;
	uint16_t key;
	long timestamp;
	int desplazamiento = 0;
	memcpy(&tamanio_nom_tabla, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* nom_tabla=malloc(tamanio_nom_tabla);
	memcpy(nom_tabla, buffer+desplazamiento, tamanio_nom_tabla);
	desplazamiento+=tamanio_nom_tabla;
	memcpy(&key, buffer+desplazamiento, sizeof(uint16_t));
	desplazamiento+= sizeof(uint16_t);
	memcpy(&tamanio_value, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* value=malloc(tamanio_value);
	memcpy(value, buffer+desplazamiento, tamanio_value);
	desplazamiento+=tamanio_value;
	memcpy(&timestamp, buffer+desplazamiento, sizeof(long));
	tp_insert param_insert=malloc(sizeof(t_insert));
	param_insert->nom_tabla=nom_tabla;
	param_insert->key=key;
	param_insert->value=value;
	param_insert->timestamp=timestamp;
	free(buffer);

	return param_insert;
}
