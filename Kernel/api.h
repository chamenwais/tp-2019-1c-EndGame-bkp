/*
 * api.h
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#ifndef API_H_
#define API_H_

#include <pthread.h>
#include <readline/readline.h>
#include "variablesGlobales.h"
#include "funcionesKernel.h"

int lanzarConsola();
int esperarAQueTermineLaConsola();
void *funcionHiloConsola(void *arg);
char** parser_instruccion(char* linea);
int selectConsola(char* linea);
int insertConsola(char* linea);
int insertConsolaNoTime(char* linea);
int createConsola(char* linea);
int describeConsola(char* linea);
int describeConsolaAll(char* linea);
int dropConsola(char* linea);
int realoadConfig();
int man();
int journalConsola(char* linea);
int runConsola(char* path);
void metricsConsola();
int addConsola(int memnum, char* criterio);
tp_lql_pcb crear_PCB(char* path);
bool existeMemoria(int numero);


#endif /* API_H_ */
