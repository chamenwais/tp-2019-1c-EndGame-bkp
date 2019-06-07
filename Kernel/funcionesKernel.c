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
	quantum = configKernel.quantum;

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
	listaTablasCreadas = list_create(); // se va a guardar las tablas a las q se les hace create t_entrada_tabla_creada
	listaEC = list_create();
	listaHC = list_create();
	listaSC = list_create();
	return EXIT_SUCCESS;
}

int inicializarSemaforos(){
	logger(escribir_loguear, l_info, "Inicializando semaforos del kernel");

	sem_init(&hay_request,0,0);
	sem_init(&NEW,0,0);

	pthread_mutex_init(&mutex_New, NULL);
	pthread_mutex_init(&mutex_Ready, NULL);
	pthread_mutex_init(&mutex_Exec, NULL);
	pthread_mutex_init(&mutex_Exit, NULL);
	pthread_mutex_init(&mutex_MemConectadas, NULL);
	pthread_mutex_init(&mutexDePausaDePlanificacion, NULL);
	pthread_mutex_init(&mutexPCP, NULL);
	return EXIT_SUCCESS;
}

int conectarse_con_memoria(int ip, int puerto){
	logger(escribir_loguear, l_info, "Conectandose a la primera memoria en ip %s y puerto %i",
			ip, puerto);
	int socket_mem = conectarseA(ip, puerto);
	if(socket_mem < 0){
		logger(escribir_loguear, l_error, "No se puede conectar con la memoria de ip %i", ip);
		close(socket_mem);
	}
	enviar_handshake(socket_mem);

	int tu_variable_de_numero_de_memoria = prot_recibir_int(socket_mem);

	tp_memo_del_pool_kernel entrada_tabla_memorias = calloc(1, sizeof(t_memo_del_pool_kernel));
	entrada_tabla_memorias->ip = ip;
	entrada_tabla_memorias->puerto = puerto;
	entrada_tabla_memorias->numero_memoria = tu_variable_de_numero_de_memoria;
	entrada_tabla_memorias->socket = socket_mem;
	list_add(listaMemConectadas, entrada_tabla_memorias);
	list_add(listaSC, entrada_tabla_memorias); //BORRAR LUEGO, ES PARA PROBAR
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

t_operacion parsear(char * linea){

	t_operacion resultado_de_parsear;
	char* linea_auxiliar = string_duplicate(linea);
	string_trim(&linea_auxiliar);

	char** split = string_n_split(linea_auxiliar, 5, " ");

	char* tipo_de_operacion = split[0];
	char* parametros = split[1];

	if(linea == NULL || string_equals_ignore_case(linea, "")){
		//TODO ver que hacer aca
	}

	if(string_equals_ignore_case(tipo_de_operacion, "select")){
		resultado_de_parsear.tipo_de_operacion = _SELECT;
		resultado_de_parsear.parametros.select.nombre_tabla = split[1];
		resultado_de_parsear.parametros.select.key = atoi(split[2]);
	} else if(string_equals_ignore_case(tipo_de_operacion, "insert")){
		resultado_de_parsear.tipo_de_operacion = _INSERT;
		resultado_de_parsear.parametros.insert.nombre_tabla = split[1];
		resultado_de_parsear.parametros.insert.key = atoi(split[2]);
		resultado_de_parsear.parametros.insert.value = obtener_value_a_insertar(linea_auxiliar); //si esta mal devuelve NULL
	} else if(string_equals_ignore_case(tipo_de_operacion, "create")){
		resultado_de_parsear.tipo_de_operacion = _CREATE;
		resultado_de_parsear.parametros.create.nombre_tabla = split[1];
		resultado_de_parsear.parametros.create.tipo_consistencia = split[2];
		resultado_de_parsear.parametros.create.num_particiones = atoi(split[3]);
		resultado_de_parsear.parametros.create.compaction_time = atoi(split[4]);
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
		resultado_de_parsear.parametros.add.memory = atoi(split[1]);//enum
		resultado_de_parsear.parametros.add.num_memoria = atoi(split[2]);
		resultado_de_parsear.parametros.add.to = atoi(split[3]);//enum
		resultado_de_parsear.parametros.add.tipo_consistencia = atoi(split[4]);
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

void remover_pcb_de_lista(t_list* lista, tp_lql_pcb pcb){
	int i = 0;
	void coincidePath(void* nodo){
			if(strcmp(((tp_lql_pcb) nodo)->path, pcb->path)==0){
				list_remove(lista, i);
			}
			i++;
		}
}

void operacion_select(char* nombre_tabla, uint16_t key, tp_lql_pcb pcb, int socket_memoria){

	//if(existeTabla(nombre_tabla)){

		logger(escribir_loguear, l_info, "Voy a realizar la operacion select");
		prot_enviar_select(nombre_tabla, key, socket_memoria);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		t_cabecera rta_pedido = recibirCabecera(socket_memoria);

		if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "La memoria realizo el select correctamente");
			tp_select_rta seleccion = prot_recibir_respuesta_select(rta_pedido.tamanio, socket_memoria);

			logger(escribir_loguear, l_info, "Esta es la informacion recibida:");
			logger(escribir_loguear, l_info, "Value: %s", seleccion->value);


			//Libero la estructura que recibi
			free(seleccion->value);
			free(seleccion);

		}

		if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
			logger(escribir_loguear, l_error, "No existe la tabla en el FS");
		}

	//}else{//termino el script
	/*	logger(escribir_loguear, l_error, "No existe la tabla %s\n", nombre_tabla);
		pthread_mutex_lock(&mutex_Exec);
		remover_pcb_de_lista(listaExec, pcb);
		pthread_mutex_unlock(&mutex_Exec);
		pthread_mutex_lock(&mutex_Exit);
		list_add(listaExit, pcb);
		pthread_mutex_unlock(&mutex_Exit);
		logger(escribir_loguear, l_info, "El pcb %s fue terminado\n", pcb->path);
	}*/

}

