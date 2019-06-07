/*
 * ManejoConexionesyAPI.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */


#include "ManejoConexionesyAPI.h"

void atender_create(int cliente, int tamanio){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un create");
	tp_create creacion = prot_recibir_create(tamanio, cliente);
	enum MENSAJES respuesta_create = realizar_create(creacion->nom_tabla, creacion->tipo_consistencia,
			creacion->numero_particiones, creacion->tiempo_compactacion);

	retornar_respuesta_al_kernel(respuesta_create, prot_enviar_respuesta_create, cliente);

	free(creacion->nom_tabla);
	free(creacion->tipo_consistencia);
	free(creacion);
}

void atender_select(int cliente, int tamanio){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un select");
	tp_select seleccion = prot_recibir_select(tamanio, cliente);
	tp_select_rta_a_kernel rta_select = realizar_select(seleccion->nom_tabla, seleccion->key);

	if(rta_select->respuesta==REQUEST_SUCCESS){
		prot_enviar_respuesta_select(rta_select->value, rta_select->key, rta_select->timestamp, cliente);
		free(rta_select->value);
		free(rta_select);
	} else {
		prot_enviar_error(rta_select->respuesta,cliente);
		free(rta_select);
	}

	free(seleccion->nom_tabla);
	free(seleccion);
}

void atender_insert(int cliente, int tamanio){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un insert");
	tp_insert insercion = prot_recibir_insert(tamanio, cliente);
	realizar_insert(insercion->nom_tabla, insercion->timestamp, insercion->key, insercion->value);

	prot_enviar_respuesta_insert(cliente);

	free(insercion->nom_tabla);
	free(insercion->value);
	free(insercion);

}

void atender_drop(int cliente, int tamanio){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un drop");
	tp_drop dropeo = prot_recibir_drop(tamanio, cliente);
	enum MENSAJES respuesta_drop=realizar_drop(dropeo->nom_tabla);

	retornar_respuesta_al_kernel(respuesta_drop, prot_enviar_respuesta_drop, cliente);

	free(dropeo->nom_tabla);
	free(dropeo);
}

void atender_describe(int cliente, int tamanio){
	tp_describe descripcion = prot_recibir_describe(cliente, tamanio);

	if(descripcion->nom_tabla!=NULL){
		atender_describe_tabla_particular(descripcion, cliente);
		free(descripcion->nom_tabla);
	}else{
		atender_describe_de_todas_las_tablas(descripcion, cliente);
	}

	free(descripcion);
}

void atender_describe_de_todas_las_tablas(tp_describe paquete_describe, int cliente){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un describe de todas las tablas que tiene liss");

	usleep(RETARDO_ACCESO_FILESYSTEM*1000);
	prot_enviar_describeAll(SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Existen tablas en el FS");
		tp_describeAll_rta info_de_las_tablas = prot_recibir_respuesta_describeAll(rta_pedido.tamanio, SOCKET_LISS);

		prot_enviar_respuesta_describeAll(*info_de_las_tablas, cliente);
		logger(escribir_loguear, l_info, "Se ha enviado la sgte informacion al kernel:");
		list_iterate(info_de_las_tablas->lista, imprimir_informacion_tabla_particular);

		//Libero la lista
		prot_free_tp_describeAll_rta(info_de_las_tablas);
	}

	if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
		logger(escribir_loguear, l_error, "No hay tablas en el FS");
		prot_enviar_error(rta_pedido.tipoDeMensaje,cliente);
	}

}

void atender_describe_tabla_particular(tp_describe paquete_describe, int cliente){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un describe de una tabla en particular");
	tp_describe_particular_rta_a_kernel rta_describe_particular = realizar_describe_para_tabla_particular(paquete_describe->nom_tabla);

	if(rta_describe_particular->respuesta==REQUEST_SUCCESS){
		prot_enviar_respuesta_describe(rta_describe_particular->nombre, rta_describe_particular->particiones, rta_describe_particular->consistencia,
				rta_describe_particular->tiempoDeCompactacion, cliente);
		free(rta_describe_particular->nombre);
		free(rta_describe_particular->consistencia);
		free(rta_describe_particular);
	} else {
		prot_enviar_error(rta_describe_particular->respuesta,cliente);
		free(rta_describe_particular);
	}
}

void retornar_respuesta_al_kernel(enum MENSAJES respuesta, void(*enviador_respuesta_ok)(int), int socket_kernel){
	if(respuesta==REQUEST_SUCCESS){
		enviador_respuesta_ok(socket_kernel);
	} else {
		prot_enviar_error(respuesta,socket_kernel);
	}
}

