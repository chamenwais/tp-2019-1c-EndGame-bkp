/*
 * GestionSegPaginada.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "GestionSegPaginada.h"

int obtener_tamanio_marco() {
	int tamanio_marco = sizeof(long) + sizeof(uint16_t) + TAMANIO_VALUE
			+ 1;
	return tamanio_marco;
}

int obtener_cantidad_marcos_en_MP(int tamanio_marco) {
	int cantidad_marcos = (int) TAMANIO_MEMORIA / tamanio_marco;
	return cantidad_marcos;
}

void inicializar_bitmap_marcos(){
	int tamanio_bitmap=obtener_cantidad_marcos_en_MP(obtener_tamanio_marco());
	void * bits_bitmap = malloc(tamanio_bitmap);
	bitmap_marcos=bitarray_create_with_mode(bits_bitmap, tamanio_bitmap,  LSB_FIRST);
}

void liberar_bitmap_marcos(){
	free(bitmap_marcos->bitarray);
	bitarray_destroy(bitmap_marcos);
}

int obtener_marco_libre_del_bitmap(){
	/* Retorna un numero de marco libre en el bitmap
	 * Si no hay mas marcos libres retorna -1 */
	int i;
	for(i=0;i<obtener_cantidad_marcos_en_MP(obtener_tamanio_marco());i++){
		if(!bitarray_test_bit(bitmap_marcos,i)){
			logger(escribir_loguear, l_trace,"El marco %d esta libre", i);
			return i;
		}
	}
	logger(escribir_loguear, l_warning,"No hay mas marcos libres");
	return -1;
}

void ocupar_marco_en_el_bitmap(int marco){
	bitarray_set_bit(bitmap_marcos,marco);
}

void inicializar_tabla_segmentos(){
	tabla_de_segmentos=list_create();
}

void destructor_pagina(void * pagina){
	free(pagina);
}

void destructor_segmento(void * segmento){
	list_destroy_and_destroy_elements((*(t_entrada_tabla_segmentos *)segmento).base,destructor_pagina);
	free((*(t_entrada_tabla_segmentos *)segmento).tabla);
	free(segmento);
}

void liberar_tabla_segmentos(){
	list_destroy_and_destroy_elements(tabla_de_segmentos, destructor_segmento);
}

uint16_t* obtener_key_desde_marco_en_MP(int marco) {
	int desplazamiento = obtener_1er_byte_marco_en_MP(marco);
	desplazamiento += sizeof(long);
	uint16_t* bytes_key_en_MP = malloc(sizeof(uint16_t));
	memcpy(bytes_key_en_MP, MEMORIA_PRINCIPAL + desplazamiento, sizeof(uint16_t));
	return bytes_key_en_MP;
}

long * obtener_timestamp_desde_marco_en_MP(int marco){
	int desplazamiento = obtener_1er_byte_marco_en_MP(marco);
	long *bytes_timestamp_en_MP = malloc(sizeof(long));
	memcpy(bytes_timestamp_en_MP, MEMORIA_PRINCIPAL + desplazamiento, sizeof(long));
	return bytes_timestamp_en_MP;
}

char * obtener_value_desde_marco_en_MP(int marco){
	int desplazamiento = obtener_1er_byte_marco_en_MP(marco);
	desplazamiento += sizeof(long);
	desplazamiento += sizeof(uint16_t);
	char * bytes_value_en_MP = malloc(TAMANIO_VALUE+1);
	memcpy(bytes_value_en_MP, MEMORIA_PRINCIPAL + desplazamiento, TAMANIO_VALUE+1);
	return bytes_value_en_MP;
}

