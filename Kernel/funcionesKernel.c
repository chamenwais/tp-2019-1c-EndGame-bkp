/*
 * funcionesKernel.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "funcionesKernel.h"

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
    	//terminar_programa(EXIT_SUCCESS); TODO
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
			log_info(LOG_KERNEL, escribir_loguear, l_info, msj_salida);
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

void inicializarLogKernel(){
	LOG_KERNEL = log_create("Kernel.log","Kernel",false,LOG_LEVEL_DEBUG);
	logger(escribir_loguear, l_info,"Log del Kernel iniciado");
	return;
}

int levantarConfiguracionInicialDelKernel(){

	char* ubicacionDelArchivoDeConfiguracion;
	ubicacionDelArchivoDeConfiguracion="kernel.config";

	t_config* configuracion = config_create(ubicacionDelArchivoDeConfiguracion);

	if(configuracion!=NULL){
		logger(escribir_loguear, l_info,"El archivo de configuracion existe");
	}else{
		log_error(LOG_KERNEL,"No existe el archivo de configuracion en: %s",ubicacionDelArchivoDeConfiguracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	logger(escribir_loguear, l_info,"Abriendo el archivo de configuracion del Kernel");

	//Recupero el IP de la Memoria
	if(!config_has_property(configuracion,"IP_MEMORIA")) {
		log_error(LOG_KERNEL,"No esta el IP_MEMORIA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	char* ipMemoria = config_get_string_value(configuracion,"IP_MEMORIA");
	configKernel.ipMemoria = malloc(strlen(ipMemoria)+1);
	strcpy(configKernel.ipMemoria, ipMemoria);
	logger(escribir_loguear, l_info,"IP_MEMORIA del archivo de configuracion del Kernel recuperado: %s",
			configKernel.ipMemoria);
	//Recupero el puerto de la memoria
	if(!config_has_property(configuracion,"PUERTO_MEMORIA")) {
		log_error(LOG_KERNEL,"No esta el PUERTO_MEMORIA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.puertoMemoria = config_get_int_value(configuracion, "PUERTO_MEMORIA");
	logger(escribir_loguear, l_info,"PUERTO_MEMORIA del archivo de configuracion del Kernel recuperado: %d",
		configKernel.puertoMemoria);
	//Recupero el quantum
	if(!config_has_property(configuracion,"QUANTUM")) {
		log_error(LOG_KERNEL,"No esta el QUANTUM en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.quantum = config_get_int_value(configuracion,"QUANTUM");
	logger(escribir_loguear, l_info,"Quantum del archivo de configuracion del Kernel recuperado: %d",
			configKernel.quantum);

	//Recupero el valor de multiprocesamiento
	if(!config_has_property(configuracion,"MULTIPROCESAMIENTO")) {
		log_error(LOG_KERNEL,"No esta el MULTIPROCESAMIENTO en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.multiprocesamiento = config_get_int_value(configuracion,"MULTIPROCESAMIENTO");
	logger(escribir_loguear, l_info,"Multiprocesamiento del archivo de configuracion del Kernel recuperado: %d",
			configKernel.multiprocesamiento);

	//Recupero el tiempo refresh metadata
	if(!config_has_property(configuracion,"REFRESH_METADATA")) {
		log_error(LOG_KERNEL,"No esta el REFRESH_METADATA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.refreshMetadata = config_get_int_value(configuracion,"REFRESH_METADATA");
	logger(escribir_loguear, l_info,"refresh metadata del archivo de configuracion del Kernel recuperado: %d",
			configKernel.refreshMetadata);

	//Recupero el tiempo de retardo del ciclo
		if(!config_has_property(configuracion,"RETARDO_CICLO")) {
			log_error(LOG_KERNEL,"No esta el RETARDO_CICLO en el archivo de configuracion");
			config_destroy(configuracion);
			log_error(LOG_KERNEL,"No se pudo levantar la configuracion del Kernel, abortando");
			return EXIT_FAILURE;
			}
		configKernel.retardoCiclo = config_get_int_value(configuracion,"RETARDO_CICLO");
		logger(escribir_loguear, l_info,"retardo_ciclo del archivo de configuracion del Kernel recuperado: %d",
				configKernel.retardoCiclo);

	config_destroy(configuracion);
	logger(escribir_loguear, l_info,"Configuracion del Kernel recuperada exitosamente");

	return EXIT_SUCCESS;
	}

int inicializarListas(){
	logger(escribir_loguear, l_info, "Inicializando listas del Kernel");
	listaNew = list_create();
	listaReady = list_create();
	listaExec = list_create();
	listaExit = list_create();
	listaMemConectadas = list_create(); // va a ser una lista de t_memo_del_pool_kernel
	return EXIT_SUCCESS;
}

int inicializarSemaforos(){
	logger(escribir_loguear, l_info, "Inicializando semaforos del kernel");
	pthread_mutex_init(&mutex_New, NULL);
	pthread_mutex_init(&mutex_Ready, NULL);
	pthread_mutex_init(&mutex_Exec, NULL);
	pthread_mutex_init(&mutex_Exit, NULL);
	pthread_mutex_init(&mutex_MemConectadas, NULL);
	pthread_mutex_init(&mutexDePausaDePlanificacion, NULL);
	return EXIT_SUCCESS;
}

int conectarse_con_memoria(int ip, int puerto){//ESTA DEBE ABRIR UN HILO Y CONECTARSE!
	logger(escribir_loguear, l_info, "Conectandose a la primera memoria en ip %s y puerto %i",
			ip, puerto);
	int socket_mem = conectarseA(ip, puerto);
	if(socket_mem < 0){
		logger(escribir_loguear, l_error, "No se puede conectar con la memoria de ip %i", ip);
		close(socket_mem);
	}
	enviar_handshake(socket_mem);
	tp_memo_del_pool_kernel entrada_tabla_memorias = calloc(1, sizeof(t_memo_del_pool_kernel));
	entrada_tabla_memorias->ip = ip;
	entrada_tabla_memorias->puerto = puerto;
	entrada_tabla_memorias->socket = socket_mem;
	list_add(listaMemConectadas, entrada_tabla_memorias);
	logger(escribir_loguear, l_info, "Se va a abrir un hilo para la memoria %i", socket_mem);
	int resultadoHiloMemo = pthread_create( &threadMemo, NULL, funcionHiloMemo, (void*)NULL);
	pthread_detach(threadMemo);
	return EXIT_SUCCESS;
}

void enviar_handshake(int socket){
	logger(escribir_loguear, l_info, "Se intenta enviar handshake a memoria");
	if (enviarHandshake(KERNEL, MEMORIA, socket) == 0) {
		logger(escribir_loguear, l_error, "No se pudo enviar handshake a memoria");
		close(socket);
	}
	logger(escribir_loguear, l_info, "Se realizo el handshake con la memoria en el socket %i", socket);
}

int funcionHiloMemo(){
	//Este hilo deberia esperar que la memoria devuelva algo
	return EXIT_SUCCESS;
}

t_operacion parsear(char * linea){

	t_operacion resultado_de_parsear;
	char* linea_auxiliar = string_duplicate(linea);
	string_trim(&linea_auxiliar);

	char** split = string_n_split(linea_auxiliar, 4, " ");

	char* tipo_de_operacion = split[0];
	char* parametros = split[1];

	if(linea == NULL || string_equals_ignore_case(linea, "")){
		//TODO ver que hacer aca
	}

	if(string_equals_ignore_case(tipo_de_operacion, "#")){
		logger(escribir_loguear, l_warning,"Es un comentario, sera ignorado");
		//no se hace nada...
	}

	if(tipo_de_operacion[0]=='#'){
		logger(escribir_loguear, l_warning,"Es un comentario, sera ignorado");
		//no se hace nada...
	}

	if(string_equals_ignore_case(tipo_de_operacion, "select")){
		resultado_de_parsear.tipo_de_operacion = _SELECT;
		resultado_de_parsear.parametros.select.nombre_tabla = split[1];
		resultado_de_parsear.parametros.select.key = split[2];
	} else if(string_equals_ignore_case(tipo_de_operacion, "insert")){
		resultado_de_parsear.tipo_de_operacion = _INSERT;
		resultado_de_parsear.parametros.insert.nombre_tabla = split[1];
		resultado_de_parsear.parametros.insert.key = split[2];
		resultado_de_parsear.parametros.insert.value = split[3];
	} else if(string_equals_ignore_case(tipo_de_operacion, "create")){
		resultado_de_parsear.tipo_de_operacion = _CREATE;
		resultado_de_parsear.parametros.create.nombre_tabla = split[1];
		resultado_de_parsear.parametros.create.tipo_consistencia = split[2];
		resultado_de_parsear.parametros.create.num_particiones = split[3];
		resultado_de_parsear.parametros.create.compaction_time = split[4];
	} else if(string_equals_ignore_case(tipo_de_operacion, "describe")){
		resultado_de_parsear.tipo_de_operacion = _DESCRIBE;
		resultado_de_parsear.parametros.describe.nombre_tabla = split[1];
	} else if(string_equals_ignore_case(tipo_de_operacion, "drop")){
		resultado_de_parsear.tipo_de_operacion = _DROP;
		resultado_de_parsear.parametros.drop.nombre_tabla = split[1];
	} else if(string_equals_ignore_case(tipo_de_operacion, "journal")){
		resultado_de_parsear.tipo_de_operacion = _JOURNAL;
	} else if(string_equals_ignore_case(tipo_de_operacion, "add")){
		resultado_de_parsear.tipo_de_operacion = ADD;
		resultado_de_parsear.parametros.add.memory = split[1];
		resultado_de_parsear.parametros.add.num_memoria = split[2];
		resultado_de_parsear.parametros.add.to = split[3];
		resultado_de_parsear.parametros.add.tipo_consistencia = split[4];
	} else if(string_equals_ignore_case(tipo_de_operacion, "run")){
		resultado_de_parsear.tipo_de_operacion = RUN;
		resultado_de_parsear.parametros.run.path = split[1];
	} else if(string_equals_ignore_case(tipo_de_operacion, "metrics")){
		resultado_de_parsear.tipo_de_operacion = METRICS;
	}

	free(linea_auxiliar);
	return resultado_de_parsear;
}


void conocer_pool_memorias(){
	logger(escribir_loguear, l_info, "Voy a consultar el pool de memorias");
	tp_memo_del_pool_kernel primera_memo = list_get(listaMemConectadas, 0);
	//TODO controlar si la primera sigue conectada, sino sacarla de la lista y pedirsela a la segunda.Quizas algun flag marcando que existe la q tomé?
	enviarCabecera(primera_memo->socket, POOL_REQUEST, sizeof(int));

}

void operacion_select(char* nombre_tabla, int key, tp_lql_pcb pcb){

}

void operacion_insert(char* nombre_tabla, int key, char* value, tp_lql_pcb pcb){

}

void operacion_create(char* nombre_tabla, int tipo_consistencia, int num_particiones, int compaction_time, tp_lql_pcb pcb){

}

void operacion_describe(char* nombre_tabla, tp_lql_pcb pcb){

}

void operacion_drop(char* nombre_tabla, tp_lql_pcb pcb){

}

void operacion_journal(){

}

void operacion_add(int num_memoria, int tipo_consistencia, tp_lql_pcb pcb){

}

void realizar_operacion(t_operacion resultado_del_parseado, tp_lql_pcb pcb){
	switch(resultado_del_parseado.tipo_de_operacion){
		case SELECT:
			operacion_select(resultado_del_parseado.parametros.select.nombre_tabla, resultado_del_parseado.parametros.select.key, pcb);
			break;
		case INSERT:
			operacion_insert(resultado_del_parseado.parametros.insert.nombre_tabla, resultado_del_parseado.parametros.insert.key,
					resultado_del_parseado.parametros.insert.value, pcb);
			break;
		case CREATE:
			operacion_create(resultado_del_parseado.parametros.create.nombre_tabla, resultado_del_parseado.parametros.create.tipo_consistencia,
					resultado_del_parseado.parametros.create.num_particiones, resultado_del_parseado.parametros.create.compaction_time, pcb);
			break;
		case DESCRIBE:
			operacion_describe(resultado_del_parseado.parametros.describe.nombre_tabla,pcb);
			break;
		case DROP:
			operacion_drop(resultado_del_parseado.parametros.drop.nombre_tabla,pcb);
			break;
		case JOURNAL:
			operacion_journal();
			break;
		case ADD:
			operacion_add(resultado_del_parseado.parametros.add.num_memoria, resultado_del_parseado.parametros.add.tipo_consistencia, pcb);
			break;
	}
}

int lanzarPlanificador(){

	return EXIT_SUCCESS;
}
