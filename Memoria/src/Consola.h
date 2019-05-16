/*
 * Consola.h
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#ifndef MEMORIA_SRC_CONSOLA_H_
#define MEMORIA_SRC_CONSOLA_H_

#include "Contexto.h"
#include "Utilidades.h"
#include "GestionSegPaginada.h"

#define _SELECT "SELECT"
#define _INSERT "INSERT"
#define _CREATE "CREATE"
#define _DESCRIBE "DESCRIBE"
#define _DROP "DROP"
#define _JOURNAL "JOURNAL"

void consola_select(char**);
void consola_insert(char**);
void consola_create(char**);
void consola_describe(char**);
void consola_drop(char**);
void consola_journal(int);
int consola_obtener_key_comando(char**, int);
void obtener_un_parametro(char** , char**, char *);
void loguear_un_parametros_recibido(char * );
void obtener_dos_parametros(char**, char**, char**, char *);
void loguear_dos_parametros_recibidos(char *, char * );
void obtener_cuatro_parametros(char**, char**, char**, char**, char**, char *);
void loguear_cuatro_parametros_recibidos(char *, char *, char *, char*);
int validar_parametro_consola(char ** );
int consola_derivar_comando(char *, int);
void *consola();
int consola_leer_stdin(char *, size_t );
void limpiar_parametro(char* );


#endif /* MEMORIA_SRC_CONSOLA_H_ */
