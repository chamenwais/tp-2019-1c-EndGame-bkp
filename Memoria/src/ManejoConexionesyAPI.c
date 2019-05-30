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
	//Ver si hay memory leaks
}

void atender_select(int cliente, int tamanio){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un select");
	tp_select seleccion = prot_recibir_select(tamanio, cliente);
	realizar_select(seleccion->nom_tabla, seleccion->key);

	tp_select_rta rta_select = verificar_existencia_en_MP(seleccion->nom_tabla, seleccion->key);
	prot_enviar_respuesta_select(rta_select->value, rta_select->key, rta_select->timestamp, cliente);
	//Ver si hay memory leaks
}

void atender_insert(int cliente, int tamanio){
	logger(escribir_loguear, l_info, "El kernel solicito realizar un insert");
	tp_insert insercion = prot_recibir_insert(tamanio, cliente);
	realizar_insert(insercion->nom_tabla, insercion->timestamp, insercion->key, insercion->value);

	prot_enviar_respuesta_insert(cliente);
	//Ver si hay memory leaks
}

void atender_drop(int cliente, int tamanio){

}

void atender_describe(int cliente, int tamanio){

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