tp_select_rta verificar_existencia_en_MP(char * nombre_tabla, uint16_t key){
	t_entrada_tabla_segmentos* segmento = buscar_segmento_de_tabla(nombre_tabla);
	if (segmento == NULL) {
		return NULL;
	}
	if(list_is_empty(segmento->base)){
		return NULL;
	}

	t_entrada_tabla_paginas * pagina_con_key=buscar_pagina_de_key_en_MP(segmento->base, key);
	if(pagina_con_key==NULL){
		return NULL;
	}
	tp_select_rta valor_marco = malloc(sizeof(t_select_rta));
	long* p_timestamp = obtener_timestamp_desde_marco_en_MP(pagina_con_key->marco);
	valor_marco->timestamp = *p_timestamp;
	free(p_timestamp);
	uint16_t* p_key = obtener_key_desde_marco_en_MP(pagina_con_key->marco);
	valor_marco->key = *p_key;
	free(p_key);
	char * p_value = obtener_value_desde_marco_en_MP(pagina_con_key->marco);
	valor_marco->value=string_duplicate(p_value);
	free(p_value);
	return valor_marco;
}

t_entrada_tabla_paginas * buscar_pagina_de_key_en_MP(t_list* tabla_de_paginas,uint16_t key){
	bool tiene_la_key(void * una_pagina){
		uint16_t* bytes_key_en_MP = obtener_key_desde_marco_en_MP((*(t_entrada_tabla_paginas*) una_pagina).marco);
		if(*bytes_key_en_MP==key){
			free(bytes_key_en_MP);
			return true;
		} else {
			free(bytes_key_en_MP);
			return false;
		}
	}
	usleep(RETARDO_ACCESO_MEMORIA*1000);
	return list_find(tabla_de_paginas,tiene_la_key);
}

t_entrada_tabla_segmentos* obtener_segmento_de_tabla(char* nom_tabla) {
	t_entrada_tabla_segmentos* segmento = buscar_segmento_de_tabla(nom_tabla);
	if (segmento == NULL) {
		segmento = crear_segmento_a_tabla(nom_tabla);
	}
	return segmento;
}

int obtener_marco_libre() {
	int marco_asignado = obtener_marco_libre_del_bitmap();
	if (marco_asignado == -1) {
		marco_asignado = ejecutar_algoritmo_reemplazo_y_obtener_marco();
	}
	return marco_asignado;
}

int almacenar_valor(char* nom_tabla, long timestamp, uint16_t key, char* value,
		int flag) {
	t_entrada_tabla_segmentos* segmento = obtener_segmento_de_tabla(nom_tabla);
	int marco_asignado = obtener_marco_libre();
	if(marco_asignado<0){
		return marco_asignado;
	}
	usleep(RETARDO_ACCESO_MEMORIA*1000);
	insertar_registro_en_marco(timestamp, key, value, marco_asignado);
	crear_pagina_en_tabla_paginas(segmento, marco_asignado, flag);
	return 1;
}

int colocar_value_en_MP(char *nom_tabla, long timestamp, uint16_t key, char *value){
	return almacenar_valor(nom_tabla, timestamp, key, value, FLAG_NO_MODIFICADO);
}

int insertar_value_modificado_en_MP(char *nom_tabla, long timestamp, uint16_t key, char *value){
	return almacenar_valor(nom_tabla, timestamp, key, value, FLAG_MODIFICADO);
}

void actualizar_value_modificado_en_MP(char *nom_tabla, long timestamp, uint16_t key, char *value){
	t_entrada_tabla_segmentos* segmento = buscar_segmento_de_tabla(nom_tabla);
	t_entrada_tabla_paginas * pagina_con_key=buscar_pagina_de_key_en_MP(segmento->base, key);
	usleep(RETARDO_ACCESO_MEMORIA*1000);
	insertar_registro_en_marco(timestamp, key, value, pagina_con_key->marco);
	pagina_con_key->flag=FLAG_MODIFICADO;
}

t_entrada_tabla_segmentos * buscar_segmento_de_tabla(char * nombre_tabla){
	if(list_is_empty(tabla_de_segmentos)){
		return NULL;
	}
	bool es_segmento_de_tabla(void * un_segmento){
		return string_equals_ignore_case((*(t_entrada_tabla_segmentos *)un_segmento).tabla,nombre_tabla);
	}
	t_entrada_tabla_segmentos * segmento=(t_entrada_tabla_segmentos *) list_find(tabla_de_segmentos, es_segmento_de_tabla);
	return segmento;
}