void operacion_insert(char* nombre_tabla, int key, char* value, tp_lql_pcb pcb, int socket_memoria){

	//if(existeTabla(nombre_tabla)){
		logger(escribir_loguear, l_info, "Voy a realizar la operacion insert");

		long timestamp;
		timestamp=(unsigned)time(NULL);
		logger(escribir_loguear, l_debug,"El timestamp fue '%d'",timestamp);

		prot_enviar_insert(nombre_tabla, key, value, timestamp, socket_memoria);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		enum MENSAJES insercion = prot_recibir_respuesta_insert(socket_memoria);

		if(insercion == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "La memoria realizo el insert correctamente");
		}
	//}else{//terminar script
	/*	logger(escribir_loguear, l_error, "No existe la tabla %s\n", nombre_tabla);
		pthread_mutex_lock(&mutex_Exec);
		remover_pcb_de_lista(listaExec, pcb);
		pthread_mutex_unlock(&mutex_Exec);
		pthread_mutex_lock(&mutex_Exit);
		list_add(listaExit, pcb);
		pthread_mutex_unlock(&mutex_Exit);
		logger(escribir_loguear, l_info, "El pcb %s fue terminado\n", pcb->path);
*/
	//}

}

void operacion_create(char* nombre_tabla, char* tipo_consistencia, int num_particiones, int compaction_time, tp_lql_pcb pcb, int socket_memoria){

	//if(!existeTabla(nombre_tabla)){
		logger(escribir_loguear, l_info, "Se le solicita a la memoria crear la tabla: %s", nombre_tabla);
		prot_enviar_create(nombre_tabla, tipo_consistencia, num_particiones, compaction_time, socket_memoria);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		enum MENSAJES rta_creacion = prot_recibir_respuesta_create(socket_memoria);

		if(rta_creacion == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "La memoria realizo el create correctamente");
		}
		if(rta_creacion == TABLA_YA_EXISTIA){
			logger(escribir_loguear, l_info, "Ya existe la tabla que queres crear");
		}

	//}else{//terminar script
/*
		logger(escribir_loguear, l_error, "No existe la tabla %s\n", nombre_tabla);
		pthread_mutex_lock(&mutex_Exec);
		remover_pcb_de_lista(listaExec, pcb);
		pthread_mutex_unlock(&mutex_Exec);
		pthread_mutex_lock(&mutex_Exit);
		list_add(listaExit, pcb);
		pthread_mutex_unlock(&mutex_Exit);
		logger(escribir_loguear, l_info, "El pcb %s fue terminado\n", pcb->path);
*/
	//}
}

