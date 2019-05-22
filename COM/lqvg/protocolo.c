#include "protocolo.h"

void prot_enviar_int(int i,int sock){
	enviar(sock,&i,sizeof(i));
}

int prot_recibir_int(int sock){
	int i;
	recibir(sock,&i,sizeof(i));
	return i;
}

void prot_enviar_error(enum MENSAJES error, int socket){//para enviar cuando la tabla no existe,etc
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
void prot_enviar_respuesta_create(int socket){
	t_paquete* paquete = crear_paquete(REQUEST_SUCCESS);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

enum MENSAJES prot_recibir_respuesta_create(int socket){
	//posibles MENSAJES a recibir del fs: REQUEST_SUCCESS, TABLA_YA_EXISTIA
	t_cabecera cabecera = recibirCabecera(socket);
	return cabecera.tipoDeMensaje;
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

void prot_enviar_respuesta_select(char * value, int socket){
	t_paquete* paquete = crear_paquete(REQUEST_SUCCESS);
	agregar_string_a_paquete(paquete, value, strlen(value)+1);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_select_rta prot_recibir_respuesta_select(int tamanio_paq,int socket){
	//posibles MENSAJES a recibir del fs: REQUEST_SUCCESS, TABLA_NO_EXISTIA
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_value;
	int desplazamiento = 0;
	memcpy(&tamanio_value, buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* value=malloc(tamanio_value);
	memcpy(value, buffer+desplazamiento, tamanio_value);
	tp_select_rta param_select_rta =malloc(sizeof(t_select_rta));
	param_select_rta->value=value;
	free(buffer);
	return param_select_rta;
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

void prot_enviar_drop(char * nom_tabla,int socket){
	t_paquete* paquete = crear_paquete(DROP);
	agregar_string_a_paquete(paquete, nom_tabla, strlen(nom_tabla)+1);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_drop prot_recibir_drop(int tamanio_paq, int socket){
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_nom_tabla;
	int desplazamiento = 0;
	memcpy(&tamanio_nom_tabla, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* nom_tabla=malloc(tamanio_nom_tabla);
	memcpy(nom_tabla, buffer+desplazamiento, tamanio_nom_tabla);
	tp_drop param_drop=malloc(sizeof(t_drop));
	param_drop->nom_tabla=nom_tabla;
	free(buffer);
	return param_drop;
}

void prot_enviar_respuesta_drop(int socket){
	t_paquete* paquete = crear_paquete(REQUEST_SUCCESS);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

enum MENSAJES prot_recibir_respuesta_drop(int socket){
	//posibles MENSAJES a recibir del fs: REQUEST_SUCCESS, TABLA_NO_EXISTIA
	t_cabecera cabecera = recibirCabecera(socket);
	return cabecera.tipoDeMensaje;
}

void prot_enviar_describe(char *nom_tabla,int socket){//para una tabla
	t_paquete* paquete = crear_paquete(DESCRIBE);
	agregar_string_a_paquete(paquete, nom_tabla, strlen(nom_tabla)+1);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_describe prot_recibir_describe(int tamanio_paq, int socket){
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_nom_tabla;
	int desplazamiento = 0;
	memcpy(&tamanio_nom_tabla, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* nom_tabla=malloc(tamanio_nom_tabla);
	memcpy(nom_tabla, buffer+desplazamiento, tamanio_nom_tabla);
	tp_describe param_describe=malloc(sizeof(t_describe));
	param_describe->nom_tabla=nom_tabla;
	free(buffer);
	return param_describe;
}

void prot_enviar_respuesta_describe(char* nom_tabla,int particiones, char* consistencia, int tiempo_compactacion, int socket){
	t_paquete* paquete = crear_paquete(REQUEST_SUCCESS);
	agregar_string_a_paquete(paquete,nom_tabla,strlen(nom_tabla)+1);
	agregar_int_a_paquete(paquete,particiones);
	agregar_string_a_paquete(paquete, consistencia, strlen(consistencia)+1);
	agregar_int_a_paquete(paquete,tiempo_compactacion);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_describe_rta prot_recibir_respuesta_describe(int tamanio_paq,int socket){
	//posibles MENSAJES a recibir del fs: REQUEST_SUCCESS, TABLA_NO_EXISTIA
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int tamanio_nom_tabla;
	int particiones;
	int tamanio_consistencia;
	int tiempo_compactacion;
	int desplazamiento = 0;
	memcpy(&tamanio_nom_tabla, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* nom_tabla = malloc(tamanio_nom_tabla);
	memcpy(nom_tabla, buffer+desplazamiento, tamanio_nom_tabla);
	desplazamiento+=tamanio_nom_tabla;
	memcpy(&particiones, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(&tamanio_consistencia, buffer+desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	char* consistencia=malloc(tamanio_consistencia);
	memcpy(consistencia, buffer+desplazamiento, tamanio_consistencia);
	desplazamiento+=tamanio_consistencia;
	memcpy(&tiempo_compactacion, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);
	tp_describe_rta param_describe_rta =malloc(sizeof(t_describe_rta));
	param_describe_rta->nombre=nom_tabla;
	param_describe_rta->particiones=particiones;
	param_describe_rta->consistencia=consistencia;
	param_describe_rta->tiempoDeCompactacion=tiempo_compactacion;
	free(buffer);
	return param_describe_rta;
}

void prot_enviar_describeAll(int socket){
	t_paquete* paquete = crear_paquete(DESCRIBE);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

void prot_enviar_respuesta_describeAll(t_describeAll_rta descripciones, int socket){
	t_paquete* paquete = crear_paquete(REQUEST_SUCCESS);
	int cantidad_descripciones = descripciones.lista->elements_count;
	agregar_int_a_paquete(paquete,cantidad_descripciones);
	for(int i=0;i<cantidad_descripciones;i++){
		tp_describe_rta descriptor = (tp_describe_rta)list_get(descripciones.lista,i);
		agregar_string_a_paquete(paquete,descriptor->nombre,strlen(descriptor->nombre)+1);
		agregar_int_a_paquete(paquete,descriptor->particiones);
		agregar_string_a_paquete(paquete, descriptor->consistencia, strlen(descriptor->consistencia)+1);
		agregar_int_a_paquete(paquete,descriptor->tiempoDeCompactacion);
	}
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

tp_describeAll_rta prot_recibir_respuesta_describeAll(int tamanio_paq, int socket){
	//posibles MENSAJES a recibir del fs: REQUEST_SUCCESS, TABLA_NO_EXISTIA(no hay tablas en el fs)
	void * buffer = malloc(tamanio_paq);
	recibir(socket, buffer, tamanio_paq);
	int cantidad_descripciones;
	int desplazamiento=0;
	int tamanio_nombre;
	int tamanio_consistencia;
	tp_describeAll_rta descripciones = malloc(sizeof(t_describeAll_rta));
	descripciones->lista = list_create();

	memcpy(&cantidad_descripciones, buffer + desplazamiento, sizeof(int));
	desplazamiento+=sizeof(int);

	for(int i =0;i<cantidad_descripciones;i++){//agrego cada descriptor
		tp_describe_rta descriptor = malloc(sizeof(t_describe_rta));

		memcpy(&tamanio_nombre,buffer+desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		descriptor->nombre = malloc(tamanio_nombre);
		memcpy(descriptor->nombre,buffer+desplazamiento,tamanio_nombre);
		desplazamiento+=sizeof(int);
		memcpy(&descriptor->particiones, buffer+desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		memcpy(&tamanio_consistencia, buffer+desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		descriptor->consistencia=malloc(tamanio_consistencia);
		memcpy(descriptor->consistencia, buffer+desplazamiento, tamanio_consistencia);
		desplazamiento+=tamanio_consistencia;
		memcpy(&descriptor->tiempoDeCompactacion, buffer+desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);

		list_add(descripciones->lista,descriptor);
	}
	free(buffer);
	return descripciones;
}
void free_tp_describeAll_rta(void* d){
	tp_describe_rta unDescriptor = d;
	free(unDescriptor->nombre);
	free(unDescriptor->consistencia);
	free(unDescriptor);
}

void prot_free_tp_describeAll_rta(tp_describeAll_rta descriptores){
	//una unica funcion para liberar todo lo referido a la lista, no usar nada mas
	list_destroy_and_destroy_elements(descriptores->lista,free_tp_describeAll_rta);
	free(descriptores);
}








