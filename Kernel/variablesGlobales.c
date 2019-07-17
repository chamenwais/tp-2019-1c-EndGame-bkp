/*
 * variablesGlobales.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "variablesGlobales.h"

t_configuracionDelKernel configKernel;
t_config * k_config;
t_log* LOG_KERNEL;
pthread_t threadConsola;
pthread_t threadPlanif;
pthread_t threadRequest;
pthread_t threadPCP;
pthread_t hiloDescribeAll;
pthread_t threadMonitoreadorDeArchivos;
pthread_t hiloGossip;
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
pthread_mutex_t mutex_tablas;
pthread_mutex_t mutexVariableQuantum;
pthread_mutex_t mutexVariableRefresh;
pthread_mutex_t mutexVariableRetardo;
pthread_mutex_t mutexVariableGossip;
pthread_mutex_t M_RUTA_ARCHIVO_CONF;
pthread_mutex_t M_CONF_FD;
pthread_mutex_t M_WATCH_DESCRIPTOR;
pthread_mutex_t M_PATH_ARCHIVO_CONFIGURACION;

sem_t NEW;
sem_t READY;

int quantum;
int retardo;
int path_api = 0;
int ultima_memoria_EC = 100;
int socket_primera_memoria;

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
		pthread_mutex_destroy(&mutex_tablas);
		pthread_mutex_destroy(&mutexVariableQuantum);
		pthread_mutex_destroy(&mutexVariableRefresh);
		pthread_mutex_destroy(&mutexVariableRetardo);
		pthread_mutex_destroy(&mutexVariableGossip);
		pthread_mutex_destroy(&M_RUTA_ARCHIVO_CONF);
		pthread_mutex_destroy(&M_CONF_FD);
		pthread_mutex_destroy(&M_WATCH_DESCRIPTOR);
		pthread_mutex_destroy(&M_PATH_ARCHIVO_CONFIGURACION);

}

void destructor_pcb(void* pcb){
	free(((t_lql_pcb *)pcb)->path);
	void destructor_lista_pcb(void* nodo){
		free((char*)nodo);
	}
	if(!list_is_empty(((t_lql_pcb *)pcb)->lista)){
		list_destroy_and_destroy_elements(((t_lql_pcb *)pcb)->lista, destructor_lista_pcb);
	}else{
		list_destroy(((t_lql_pcb *)pcb)->lista);
	}
	free((t_lql_pcb *)pcb);
}

void destructor_memoria_del_pool_kernel(void * memo_del_pool){
		free(((t_memo_del_pool_kernel*) memo_del_pool)->ip);
		free(((t_memo_del_pool_kernel*) memo_del_pool)->puerto);
		//free(((t_memo_del_pool_kernel*) memo_del_pool)->numero_memoria);
		//free(((t_memo_del_pool_kernel*) memo_del_pool)->socket);
		free((t_memo_del_pool_kernel*)memo_del_pool);
}

void destruirListas(){

	if(!list_is_empty(listaNew)){
		pthread_mutex_lock(&mutex_New);
		list_destroy_and_destroy_elements(listaNew, destructor_pcb);
		pthread_mutex_unlock(&mutex_New);
	}else{
		pthread_mutex_lock(&mutex_New);
		list_destroy(listaNew);
		pthread_mutex_unlock(&mutex_New);
	}
	if(!list_is_empty(listaReady)){
		pthread_mutex_lock(&mutex_Ready);
		list_destroy_and_destroy_elements(listaReady, destructor_pcb);
		pthread_mutex_unlock(&mutex_Ready);
	}else{
		pthread_mutex_lock(&mutex_Ready);
		list_destroy(listaReady);
		pthread_mutex_unlock(&mutex_Ready);
	}
	if(!list_is_empty(listaExec)){
		pthread_mutex_lock(&mutex_Exec);
		list_destroy_and_destroy_elements(listaExec, destructor_pcb);
		pthread_mutex_unlock(&mutex_Exec);
	}else{
		pthread_mutex_lock(&mutex_Exec);
		list_destroy(listaExec);
		pthread_mutex_unlock(&mutex_Exec);
	}
	if(!list_is_empty(listaExit)){
		pthread_mutex_lock(&mutex_Exit);
		list_destroy_and_destroy_elements(listaExit, destructor_pcb);
		pthread_mutex_unlock(&mutex_Exit);
	}else{
		pthread_mutex_lock(&mutex_Exit);
		list_destroy(listaExit);
		pthread_mutex_unlock(&mutex_Exit);
	}
	if(!list_is_empty(listaMemConectadas)){
		pthread_mutex_lock(&mutex_MemConectadas);
		list_destroy_and_destroy_elements(listaMemConectadas, destructor_memoria_del_pool_kernel);
		pthread_mutex_unlock(&mutex_MemConectadas);
	}else{
		pthread_mutex_lock(&mutex_MemConectadas);
		list_destroy(listaMemConectadas);
		pthread_mutex_unlock(&mutex_MemConectadas);
	}
	if(!list_is_empty(listaSC)){
		pthread_mutex_lock(&mutex_SC);
		list_destroy_and_destroy_elements(listaSC, destructor_memoria_del_pool_kernel);
		pthread_mutex_unlock(&mutex_SC);
	}else{
		pthread_mutex_lock(&mutex_SC);
		list_destroy(listaSC);
		pthread_mutex_unlock(&mutex_SC);
	}
	if(!list_is_empty(listaEC)){
		pthread_mutex_lock(&mutex_EC);
		list_destroy_and_destroy_elements(listaEC, destructor_memoria_del_pool_kernel);
		pthread_mutex_unlock(&mutex_EC);
	}else{
		pthread_mutex_lock(&mutex_EC);
		list_destroy(listaEC);
		pthread_mutex_unlock(&mutex_EC);
	}
	if(!list_is_empty(listaHC)){
		pthread_mutex_lock(&mutex_HC);
		list_destroy_and_destroy_elements(listaHC, destructor_memoria_del_pool_kernel);
		pthread_mutex_unlock(&mutex_HC);
	}else{
		pthread_mutex_lock(&mutex_HC);
		list_destroy(listaHC);
		pthread_mutex_unlock(&mutex_HC);
	}

	void destructor_tablas(void * tabla){
			free(((t_entrada_tabla_creada*) tabla)->nombre_tabla);
			free(((t_entrada_tabla_creada*) tabla)->criterio);
			free((t_memo_del_pool_kernel *) tabla);
	}
	if(!list_is_empty(listaTablasCreadas)){
		list_destroy_and_destroy_elements(listaTablasCreadas, destructor_tablas);
	}else{
		list_destroy(listaTablasCreadas);
	}
}

void terminar_programa(int codigo_finalizacion){
	logger(escribir_loguear, l_warning,"Se va a finalizar el kernel.");
	log_destroy(LOG_KERNEL);
	config_destroy(k_config);

	pthread_mutex_lock(&M_WATCH_DESCRIPTOR);
	inotify_rm_watch(conf_fd, watch_descriptor);
	pthread_mutex_unlock(&M_WATCH_DESCRIPTOR);

	pthread_mutex_lock(&M_CONF_FD);
	close(conf_fd);
	pthread_mutex_unlock(&M_CONF_FD);

	pthread_mutex_lock(&M_PATH_ARCHIVO_CONFIGURACION);
	free(path_archivo_configuracion);
	pthread_mutex_unlock(&M_PATH_ARCHIVO_CONFIGURACION);

	pthread_mutex_lock(&M_RUTA_ARCHIVO_CONF);
	free(ruta_archivo_conf->directorio);
	free(ruta_archivo_conf->nombre_archivo);
	free(ruta_archivo_conf);
	pthread_mutex_unlock(&M_RUTA_ARCHIVO_CONF);

	destruirListas();

	apagar_semaforos();

	//free(socket_primera_memoria);
	//free(path_api);
	//free(quantum);
	//free(retardo);
	//free(ultima_memoria_EC);


	exit(codigo_finalizacion);
}

