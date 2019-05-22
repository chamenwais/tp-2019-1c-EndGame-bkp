/*
 * GestionSegPaginada.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "GestionSegPaginada.h"

int obtener_tamanio_marco() {
	int tamanio_marco = sizeof(long) + sizeof(uint16_t) + sizeof(TAMANIO_VALUE)
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
		if(bitarray_test_bit(bitmap_marcos,i)){
			logger(escribir_loguear, l_trace,"El bloque %d esta libre", i);
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

void liberar_tabla_segmentos(){
	list_destroy(tabla_de_segmentos);
}

tp_select_rta verificar_existencia_en_MP(char * nombre_tabla, uint16_t key){
	tp_select_rta marco = malloc(sizeof(t_select_rta));
	marco->value=NULL;
	return marco;
}

void colocar_value_en_MP(char *nom_tabla, long timestamp, uint16_t key, char *value){
	t_entrada_tabla_segmentos * segmento = buscar_segmento_de_tabla(nom_tabla);

}

t_entrada_tabla_segmentos * buscar_segmento_de_tabla(char * nombre_tabla){
	t_entrada_tabla_segmentos * segmento=NULL;
	return segmento;
}
