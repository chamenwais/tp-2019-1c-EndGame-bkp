/*
 * ManejoConexionesyAPI.c
 *
 *  Created on: 20 may. 2019
 *      Author: utnso
 */


#include "ManejoConexionesyAPI.h"

void loguear_value_por_pantalla(char * value){
	logger(escribir_loguear, l_info, "El value de la key solicitada es '%s", value);
}

char * pedir_value_a_liss(char * nombre_tabla, int key){
	tp_select_rta pedido_value;
	char * value_en_cuestion = NULL;

	prot_enviar_select(nombre_tabla, key, SOCKET_LISS);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(SOCKET_LISS);

	if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
		logger(escribir_loguear, l_debug, "Value recibido correctamente");
		pedido_value = prot_recibir_respuesta_select(rta_pedido.tamanio, SOCKET_LISS);
		value_en_cuestion = pedido_value->value;
	}
	if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
		logger(escribir_loguear, l_error, "Hubo un problema con el FS, parece que no existe la tabla");
	}

	free(pedido_value->value);
	free(pedido_value);

	return value_en_cuestion;
}

void realizar_select(char * nombre_tabla, int key){
	char* value = verificar_existencia_en_MP(nombre_tabla, key);

	if(value != NULL){
		loguear_value_por_pantalla(value);
	}else{
		logger(escribir_loguear, l_info, "No contengo el valor de la key solicitada");
		logger(escribir_loguear, l_info, "Se enviara una solicitud al FS para obtener dicho valor");

		value = pedir_value_a_liss(nombre_tabla, key);

		if(value!=NULL){
			//TODO ¡El timestamp tiene que venir de LFS, no se tiene que calcular!
			colocar_value_en_MP(nombre_tabla, (unsigned)time(NULL),(uint16_t)key,value);
		}
	}

	free(value);
}