t_entrada_tabla_segmentos * crear_segmento_a_tabla(char * nombre_tabla){
	t_entrada_tabla_segmentos * segmento=malloc(sizeof(t_entrada_tabla_segmentos));
	segmento->base=list_create();
	segmento->tabla=string_duplicate(nombre_tabla);
	list_add(tabla_de_segmentos,segmento);
	return segmento;
}

t_list * recopilar_paginas_modificadas(){
	t_list * paginas_modificadas=list_create();
	bool es_pagina_modificada(void * pagina){
		return ((t_entrada_tabla_paginas *)pagina)->flag==FLAG_MODIFICADO;
	}
	recopilar_paginas_de_segmento_por_criterio(paginas_modificadas, es_pagina_modificada);
	return paginas_modificadas;
}

void recopilar_paginas_de_segmento_por_criterio(t_list *paginas_recopiladas, bool (*criterio)(void*)){
	void recopilador_paginas_de_segmento_por_criterio(void * un_segmento){
		t_list* paginas_segmento_que_cumplen_criterio = list_filter(
				((t_entrada_tabla_segmentos*) un_segmento)->base, criterio);
		if(!list_is_empty(paginas_segmento_que_cumplen_criterio)){
			list_add_all(paginas_recopiladas, paginas_segmento_que_cumplen_criterio);
		}
		list_destroy(paginas_segmento_que_cumplen_criterio);
	}
	list_iterate(tabla_de_segmentos, recopilador_paginas_de_segmento_por_criterio);
}

int ejecutar_algoritmo_reemplazo_y_obtener_marco(){
	logger(escribir_loguear, l_trace, "Voy a aplicar el algoritmo Last Recently Used");
	t_list * paginas_no_modificadas=list_create();
	bool es_pagina_no_modificada(void * pagina){
		return ((t_entrada_tabla_paginas *)pagina)->flag==FLAG_NO_MODIFICADO;
	}
	recopilar_paginas_de_segmento_por_criterio(paginas_no_modificadas,es_pagina_no_modificada);
	if(list_is_empty(paginas_no_modificadas)){
		//Ya no quedan páaginas sin modificar
		list_destroy(paginas_no_modificadas);
		return -1;
	}
	//Elige la más antigua, la elimina de la tabla y devuelve el nro de marco
	bool mas_antigua_adelante(void * primera_pag, void * segunda_pag){
		return ((t_entrada_tabla_paginas *)primera_pag)->ultimo_uso
				< ((t_entrada_tabla_paginas *)segunda_pag)->ultimo_uso;
	}
	list_sort(paginas_no_modificadas, mas_antigua_adelante);
	t_entrada_tabla_paginas * pagina_mas_antigua=list_get(paginas_no_modificadas,0);
	int marco_de_la_pag_mas_antigua=pagina_mas_antigua->marco;
	bool es_la_pagina_mas_antigua(void* pagina){
		return ((t_entrada_tabla_paginas *)pagina)==pagina_mas_antigua;
	}
	void buscar_tabla_paginas_para_eliminar_pagina_antigua(void * un_segmento){
		list_remove_and_destroy_by_condition(((t_entrada_tabla_segmentos*) un_segmento)->base,
				es_la_pagina_mas_antigua, destructor_pagina);
	}
	list_iterate(tabla_de_segmentos, buscar_tabla_paginas_para_eliminar_pagina_antigua);
	list_destroy(paginas_no_modificadas);
	return marco_de_la_pag_mas_antigua;
}

void insertar_registro_en_marco(long timestamp, uint16_t key, char *value, int marco){
	int desplazamiento=obtener_1er_byte_marco_en_MP(marco);
	logger(escribir_loguear, l_debug, "El marco %d tiene como primer byte %d",marco,desplazamiento);
	memcpy(MEMORIA_PRINCIPAL+desplazamiento,&timestamp,sizeof(long));
	desplazamiento+=sizeof(long);
	memcpy(MEMORIA_PRINCIPAL+desplazamiento,&key,sizeof(uint16_t));
	desplazamiento+=sizeof(uint16_t);
	int tamanio_real_value=strlen(value);
	memcpy(MEMORIA_PRINCIPAL+desplazamiento,value,tamanio_real_value);
	desplazamiento+=tamanio_real_value;
	char barra_cero='\0';
	memcpy(MEMORIA_PRINCIPAL+desplazamiento,&barra_cero,1);
}