enum MENSAJES realizar_drop(char * nombre_tabla){
	t_entrada_tabla_segmentos * segmento = buscar_segmento_de_tabla(nombre_tabla);

	if(segmento!=NULL){
		usleep(RETARDO_ACCESO_MEMORIA*1000);
		liberar_segmento_de_MP(segmento);
	}else{
		logger(escribir_loguear, l_info, "No existe un segmento correspondiente a '%s' en memoria principal", nombre_tabla);
	}

	usleep(RETARDO_ACCESO_FILESYSTEM*1000);
	prot_enviar_drop(nombre_tabla, SOCKET_LISS);
	logger(escribir_loguear, l_info, "Se envio a liss la solicitud para borrar la tabla: '%s':", nombre_tabla);

	enum MENSAJES respuesta = prot_recibir_respuesta_drop(SOCKET_LISS);

	switch(respuesta){
		case REQUEST_SUCCESS: logger(escribir_loguear, l_info, "La tabla fue borrada exitosamente de liss");
			break;
		case TABLA_NO_EXISTIA: logger(escribir_loguear, l_info, "Liss dice que no exite la tabla que queres borrar");
			break;
		default:
			break;
	}
	return respuesta;
}

void realizar_describe_de_todas_las_tablas(){
	logger(escribir_loguear, l_info, "Se solicito hacer un describe de todas las tablas que tiene liss");

	usleep(RETARDO_ACCESO_FILESYSTEM*1000);
	prot_enviar_describeAll(SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Existen tablas en el FS");
		tp_describeAll_rta info_de_las_tablas = prot_recibir_respuesta_describeAll(rta_pedido.tamanio, SOCKET_LISS);
		logger(escribir_loguear, l_info, "Liss ha enviado la sgte informacion:");
		list_iterate(info_de_las_tablas->lista, imprimir_informacion_tabla_particular);

		//Libero la lista
		prot_free_tp_describeAll_rta(info_de_las_tablas);
	}

	if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
		logger(escribir_loguear, l_error, "No hay tablas en el FS");
	}

}

void imprimir_informacion_tabla_particular(void * info_tabla){
	logger(escribir_loguear, l_info, "El nombre de la tabla es: %s", (*(t_describe_rta*)info_tabla).nombre);
	logger(escribir_loguear, l_info, "La consistencia es: %s", (*(t_describe_rta*)info_tabla).consistencia);
	logger(escribir_loguear, l_info, "El numero de particiones es: %d", (*(t_describe_rta*)info_tabla).particiones);
	logger(escribir_loguear, l_info, "El tiempo de compactacion es: %d\n", (*(t_describe_rta*)info_tabla).tiempoDeCompactacion);
}

void convertir_respuesta_describe_particular(tp_describe_particular_rta_a_kernel respuesta_a_kernel,
		tp_describe_rta info_tabla, enum MENSAJES mensaje_respuesta) {
	respuesta_a_kernel->consistencia= info_tabla->consistencia;
	respuesta_a_kernel->nombre= info_tabla->nombre;
	respuesta_a_kernel->particiones = info_tabla->particiones;
	respuesta_a_kernel->tiempoDeCompactacion = info_tabla->tiempoDeCompactacion;
	respuesta_a_kernel->respuesta = mensaje_respuesta;
}

tp_describe_particular_rta_a_kernel realizar_describe_para_tabla_particular(char * nom_tabla){
	tp_describe_rta info_tabla;
	tp_describe_particular_rta_a_kernel rta_describe_particular_a_kernel = malloc(sizeof(t_describe_particular_rta_a_kernel));

	logger(escribir_loguear, l_info, "Se solicito hacer un describe de la tabla '%s'", nom_tabla);
	logger(escribir_loguear, l_info, "Le voy a pedir a liss la informacion de: '%s'", nom_tabla);

	usleep(RETARDO_ACCESO_FILESYSTEM*1000);
	prot_enviar_describe(nom_tabla, SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Informacion recibida correctamente");
		info_tabla = prot_recibir_respuesta_describe(rta_pedido.tamanio, SOCKET_LISS);
		convertir_respuesta_describe_particular(rta_describe_particular_a_kernel, info_tabla, rta_pedido.tipoDeMensaje);

		logger(escribir_loguear, l_info, "Liss ha enviado la sgte informacion:");
		imprimir_informacion_tabla_particular(info_tabla);

		free(info_tabla->consistencia);
		free(info_tabla->nombre);
		free(info_tabla);

	}

	if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
		rta_describe_particular_a_kernel->consistencia=NULL;
		rta_describe_particular_a_kernel->nombre=NULL;
		rta_describe_particular_a_kernel->particiones=0;
		rta_describe_particular_a_kernel->tiempoDeCompactacion=0;
		rta_describe_particular_a_kernel->respuesta=rta_pedido.tipoDeMensaje;
		logger(escribir_loguear, l_error, "Hubo un problema con el FS, parece que no existe la tabla");
	}

	return rta_describe_particular_a_kernel;

}

