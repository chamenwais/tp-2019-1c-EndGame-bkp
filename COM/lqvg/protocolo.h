#ifndef LQVG_PROTOCOLO_H_
#define LQVG_PROTOCOLO_H_

#include "com.h"
#include <commons/string.h>
#include <commons/collections/list.h>

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

typedef struct struCreate{
	char * nom_tabla;
	char* tipo_consistencia;
	int numero_particiones;
	int tiempo_compactacion;
} t_create;
typedef t_create* tp_create;

typedef struct struDescribe{
	char * nom_tabla;
} t_describe;
typedef t_describe* tp_describe;

typedef struct struDrop{
	char * nom_tabla;
} t_drop;
typedef t_drop* tp_drop;

typedef struct struSelectRta{
	char* value;
} t_select_rta;
typedef t_select_rta* tp_select_rta;

typedef struct struDescribeRta{
	int particiones;
	char* consistencia;
	int tiempo_compactacion;
} t_describe_rta;
typedef t_describe_rta* tp_describe_rta;

typedef struct procesarMsg{
	int cliente;
	t_cabecera cabecera;
}datos_iniciales;

typedef struct metadataDescribe{//para lista de describe all
	int particiones;
	char* consistencia;
	int tiempoDeCompactacion;
}datos_describe;
typedef datos_describe* tp_datos_describe;

typedef struct listaDescribeAll{//va a ser una lista de datos_describe
	t_list *lista;
}t_describeAll_rta;
typedef t_describeAll_rta* tp_describeAll_rta;

void prot_enviar_int(int,int);
int prot_recibir_int(int);

void prot_enviar_select(char *, uint16_t, int);
tp_select prot_recibir_select(int, int);
void prot_enviar_respuesta_select(char *, int);
tp_select_rta prot_recibir_respuesta_select(int,int);//@necesita antes hacer un recibirCabecera para el tamaño

void prot_enviar_insert(char *, uint16_t, char *, long, int);
tp_insert prot_recibir_insert(int, int);
void prot_enviar_respuesta_insert(int);
enum MENSAJES prot_recibir_respuesta_insert(int);

void prot_enviar_create(char *, char *, int, int,int);
tp_create prot_recibir_create(int, int);
void prot_enviar_respuesta_create(int);
enum MENSAJES prot_recibir_respuesta_create(int);

void prot_enviar_describe(char *,int);
tp_describe prot_recibir_describe(int, int);
void prot_enviar_respuesta_describe(int, char*, int, int);
tp_describe_rta prot_recibir_respuesta_describe(int,int);//@necesita antes hacer un recibirCabecera para el tamaño

void prot_enviar_drop(char *,int);
tp_drop prot_recibir_drop(int, int);
void prot_enviar_respuesta_drop(int);
enum MENSAJES prot_recibir_respuesta_drop(int);

void prot_enviar_describeAll(int);
//no hay prot_recibir_ describeAll, cuando antes le hagas recibirCabecera ya vas a tener todos los datos necesarios
void prot_enviar_respuesta_describeAll(t_describeAll_rta,int);
tp_describeAll_rta prot_recibir_respuesta_describeAll(int,int);//@necesita antes hacer un recibirCabecera para el tamaño
void prot_free_tp_describeAll_rta(tp_describeAll_rta);//para eliminar la lista una vez se usó

void prot_enviar_error(enum MENSAJES, int);
#endif
