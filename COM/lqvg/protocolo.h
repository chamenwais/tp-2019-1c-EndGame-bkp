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
	unsigned int timestamp;
} t_insert;
typedef t_insert* tp_insert;

typedef struct memo_del_pool {
	char * ip;
	char * puerto;
}t_memo_del_pool;
typedef t_memo_del_pool* tp_memo_del_pool;

typedef struct struCreate{
	char * nom_tabla;
	char* tipo_consistencia;//SC, SHC, EC
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
	unsigned int timestamp;
	uint16_t key;
	char* value;
} t_select_rta;
typedef t_select_rta* tp_select_rta;

typedef struct struDescribeRta{
	char* nombre;//@@@@@@@@por que tengo dos describe rta, no puedo usar esto para all tmb o al reves?
	int particiones;
	char* consistencia;
	int tiempoDeCompactacion;
} t_describe_rta;
typedef t_describe_rta* tp_describe_rta;

typedef struct procesarMsg{
	int cliente;
	t_cabecera cabecera;
}datos_iniciales;

typedef struct listaDescribeAll{//va a ser una lista de tp_datos_describe
	t_list *lista;
}t_describeAll_rta;
typedef t_describeAll_rta* tp_describeAll_rta;

typedef struct tablaGossiping{
	t_list *lista;
}t_tabla_gossiping;
typedef t_tabla_gossiping* tp_tabla_gossiping;

void prot_enviar_int(int,int);
int prot_recibir_int(int);

//@@Antes de cada funcion de recibir, ya sea recibir o recibir_respuesta tenés que
//  hacer antes un recibirCabecera para el saber tamaño (excepto las que indiqué)

void prot_enviar_select(char *, uint16_t, int);
tp_select prot_recibir_select(int, int);
void prot_enviar_respuesta_select(char *,uint16_t,unsigned int, int);
tp_select_rta prot_recibir_respuesta_select(int,int);

void prot_enviar_insert(char *, uint16_t, char *, unsigned int, int);
tp_insert prot_recibir_insert(int, int);
void prot_enviar_respuesta_insert(int);
enum MENSAJES prot_recibir_respuesta_insert(int);//@NO necesita antes hacer un recibirCabecera

void prot_enviar_create(char *, char *, int, int,int);//consistencia=SC,SHC,EC
tp_create prot_recibir_create(int, int);
void prot_enviar_respuesta_create(int);
enum MENSAJES prot_recibir_respuesta_create(int);//@NO necesita antes hacer un recibirCabecera

void prot_enviar_describe(char *,int);
tp_describe prot_recibir_describe(int, int);
void prot_enviar_respuesta_describe(char*,int, char*, int, int);
tp_describe_rta prot_recibir_respuesta_describe(int,int);

void prot_enviar_drop(char *,int);
tp_drop prot_recibir_drop(int, int);
void prot_enviar_respuesta_drop(int);
enum MENSAJES prot_recibir_respuesta_drop(int);//@NO necesita antes hacer un recibirCabecera

void prot_enviar_describeAll(int);
//no hay prot_recibir_ describeAll, cuando antes le hagas recibirCabecera ya vas a tener todos los datos necesarios
void prot_enviar_respuesta_describeAll(t_describeAll_rta,int);
tp_describeAll_rta prot_recibir_respuesta_describeAll(int,int);


void prot_free_tp_describeAll_rta(tp_describeAll_rta);//para eliminar la lista una vez se usó

void prot_enviar_error(enum MENSAJES, int);//usarlo cada que vez que la respuesta a cualquier pedido no sea correcta

void prot_enviar_journal(int);
void prot_enviar_respuesta_journaling(int);
enum MENSAJES prot_recibir_respuesta_journal(int);//@NO necesita antes hacer un recibirCabecera

void prot_enviar_mi_tabla_gossiping(t_tabla_gossiping, int);
tp_tabla_gossiping prot_recibir_tabla_gossiping(int, int);
void prot_free_tp_tabla_gossiping(tp_tabla_gossiping);

#endif
