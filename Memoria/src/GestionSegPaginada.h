/*
 * GestionSegPaginada.h
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#ifndef SRC_GESTIONSEGPAGINADA_H_
#define SRC_GESTIONSEGPAGINADA_H_

#include "Contexto.h"
#include "Utilidades.h"

/* Esta sería la estructura de un marco en la MP
 *
struct stru_marco{
	long timestamp;
	uint16_t key;
	char value[TAMANIO_VALUE];
};
typedef struct stru_marco t_marco;*/

int obtener_tamanio_marco();
int obtener_cantidad_marcos_en_MP(int );
void inicializar_bitmap_marcos();
void inicializar_tabla_segmentos();
void liberar_bitmap_marcos();
void liberar_tabla_segmentos();
int obtener_marco_libre_del_bitmap();
void ocupar_marco_en_el_bitmap(int);
tp_select_rta verificar_existencia_en_MP(char *, uint16_t);
void colocar_value_en_MP(char *, long, uint16_t,char *);
void insertar_value_modificado_en_MP(char *, long, uint16_t,char *);
t_entrada_tabla_segmentos * buscar_segmento_de_tabla(char *);
t_entrada_tabla_segmentos * crear_segmento_a_tabla(char *);
int ejecutar_algoritmo_reemplazo_y_obtener_marco();
void insertar_registro_en_marco(long , uint16_t , char *, int );
void crear_pagina_en_tabla_paginas(t_entrada_tabla_segmentos *, int, int);
t_entrada_tabla_segmentos* obtener_segmento_de_tabla(char*);
int obtener_marco();
void almacenar_valor(char*, long, uint16_t, char*, int);
int obtener_1er_byte_marco_en_MP(int);
long * obtener_timestamp_desde_marco_en_MP(int);
uint16_t* obtener_key_desde_marco_en_MP(int);

#endif /* SRC_GESTIONSEGPAGINADA_H_ */
