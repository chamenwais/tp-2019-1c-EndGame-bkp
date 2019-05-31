/*
 * variablesGlobales.h
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#ifndef VARIABLESGLOBALES_H_
#define VARIABLESGLOBALES_H_

#include <commons/log.h>
#include <stdlib.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <lqvg/protocolo.h>
#include <commons/string.h> // Para manejo de strings
#include <signal.h>			// Para manejo de señales
#include <commons/collections/list.h>

enum tipo_logueo {
	escribir,
	loguear,
	escribir_loguear,
	l_trace,
	l_debug,
	l_info,
	l_warning,
	l_error
};

typedef struct definicionConfiguracionDelKernel {
	char* ipMemoria; //de la primera memoria q se conecta
	int puertoMemoria;
	int quantum;
	int multiprocesamiento; //cantidad de scripts corriendo a la vez
	int refreshMetadata;
	int retardoCiclo;
} t_configuracionDelKernel;

struct memo_del_pool_kernel {
	char * ip;
	char * puerto;
	int socket;
};
typedef struct memo_del_pool_kernel t_memo_del_pool_kernel;

enum tipo_consistencia {
	SC,
	HC,
	EC
};

typedef struct {
	enum {
		/*SELECT,
		INSERT,
		CREATE,
		DESCRIBE,
		DROP,
		JOURNAL,*/
		ADD,
		RUN,
		METRICS,
		MEMORY, //ver si memory y to sirven desde aca, sino sacarlos en otro enum TODO
		TO
	} tipo_de_operacion;
	union {
		struct {
			char* nombre_tabla;
			int key;
		} select;
		struct {
			char* nombre_tabla;
			int key;
			char* value;
		} insert;
		struct {
			char* nombre_tabla;
			int tipo_consistencia; //es un tipo_consistencia del enum anterior
			int num_particiones;
			int compaction_time;
		} create;
		struct {
			char* nombre_tabla; //TODO puede ser NULL
		} describe;
		struct {
			char* nombre_tabla;
		} drop;
		//JOURNAL no aparece porque no lleva parametros
		struct {
			int memory; //enum
			int num_memoria;
			int to; //enum
			int tipo_consistencia; //es un tipo_consistencia del enum anterior
		} add;
		struct {
			char* path;
		} run;
		//METRICS no aparece porque no lleva parametros

	} parametros;
} t_operacion;


extern t_configuracionDelKernel configKernel;
extern t_log* LOG_KERNEL;
extern pthread_t threadConsola;
extern pthread_t threadPlanif;
extern pthread_t threadMemo;
extern t_list* listaNew;
extern t_list* listaReady;
extern t_list* listaExec;
extern t_list* listaExit;
extern t_list* listaMemConectadas;

int inicializarVariablesGlobales();
int liberarRecursos();

#endif /* VARIABLESGLOBALES_H_ */
