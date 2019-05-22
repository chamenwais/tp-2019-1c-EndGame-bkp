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

tp_select_rta pedir_value_a_liss(char * nombre_tabla, uint16_t key){
	tp_select_rta pedido_value;

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

	if(rta_select->value != NULL){
		loguear_value_por_pantalla(rta_select->value);
	}else{
		logger(escribir_loguear, l_info, "No contengo el valor de la key solicitada");
		logger(escribir_loguear, l_info, "Se enviara una solicitud al FS para obtener dicho valor");

		rta_select = pedir_value_a_liss(nombre_tabla, (uint16_t)key);

		if(rta_select->value!=NULL){
			logger(escribir_loguear, l_info, "Recibi valor= %s",rta_select->value);
			//TODO ¡El timestamp tiene que venir de LFS, no se tiene que calcular!
			colocar_value_en_MP(nombre_tabla, rta_select->timestamp,(uint16_t)key,rta_select->value);
		}
	}

	free(rta_select->value);
	free(rta_select);
}
