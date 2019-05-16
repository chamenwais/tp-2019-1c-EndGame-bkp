#ifndef LQVG_PROTOCOLO_H_
#define LQVG_PROTOCOLO_H_

#include "com.h"
#include <commons/string.h>

typedef struct struSelect{
	char * nom_tabla;
	uint16_t key;
} t_select;
typedef t_select* tp_select;

typedef struct struInsert{
	char * nom_tabla;
	uint16_t key;
	char * value;
	long timestamp;
} t_insert;
typedef t_insert* tp_insert;

typedef struct struSelectRta{
	char* value;
} t_select_rta;
typedef t_select_rta* tp_select_rta;

typedef struct procesarMsg{
	int cliente;
	t_cabecera cabecera;
}datos_iniciales;

void prot_enviar_int(int,int);
int prot_recibir_int(int);

void prot_enviar_select(char *, uint16_t, int);
tp_select prot_recibir_select(int, int);
void prot_enviar_respuesta_select(char *, int);
tp_select_rta prot_recibir_respuesta_select(int,int);

void prot_enviar_insert(char *, uint16_t, char *, long, int);
tp_insert prot_recibir_insert(int, int);
void prot_enviar_respuesta_insert(enum MENSAJES,int);
enum MENSAJES prot_recibir_respuesta_insert(int);

void prot_enviar_error(enum MENSAJES, int);
#endif
