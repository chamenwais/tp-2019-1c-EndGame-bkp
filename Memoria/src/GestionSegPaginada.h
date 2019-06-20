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
int colocar_value_en_MP(char *, long, uint16_t,char *);
int insertar_value_modificado_en_MP(char *, long, uint16_t,char *);
void actualizar_value_modificado_en_MP(char *, long, uint16_t,char *);
t_entrada_tabla_segmentos * buscar_segmento_de_tabla(char *);
t_entrada_tabla_segmentos * crear_segmento_a_tabla(char *);

/**Ve si están todos los marcos modificados y en ese caso devuelve -1 para pedir ejecutar journaling.
Si no, obtiene marco sin modificar, borra la página de la tabla que lo usaba y devuelve el nro */
int ejecutar_algoritmo_reemplazo_y_obtener_marco();
void insertar_registro_en_marco(long , uint16_t , char *, int );
void crear_pagina_en_tabla_paginas(t_entrada_tabla_segmentos *, int, int);
t_entrada_tabla_segmentos* obtener_segmento_de_tabla(char*);
int obtener_marco_libre();
int almacenar_valor(char*, long, uint16_t, char*, int);
int obtener_1er_byte_marco_en_MP(int);
long * obtener_timestamp_desde_marco_en_MP(int);
uint16_t* obtener_key_desde_marco_en_MP(int);
t_entrada_tabla_paginas * buscar_pagina_de_key_en_MP(t_list*,uint16_t);
void liberar_segmento_de_MP(t_entrada_tabla_segmentos *);
void destructor_pagina(void *);
void destructor_segmento(void *);
void recopilar_paginas_de_segmento_por_criterio(t_list *, bool (*criterio)(void*));
t_list * recopilar_paginas_modificadas();
void limpiar_tablas_de_segmentos_y_paginas();
long * obtener_timestamp_desde_marco_en_MP(int);
uint16_t* obtener_key_desde_marco_en_MP(int);
char * obtener_value_desde_marco_en_MP(int);
void insertar_cada_registro_modificado_en_LFS(enum MENSAJES*,t_list*,int);

#endif /* SRC_GESTIONSEGPAGINADA_H_ */
