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
#include <lqvg/utils.h>
#include <semaphore.h>
#include <commons/config.h>

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

typedef struct memo_del_pool_kernel {
	char * ip;
	int puerto;
	int numero_memoria;
	int socket;
}t_memo_del_pool_kernel;
typedef t_memo_del_pool_kernel* tp_memo_del_pool_kernel;

typedef struct lql_pcb{
	char* path; //el path del archivo LQL
	t_list* lista;//lista con las requests
}t_lql_pcb;
typedef t_lql_pcb* tp_lql_pcb;

typedef struct entrada_tabla_creada {
	char* nombre_tabla;
	char* criterio;
}t_entrada_tabla_creada;
typedef t_entrada_tabla_creada* tp_entrada_tabla_creada;

enum tipo_consistencia {
	SC,
	HC,
	EC
};

typedef struct {
	enum {
		_SELECT,
		_INSERT,
		_CREATE,
		_DESCRIBE,
		_DROP,
		_JOURNAL,
		ADD,
		RUN,
		METRICS,
		MEMORY,
		TO
	} tipo_de_operacion;
	union {
		struct {
			char* nombre_tabla;
			int key;
		} select;
		struct {
			char* nombre_tabla;
			uint16_t key;
			char* value;
		} insert;
		struct {
			char* nombre_tabla;
			char* tipo_consistencia; //es un tipo_consistencia del enum anterior
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
			int tipo_consistencia; //es un tipo_consistencia del enum anterior... o deberia ser char*?
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
extern pthread_t threadRequest;
extern pthread_t threadPCP;
extern pthread_t hiloDescribeAll;
extern t_list* listaNew;
extern t_list* listaReady;
extern t_list* listaExec;
extern t_list* listaExit;
extern t_list* listaMemConectadas;
extern t_list* listaTablasCreadas;
extern t_list* listaEC;
extern t_list* listaSC;
extern t_list* listaHC;
extern pthread_mutex_t mutex_New;
extern pthread_mutex_t mutex_Ready;
extern pthread_mutex_t mutex_Exec;
extern pthread_mutex_t mutex_Exit;
extern pthread_mutex_t mutex_MemConectadas;
extern pthread_mutex_t mutex_EC;
extern pthread_mutex_t mutex_HC;
extern pthread_mutex_t mutex_SC;
extern sem_t NEW;
extern sem_t READY;
extern int quantum;
extern int retardo;
extern int path_api;
int ultima_memoria_EC;

int inicializarVariablesGlobales();
void terminar_programa(int codigo_finalizacion);
void configurar_signals(void);
void captura_signal(int);
void logger(int tipo_esc, int tipo_log, const char* mensaje, ...);
void escribir_por_pantalla(int tipo_esc, int tipo_log, char* console_buffer,
char* log_colors[8], char* msj_salida);
void definir_nivel_y_loguear(int tipo_esc, int tipo_log, char* msj_salida);
void apagar_semaforos();

#endif /* VARIABLESGLOBALES_H_ */