void operacion_describe(char* nombre_tabla, tp_lql_pcb pcb, int socket_memoria){
	if(nombre_tabla != NULL){
		logger(escribir_loguear, l_info, "Se le solicita a la memoria el describe de la tabla: %s", nombre_tabla);
		prot_enviar_describe(nombre_tabla, socket_memoria);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		//Recibo rta
		t_cabecera rta_pedido = recibirCabecera(socket_memoria);

		if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
			logger(escribir_loguear, l_debug, "Informacion recibida correctamente");
			tp_describe_rta info_tabla = prot_recibir_respuesta_describe(rta_pedido.tamanio, socket_memoria);

			logger(escribir_loguear, l_info, "Liss ha enviado la sgte informacion:");
			logger(escribir_loguear, l_info, "El nombre de la tabla es: %s", nombre_tabla);
			logger(escribir_loguear, l_info, "La consistencia es: %s", info_tabla->consistencia);
			logger(escribir_loguear, l_info, "El numero de particiones es: %d", info_tabla->particiones);
			logger(escribir_loguear, l_info, "El tiempo de compactacion es: %d\n", (*(t_describe_rta*)info_tabla).tiempoDeCompactacion);

			free(info_tabla->consistencia);
			free(info_tabla);
		}

		if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
			logger(escribir_loguear, l_error, "No existe la tabla");
		}

		/*
		 * HACER LO QUE TENGAS QUE HACER CON EL PCB
		 */
	}

	if(nombre_tabla == NULL){
		//PIDIERON UN DESCRIBE ALL
		prot_enviar_describeAll(socket_memoria);

		//Recibo rta
		t_cabecera rta_pedido = recibirCabecera(socket_memoria);

		if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
			tp_describeAll_rta info_de_las_tablas = prot_recibir_respuesta_describeAll(rta_pedido.tamanio, socket_memoria);

			/*
			 * HACER LO QUE TENGAS QUE HACER CON EL PCB
			 * EN INFO_DE_LAS_TABLAS TENES TODA LA INFORMACION QUE LLEGA DEL FILESYSTEM
			 */

			//Libero la lista
			prot_free_tp_describeAll_rta(info_de_las_tablas);
		}

		if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
			logger(escribir_loguear, l_error, "No hay tablas en el FS");
		}

	}
}

void operacion_drop(char* nombre_tabla, tp_lql_pcb pcb, int socket_memoria){
	logger(escribir_loguear, l_info, "Voy a realizar la operacion drop");

	prot_enviar_drop(nombre_tabla, socket_memoria);

	logger(escribir_loguear, l_info, "Espero la rta de memoria...");
	enum MENSAJES rta_drop = prot_recibir_respuesta_drop(socket_memoria);

	if(rta_drop == REQUEST_SUCCESS){
		logger(escribir_loguear, l_info, "La memoria realizo el drop de la tabla pedida correctamente");
	}

	if(rta_drop == TABLA_NO_EXISTIA){
		logger(escribir_loguear, l_error, "No existe la tabla que queres borrar");
	}

	/*
	 * HACER LO QUE TENGAS QUE HACER CON EL PCB
	 */

}

void operacion_journal(){

}


void realizar_operacion(t_operacion resultado_del_parseado, tp_lql_pcb pcb, int socket_memoria){
	switch(resultado_del_parseado.tipo_de_operacion){
		case SELECT:
			operacion_select(resultado_del_parseado.parametros.select.nombre_tabla, resultado_del_parseado.parametros.select.key, pcb, socket_memoria);
			break;
		case INSERT:
			operacion_insert(resultado_del_parseado.parametros.insert.nombre_tabla, resultado_del_parseado.parametros.insert.key,
					resultado_del_parseado.parametros.insert.value, pcb, socket_memoria);
			break;
		case CREATE:
			operacion_create(resultado_del_parseado.parametros.create.nombre_tabla, resultado_del_parseado.parametros.create.tipo_consistencia,
					resultado_del_parseado.parametros.create.num_particiones, resultado_del_parseado.parametros.create.compaction_time, pcb, socket_memoria);
			break;
		case DESCRIBE:
			operacion_describe(resultado_del_parseado.parametros.describe.nombre_tabla, pcb, socket_memoria);
			break;
		case DROP:
			operacion_drop(resultado_del_parseado.parametros.drop.nombre_tabla,pcb, socket_memoria);
			break;
		case JOURNAL:
			operacion_journal();
			break;
	}
}

