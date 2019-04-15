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

void consola_select();
void consola_insert();
void consola_create();
void consola_describe();
void consola_drop();
void consola_journal();
int consola_obtener_key_comando(char* comando);
void consola_obtener_parametros(char* buffer, char** comando, char** parametro1, char** parametro2);
void validar_parametro_consola(char ** parametro);
void limpiar_token_consola(char* token);
int consola_derivar_comando(char * buffer);
void *consola();
int consola_leer_stdin(char *read_buffer, size_t max_len);


#endif /* MEMORIA_SRC_CONSOLA_H_ */
