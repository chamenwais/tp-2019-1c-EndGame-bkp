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
#include "kernel.h"

int lanzarConsola();
int esperarAQueTermineLaConsola();
void *funcionHiloConsola(void *arg);
char** parser_instruccion(char* linea);
int selectConsola(char* nombreDeLaTabla,uint16_t key);
int insertConsola(char* nombreDeLaTabla,uint16_t key,char* valor,long timestamp);
int insertConsolaNoTime(char* nombreDeLaTabla,uint16_t key,char* valor);
int createConsola(char* nombreDeLaTabla,char* tipoDeConsistencia,int numeroDeParticiones,int tiempoDeCompactacion);
int describeConsola(char* nombreTabla);
int describeConsolaAll();
int dropConsola(char* nombreTabla);
int realoadConfig();
int journalConsola();
int runConsola(char* path);
void metricsConsola();
int addConsola(int memnum, char* criterio);
tp_lql_pcb crear_PCB(char* path);


#endif /* API_H_ */