int obtener_1er_byte_marco_en_MP(int marco){
	return obtener_tamanio_marco()*marco;
}

void crear_pagina_en_tabla_paginas(t_entrada_tabla_segmentos * segmento, int marco, int flag){
	t_entrada_tabla_paginas * entrada_pagina=malloc(sizeof(t_entrada_tabla_paginas));
	entrada_pagina->pagina=list_size(segmento->base);
	entrada_pagina->marco=marco;
	entrada_pagina->flag=flag;
	entrada_pagina->ultimo_uso=(unsigned int)time(NULL);
	list_add(segmento->base,entrada_pagina);
	bitarray_clean_bit(bitmap_marcos,marco);
	bitarray_set_bit(bitmap_marcos,marco);
}

void liberar_segmento_de_MP(t_entrada_tabla_segmentos * segmento){
	if(!list_is_empty(segmento->base)){
		void liberar_marco(void * entrada_pagina){
			bitarray_clean_bit(bitmap_marcos,(*(t_entrada_tabla_paginas *)entrada_pagina).marco);
		}
		list_iterate(segmento->base, liberar_marco);
	}
	bool es_segmento_de_tabla(void * un_segmento){
		return string_equals_ignore_case((*(t_entrada_tabla_segmentos *)un_segmento).tabla,segmento->tabla);
	}
	list_remove_and_destroy_by_condition(tabla_de_segmentos,es_segmento_de_tabla,destructor_segmento);
}

void limpiar_tablas_de_segmentos_y_paginas(){
	//Limpia todas las estrucuturas administrativas
	list_clean_and_destroy_elements(tabla_de_segmentos, destructor_segmento);

	if(bitmap_marcos!=NULL){
		liberar_bitmap_marcos();
	}
	inicializar_bitmap_marcos();
}

void insertar_cada_registro_modificado_en_LFS(enum MENSAJES* resultado_anterior,t_list* paginas_modificadas, int socket_con_LFS){
	//Itera cada página modificada y se la manda a LFS
	//Si algún insert falló, devuelve el error al Kernel e informa en consola
	void enviar_registro_a_liss(void * pagina){
		usleep(RETARDO_ACCESO_MEMORIA*1000);
		long* p_timestamp = obtener_timestamp_desde_marco_en_MP(((t_entrada_tabla_paginas *)pagina)->marco);
		uint16_t* p_key = obtener_key_desde_marco_en_MP(((t_entrada_tabla_paginas *)pagina)->marco);
		char * p_value = obtener_value_desde_marco_en_MP(((t_entrada_tabla_paginas *)pagina)->marco);

		bool es_el_segmento_de_la_pagina(void * segmento){

			//Devuelve si es el segmento de la página
			bool es_una_pagina_del_segmento(void * posible_pagina_del_segmento){
				return pagina==posible_pagina_del_segmento;
			}
			int es_del_segmento=list_count_satisfying(((t_entrada_tabla_segmentos*) segmento)->base
					, es_una_pagina_del_segmento);
			if (es_del_segmento>0){
				return true;
			}
			return false;
		}
		t_entrada_tabla_segmentos* segmento=list_find(tabla_de_segmentos, es_el_segmento_de_la_pagina);
		char * nombre_tabla=segmento->tabla;

		logger(escribir_loguear, l_info, "Se va a realizar el insert para la key %d de la tabla %s"
				,(int)*p_key,nombre_tabla);
		usleep(RETARDO_ACCESO_FILESYSTEM*1000);
		prot_enviar_insert(nombre_tabla, *p_key, p_value, *p_timestamp, socket_con_LFS);

		enum MENSAJES insercion = prot_recibir_respuesta_insert(socket_con_LFS);
		if(insercion == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "El insert se realizó correctamente\n");
		} else {
			logger(escribir_loguear, l_warning, "El insert falló\n");
			*resultado_anterior=insercion;
		}
		free(p_timestamp);
		free(p_key);
		free(p_value);
	}
	list_iterate(paginas_modificadas, enviar_registro_a_liss);
}
