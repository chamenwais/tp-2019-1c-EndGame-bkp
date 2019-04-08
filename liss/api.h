/*
 * api.h
 *
 *  Created on: 5 abr. 2019
 *      Author: utnso
 */

#ifndef API_H_
#define API_H_

#include <pthread.h>
#include <readline/readline.h>
#include "lissandra.h"

int lanzarConsola();
void *funcionHiloConsola(void *arg);
int esperarAQueTermineLaConsola();
char** parser_instruccion(char* linea);
int consolaSelect(char* nombreDeLaTabla,uint16_t key);
int consolaInsert(char* nombreDeLaTabla,uint16_t key,char* valor,int timestamp);
int consolaCreate(char* nombreDeLaTabla,char* tipoDeConsistencia,int numeroDeParticiones,int tiempoDeCompactacion);
int consolaDescribe(char* nombreDeLaTabla);
int consolaDrop(char* nombreDeLaTabla);
int man();
int imprimirConfiguracionDelSistema();


#endif /* SRC_API_H_ */
