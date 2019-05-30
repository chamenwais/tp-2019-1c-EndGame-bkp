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
	realizar_create(creacion->nom_tabla, creacion->numero_particiones, creacion->tiempo_compactacion, creacion->tipo_consistencia);

	prot_enviar_respuesta_create(cliente);

	free(creacion->nom_tabla);
	free(creacion->tipo_consistencia);
	free(creacion);
}

void atender_select(int cliente, int tamanio){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un select");
	tp_select seleccion = prot_recibir_select(tamanio, cliente);
	realizar_select(seleccion->nom_tabla, seleccion->key);

	tp_select_rta rta_select = verificar_existencia_en_MP(seleccion->nom_tabla, seleccion->key);
	prot_enviar_respuesta_select(rta_select->value, rta_select->key, rta_select->timestamp, cliente);

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

}

void atender_describe(int cliente, int tamanio){

}

void atender_describe_de_todas_las_tablas(int cliente, int tamanio){

}

void atender_describe_tabla_particular(char * nom_tabla){

}

void realizar_describe_de_todas_las_tablas(){
	logger(escribir_loguear, l_info, "Se solicito hacer un describe de todas las tablas que tiene liss");

	prot_enviar_describeAll(SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Existen tablas en el FS");

		tp_describeAll_rta info_de_las_tablas = prot_recibir_respuesta_describeAll(rta_pedido.tamanio, SOCKET_LISS);

		logger(escribir_loguear, l_info, "Liss ha enviado la sgte informacion:");

		list_iterate(info_de_las_tablas->lista, imprimir_informacion_tabla_particular);

		//Libero la lista
		//prot_free_tp_describeAll_rta(info_de_las_tablas->lista);
	}

	if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
		logger(escribir_loguear, l_error, "No hay tablas en el FS");
	}

}

void imprimir_informacion_tabla_particular(tp_describe_rta info_tabla){
	logger(escribir_loguear, l_info, "El nombre de la tabla es: %s", info_tabla->nombre);
	logger(escribir_loguear, l_info, "La consistencia es: %s", info_tabla->consistencia);
	logger(escribir_loguear, l_info, "El numero de particiones es: %d", info_tabla->particiones);
	logger(escribir_loguear, l_info, "El tiempo de compactacion es: %d\n", info_tabla->tiempoDeCompactacion);
}

void realizar_describe_para_tabla_particular(char * nom_tabla){
	tp_describe_rta info_tabla;

	logger(escribir_loguear, l_info, "Se solicito hacer un describe de la tabla '%s'", nom_tabla);
	logger(escribir_loguear, l_info, "Le voy a pedir a liss la informacion de: '%s'", nom_tabla);

	prot_enviar_describe(nom_tabla, SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Informacion recibida correctamente");
		info_tabla = prot_recibir_respuesta_describe(rta_pedido.tamanio, SOCKET_LISS);

		logger(escribir_loguear, l_info, "Liss ha enviado la sgte informacion:");
		imprimir_informacion_tabla_particular(info_tabla);

		free(info_tabla->consistencia);
		free(info_tabla->nombre);
		free(info_tabla);

	}

	if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
		logger(escribir_loguear, l_error, "Hubo un problema con el FS, parece que no existe la tabla");
	}

}

void loguear_value_por_pantalla(char * value){
	logger(escribir_loguear, l_info, "El value de la key solicitada es '%s'", value);
}

void realizar_create(char * nombre_tabla, char * tipo_consistencia, int numero_particiones, int tiempo_compactacion){
	prot_enviar_create(nombre_tabla, tipo_consistencia, numero_particiones, tiempo_compactacion, SOCKET_LISS);
	logger(escribir_loguear, l_info, "Se envio a liss la solicitud para crear una tabla...");

	int respuesta = prot_recibir_respuesta_create(SOCKET_LISS);

	switch(respuesta){
		case REQUEST_SUCCESS: logger(escribir_loguear, l_info, "La tabla fue creada correctamente.");
			break;
		case TABLA_YA_EXISTIA: logger(escribir_loguear, l_info, "La tabla ya existe!");
			break;
	}

}

tp_select_rta pedir_value_a_liss(char * nombre_tabla, uint16_t key){
	tp_select_rta pedido_value;

	usleep(RETARDO_ACCESO_FILESYSTEM*1000);
	prot_enviar_select(nombre_tabla, key, SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Value recibido correctamente");
		pedido_value = prot_recibir_respuesta_select(rta_pedido.tamanio, SOCKET_LISS);
		return pedido_value;
	}
	pedido_value = malloc(sizeof(t_select_rta));
	pedido_value->value=NULL;
	if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
		logger(escribir_loguear, l_error, "Hubo un problema con el FS, parece que no existe la tabla");
	}

	return pedido_value;
}

void realizar_select(char * nombre_tabla, int key){
	tp_select_rta rta_select = verificar_existencia_en_MP(nombre_tabla, key);

	if(rta_select!=NULL && rta_select->value != NULL){
		loguear_value_por_pantalla(rta_select->value);
	}else{
		logger(escribir_loguear, l_info, "No contengo el valor de la key solicitada");
		logger(escribir_loguear, l_info, "Se enviara una solicitud al FS para obtener dicho valor");

		rta_select = pedir_value_a_liss(nombre_tabla, (uint16_t)key);

		if(rta_select->value!=NULL){
			logger(escribir_loguear, l_info, "Recibi el valor '%s'",rta_select->value);
			colocar_value_en_MP(nombre_tabla, rta_select->timestamp,(uint16_t)key,rta_select->value);
		}
	}

	free(rta_select->value);
	free(rta_select);
}

void realizar_insert(char * nombre_tabla, long timestamp, uint16_t key, char * value){
	tp_select_rta rta_select = verificar_existencia_en_MP(nombre_tabla, key);

	if(rta_select != NULL && rta_select->value != NULL){
		actualizar_value_modificado_en_MP(nombre_tabla, timestamp, key, value);
	} else {
		insertar_value_modificado_en_MP(nombre_tabla, timestamp, key, value);
	}
	logger(escribir_loguear, l_info, "Se insertó el value '%s' en memoria",value);
}
