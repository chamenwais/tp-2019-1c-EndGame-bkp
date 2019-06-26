/*
 * variablesGlobales.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "variablesGlobales.h"

t_configuracionDelKernel configKernel;
t_log* LOG_KERNEL;
pthread_t threadConsola;
pthread_t threadPlanif;
pthread_t threadRequest;
pthread_t threadPCP;
pthread_t hiloDescribeAll;
t_list* listaNew;
t_list* listaReady;
t_list* listaExec;
t_list* listaExit;
t_list* listaMemConectadas;
t_list* listaTablasCreadas;
t_list* listaEC;
t_list* listaSC;
t_list* listaHC;
pthread_mutex_t mutex_New;
pthread_mutex_t mutex_Ready;
pthread_mutex_t mutex_Exec;
pthread_mutex_t mutex_Exit;
pthread_mutex_t mutex_MemConectadas;
pthread_mutex_t mutex_EC;
pthread_mutex_t mutex_HC;
pthread_mutex_t mutex_SC;

sem_t NEW;
sem_t READY;

int quantum;
int retardo;
int path_api = 0;

void configurar_signals(void) {
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGPIPE);
	if (sigaction(SIGPIPE, &signal_struct, NULL) < 0) {
		logger(escribir_loguear, l_error, " SIGACTION error ");
	}

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		logger(escribir_loguear, l_error, " SIGACTION error ");
	}

}

void captura_signal(int signo){

    if(signo == SIGINT)
    {
    	logger(escribir_loguear, l_warning,"Finalizando proceso kernel...");
    	terminar_programa(EXIT_SUCCESS);
    }
    else if(signo == SIGPIPE)
    {
    	logger(escribir_loguear, l_error," Se desconectó un proceso al que se quizo enviar.");

    }

}

void escribir_por_pantalla(int tipo_esc, int tipo_log, char* console_buffer,
		char* log_colors[8], char* msj_salida) {

	if ((tipo_esc == escribir) || (tipo_esc == escribir_loguear)) {
		console_buffer = string_from_format("%s%s%s", log_colors[tipo_log],
				msj_salida, log_colors[0]);
		printf("%s", console_buffer);
		printf("%s","\n");
		fflush(stdout);
		free(console_buffer);
	}
}

void definir_nivel_y_loguear(int tipo_esc, int tipo_log, char* msj_salida) {
	if ((tipo_esc == loguear) || (tipo_esc == escribir_loguear)) {
		if (tipo_log == l_info) {
			log_info(LOG_KERNEL, msj_salida);
		} else if (tipo_log == l_warning) {
			log_warning(LOG_KERNEL, msj_salida);
		} else if (tipo_log == l_error) {
			log_error(LOG_KERNEL, msj_salida);
		} else if (tipo_log == l_debug) {
			log_debug(LOG_KERNEL, msj_salida);
		} else if (tipo_log == l_trace) {
			log_trace(LOG_KERNEL, msj_salida);
		}
	}
}

void logger(int tipo_esc, int tipo_log, const char* mensaje, ...){

	//Colores (reset,vacio,vacio,cian,verde,vacio,amarillo,rojo)
	char *log_colors[8] = {"\x1b[0m","","","\x1b[36m", "\x1b[32m", "", "\x1b[33m", "\x1b[31m" };
	char *console_buffer=NULL;
	char *msj_salida = malloc(sizeof(char) * 256);

	//Captura los argumentos en una lista
	va_list args;
	va_start(args, mensaje);

	//Arma el mensaje formateado con sus argumentos en msj_salida.
	vsprintf(msj_salida, mensaje, args);

	escribir_por_pantalla(tipo_esc, tipo_log, console_buffer, log_colors,
			msj_salida);

	definir_nivel_y_loguear(tipo_esc, tipo_log, msj_salida);

	va_end(args);
	free(msj_salida);

	return;
}

int inicializarVariablesGlobales(){

	return EXIT_SUCCESS;
}

void apagar_semaforos(){
		sem_destroy(&NEW);
		sem_destroy(&READY);

		pthread_mutex_destroy(&mutex_New);
		pthread_mutex_destroy(&mutex_Ready);
		pthread_mutex_destroy(&mutex_Exec);
		pthread_mutex_destroy(&mutex_Exit);
		pthread_mutex_destroy(&mutex_MemConectadas);
		pthread_mutex_destroy(&mutex_SC);
		pthread_mutex_destroy(&mutex_EC);
		pthread_mutex_destroy(&mutex_HC);

}

void terminar_programa(int codigo_finalizacion){
	logger(escribir_loguear, l_warning,"Se va a finalizar el kernel.");
	log_destroy(LOG_KERNEL);
	//destruirListas();
	apagar_semaforos();

	exit(codigo_finalizacion);
}

