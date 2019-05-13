#ifndef LQVG_PROTOCOLO_H_
#define LQVG_PROTOCOLO_H_

#include "com.h"
#include <commons/string.h>

typedef struct struSelect{
	char * nom_tabla;
	int key;
} t_select;
typedef t_select* tp_select;

void prot_enviar_int(int,int);
int prot_recibir_int(int);
void prot_enviar_select(char *, int, int);
tp_select prot_recibir_select(int, int);

#endif