void loguear_value_por_pantalla(char * value){
	logger(escribir_loguear, l_info, "El value de la key solicitada es '%s'", value);
}

enum MENSAJES realizar_create(char * nombre_tabla, char * tipo_consistencia, int numero_particiones, int tiempo_compactacion){
	usleep(RETARDO_ACCESO_FILESYSTEM*1000);
	prot_enviar_create(nombre_tabla, tipo_consistencia, numero_particiones, tiempo_compactacion, SOCKET_LISS);
	logger(escribir_loguear, l_info, "Se envio a liss la solicitud para crear una tabla...");

	enum MENSAJES respuesta = prot_recibir_respuesta_create(SOCKET_LISS);

	switch(respuesta){
		case REQUEST_SUCCESS: logger(escribir_loguear, l_info, "La tabla fue creada correctamente.");
			break;
		case TABLA_YA_EXISTIA: logger(escribir_loguear, l_info, "La tabla ya existe!");
			break;
		default:
			break;
	}
	return respuesta;
}

tp_select_rta_a_kernel pedir_value_a_liss(char * nombre_tabla, uint16_t key){
	tp_select_rta_a_kernel rta_select_a_kernel = malloc(sizeof(t_select_rta_a_kernel));

	usleep(RETARDO_ACCESO_FILESYSTEM*1000);
	prot_enviar_select(nombre_tabla, key, SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Value recibido correctamente");
		tp_select_rta pedido_value = prot_recibir_respuesta_select(rta_pedido.tamanio, SOCKET_LISS);
		convertir_respuesta_select(rta_select_a_kernel, pedido_value,
				rta_pedido.tipoDeMensaje);
		//free(pedido_value);
	} else {
		rta_select_a_kernel->value=NULL;
		rta_select_a_kernel->respuesta=rta_pedido.tipoDeMensaje;
		if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
			logger(escribir_loguear, l_error, "Hubo un problema con el FS, parece que no existe la tabla");
		}
		//TODO si agregan KEY_NO_EXISTE se podría agregar un mensaje indicándolo
	}

	return rta_select_a_kernel;
}

void convertir_respuesta_select(tp_select_rta_a_kernel respuesta_a_kernel,
		tp_select_rta respuesta_memoria, enum MENSAJES mensaje_respuesta) {
	respuesta_a_kernel->key = respuesta_memoria->key;
	respuesta_a_kernel->timestamp = respuesta_memoria->timestamp;
	respuesta_a_kernel->value = respuesta_memoria->value;
	respuesta_a_kernel->respuesta = mensaje_respuesta;
	free(respuesta_memoria);
}

tp_select_rta_a_kernel realizar_select(char * nombre_tabla, int key){
	tp_select_rta rta_select_MP = verificar_existencia_en_MP(nombre_tabla, key);
	tp_select_rta_a_kernel rta_select_a_kernel;

	if(rta_select_MP!=NULL && rta_select_MP->value != NULL){
		loguear_value_por_pantalla(rta_select_MP->value);
		rta_select_a_kernel = malloc(sizeof(t_select_rta_a_kernel));
		convertir_respuesta_select(rta_select_a_kernel, rta_select_MP,
				REQUEST_SUCCESS);
	}else{
		logger(escribir_loguear, l_info, "No contengo el valor de la key solicitada");
		logger(escribir_loguear, l_info, "Se enviara una solicitud al FS para obtener dicho valor");

		rta_select_a_kernel = pedir_value_a_liss(nombre_tabla, (uint16_t)key);

		if(rta_select_a_kernel->value!=NULL){
			logger(escribir_loguear, l_info, "Recibi el valor '%s'",rta_select_a_kernel->value);
			colocar_value_en_MP(nombre_tabla, rta_select_a_kernel->timestamp,(uint16_t)key,rta_select_a_kernel->value);
		}
	}

	return rta_select_a_kernel;
}

void realizar_insert(char * nombre_tabla, long timestamp, uint16_t key, char * value){
	tp_select_rta rta_select = verificar_existencia_en_MP(nombre_tabla, key);

	if(rta_select != NULL && rta_select->value != NULL){
		actualizar_value_modificado_en_MP(nombre_tabla, timestamp, key, value);
	} else {
		insertar_value_modificado_en_MP(nombre_tabla, timestamp, key, value);
	}
	logger(escribir_loguear, l_info, "Se insertó el value '%s' para la key %d en memoria",value, key);
}