int lanzarPlanificador(){
	int resultadoDeCrearHilo = pthread_create(&threadPlanif, NULL, funcionHiloPLP, NULL);
	pthread_detach(threadPlanif);
		if(resultadoDeCrearHilo){
			logger(escribir_loguear, l_error,"Error: no se pudo crear el hilo para la planificacion a largo plazo");
			exit(EXIT_FAILURE);
			}
		else{
			logger(escribir_loguear, l_info ,"Se creo el hilo para la planificacion a largo plazo");
			return EXIT_SUCCESS;
			}

	return EXIT_SUCCESS;
}

void* funcionHiloPLP(){
	char *ret="Cerrando hilo PLP";
	while(1){
		logger(escribir_loguear, l_info, "El PLP esta bloqueado\n"); /// para salto de linea es barra invertida
		//pthread_mutex_lock(&mutexDePausaDePlanificacion);
		logger(escribir_loguear, l_info, "Se desbloqueo el PLP\n");
		tp_lql_pcb nuevo_pcb;

		sem_wait(&NEW);
		pthread_mutex_lock(&mutex_New);
		nuevo_pcb = list_remove(listaNew, 0); //remueve el primer elemento y lo retorna
		pthread_mutex_unlock(&mutex_New);
		pthread_mutex_lock(&mutex_Ready);
		list_add(listaReady, nuevo_pcb); //pasa el nuevo pcb a Ready
		pthread_mutex_unlock(&mutex_Ready);
		//logger(escribir_loguear, l_info, "El LQL %s pasa a Ready\n", nuevo_pcb->path);
		pthread_mutex_unlock(&mutexPCP);// paso un LQL a ready, habilito PCP
	}

	pthread_exit(ret);
	return EXIT_SUCCESS;
}

int lanzarPCP(){
	int resultadoDeCrearHilo = pthread_create(&threadPCP, NULL, funcionHiloPCP, NULL);
	pthread_detach(threadPCP);
			if(resultadoDeCrearHilo){
				logger(escribir_loguear, l_error,"Error: no se pudo crear el hilo para la planificacion a corto plazo");
				exit(EXIT_FAILURE);
				}
			else{
				logger(escribir_loguear, l_info ,"Se creo el hilo para la planificacion a corto plazo");
				return EXIT_SUCCESS;
				}

		return EXIT_SUCCESS;
}

void* funcionHiloPCP(){
	char *ret="Cerrando hilo PCP";
	while(1){
		pthread_mutex_lock(&mutexPCP);
		if(list_size(listaReady) > 0 && list_size(listaExec) < configKernel.multiprocesamiento){
			logger(escribir_loguear, l_info, "Se activa el PCP");
			tp_lql_pcb pcb_a_planificar;
			pthread_mutex_lock(&mutex_Ready);
			pcb_a_planificar = list_remove(listaReady, 0); //devuelve el primer elemento de la lista de Ready
			pthread_mutex_unlock(&mutex_Ready);
			pthread_mutex_lock(&mutex_Exec);
			list_add(listaExec, pcb_a_planificar);//Agrega el pcb a la lista de ejecutando
			pthread_mutex_unlock(&mutex_Exec);

			sem_wait(&hay_request);
			lanzarHiloRequest(pcb_a_planificar);

		}
	}

	pthread_exit(ret);
	return EXIT_SUCCESS;
}

int lanzarHiloRequest(tp_lql_pcb pcb){
	int resultadoDeCrearHilo = pthread_create(&threadRequest, NULL, funcionHiloRequest, pcb);
	pthread_detach(threadRequest);

	if(resultadoDeCrearHilo){
		logger(escribir_loguear, l_error,"Error: no se pudo crear el hilo para la planificacion del LQL %s\n", pcb->path);
		exit(EXIT_FAILURE);
		}
	else{
		logger(escribir_loguear, l_info ,"Se creo el hilo para la planificacion del LQL %s\n", pcb->path);
		return EXIT_SUCCESS;
		}

return EXIT_SUCCESS;

}

