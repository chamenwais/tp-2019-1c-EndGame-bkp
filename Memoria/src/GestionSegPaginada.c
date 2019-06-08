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

void destructor_segmento(void * segmento){
	void destructor_pagina(void * pagina){
		free(pagina);
	}
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

void almacenar_valor(char* nom_tabla, long timestamp, uint16_t key, char* value,
		int flag) {
	t_entrada_tabla_segmentos* segmento = obtener_segmento_de_tabla(nom_tabla);
	int marco_asignado = obtener_marco_libre();
	usleep(RETARDO_ACCESO_MEMORIA*1000);
	insertar_registro_en_marco(timestamp, key, value, marco_asignado);
	crear_pagina_en_tabla_paginas(segmento, marco_asignado, flag);
}

void colocar_value_en_MP(char *nom_tabla, long timestamp, uint16_t key, char *value){
	almacenar_valor(nom_tabla, timestamp, key, value, FLAG_NO_MODIFICADO);
}

void insertar_value_modificado_en_MP(char *nom_tabla, long timestamp, uint16_t key, char *value){
	almacenar_valor(nom_tabla, timestamp, key, value, FLAG_MODIFICADO);
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

int ejecutar_algoritmo_reemplazo_y_obtener_marco(){
	/*TODO ejecutar algoritmo de reemplazo
	ver	si están todos los marcos modificados y en ese caso ejecutar journaling
	si no, obtener marco sin modificar, borrar página de la tabla que lo usaba y devolver nro */
	return 0;
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