void* funcionHiloRequest(void* pcb){
	char *ret="Cerrando hilo Request";
	int i;
	char* linea_a_ejecutar;
	for (i = 0; i < quantum; ++i) {
		logger(escribir_loguear, l_info, "Se va a enviar la linea %i", i);
		linea_a_ejecutar = list_remove((*(tp_lql_pcb) pcb).lista, 0); //lo saca de la lista y lo devuelve, de esta manera controlamos la prox linea a ejecutar

		//Parsear la linea
		t_operacion rdo_del_parseado = parsear(linea_a_ejecutar);

		//Elegir memoria de acuerdo a la tabla
		char* tabla = obtenerTabla(rdo_del_parseado);
		tp_memo_del_pool_kernel memoria = decidir_memoria_a_utilizar(tabla);

		//TODO controlar estado de la memoria. FULL: forzar journal. JOURNALING: esperar.

		realizar_operacion(rdo_del_parseado, pcb, memoria->socket);
	}

	if(!pcbEstaEnLista(listaExit, pcb)){
		pthread_mutex_lock(&mutex_Exec);
		remover_pcb_de_lista(listaExec, pcb);
		pthread_mutex_unlock(&mutex_Exec);
		pthread_mutex_lock(&mutex_Ready);
		list_add(listaReady, pcb);
		pthread_mutex_unlock(&mutex_Ready);
		logger(escribir_loguear, l_info, "El PCB %s vuelve a READY\n", ((tp_lql_pcb) pcb)->path);
		pthread_exit(ret);
		return EXIT_SUCCESS;
	}

	pthread_exit(ret);
	return EXIT_SUCCESS;
}

char* obtenerTabla(t_operacion resultado_del_parseado){
	char* tabla = string_new();
	switch(resultado_del_parseado.tipo_de_operacion){
		case _SELECT:
			strcpy(tabla, resultado_del_parseado.parametros.select.nombre_tabla);
			break;
		case _INSERT:
			strcpy(tabla, resultado_del_parseado.parametros.insert.nombre_tabla);
			break;
		case _CREATE:
			strcpy(tabla, resultado_del_parseado.parametros.create.nombre_tabla);
			break;
		case _DESCRIBE:
			strcpy(tabla, resultado_del_parseado.parametros.describe.nombre_tabla);
			break;
		case _DROP:
			strcpy(tabla, resultado_del_parseado.parametros.drop.nombre_tabla);
			break;
	}
	return tabla;
}

bool existeTabla(char* tabla){

	bool coincideNombre(void* nodo){
		if(strcmp(((tp_entrada_tabla_creada) nodo)->nombre_tabla, tabla)==0){
			return true;
		}
		return false;
	}

	return list_any_satisfy(listaTablasCreadas, coincideNombre);
}

bool pcbEstaEnLista(t_list* lista, tp_lql_pcb pcb){

	bool coincideElPath(void* nodo){
		if(strcmp(((tp_lql_pcb) nodo)->path, pcb->path)==0){
			return true;
		}
		return false;
	}
	return list_any_satisfy(lista, coincideElPath);
}

tp_memo_del_pool_kernel decidir_memoria_a_utilizar(char* nombre_tabla){
	tp_memo_del_pool_kernel memoria;
	//buscar tabla en listaTablasCreadas y obtener el criterio
	logger(escribir_loguear, l_info, "Eligiendo memoria para la tabla %s\n", nombre_tabla);
	tp_entrada_tabla_creada entrada = list_find(listaTablasCreadas, existeTabla);
	memoria = list_get(listaSC, 0);

/*
	//buscar las memorias que tengan ese criterio asignado y elegir
	if(entrada != NULL){
		int criterio = entrada->criterio;
		switch (criterio) {
			case SC:
				memoria = list_get(listaSC, 0);
				logger(escribir_loguear, l_info, "Se eligio la memoria %i para el criterio SC", memoria->numero_memoria);
				break;
			case HC:
				logger(escribir_loguear, l_info, "Facundito todavia no hizo nada para el HC"); //TODO
				break;
			case EC:
				logger(escribir_loguear, l_info, "Facundito todavia no hizo nada para el EC"); //TODO
				break;
		}
	}  FACU, EL CRITERIO ES UN CHAR* !!!!! SINO LAS FUNCIONES DE COM NO FUNCAN
*/
	return memoria;
}

tp_memo_del_pool_kernel buscar_memorias_segun_numero(t_list* lista, int numero){

	bool coincideNumero(void* nodo){
			if(((tp_memo_del_pool_kernel) nodo)->numero_memoria == numero){
				return true;
			}
			return false;
		}

	tp_memo_del_pool_kernel memo = list_get(lista, coincideNumero);
	return memo;

}
