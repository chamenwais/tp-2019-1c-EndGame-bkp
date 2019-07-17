/*
 * funcionesKernel.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#include "funcionesKernel.h"



void inicializarLogKernel(){
	LOG_KERNEL = log_create("Kernel.log","Kernel",false,LOG_LEVEL_DEBUG);
	logger(escribir_loguear, l_debug,"Log del Kernel iniciado");
	return;
}

char* reconstruir_path_archivo(char* directorio, char* nombre_archivo) {
	char* path = string_new();
	string_append(&path, directorio);
	string_append(&path, nombre_archivo);
	return path;
}


void separar_path_pasado_por_parametro(char ** nombre_archivo, char ** directorio, char ** parametros){
	char** path_a_separar = string_n_split(parametros[1], 20, "/");

	int final = 0,cont = 0;
	while(path_a_separar[final]){
		final++;
		if(path_a_separar[final]!=NULL){
			cont++;
		}
	}
	string_append(nombre_archivo, path_a_separar[cont]);
	logger(escribir_loguear, l_debug,"El nombre del archivo es %s", *nombre_archivo);

	string_append(directorio,"/");

	int cont2 = 0;
	while(cont2 <= (cont-1)){
		string_append_with_format(directorio, "%s/", path_a_separar[cont2]);
		cont2++;
	}

	logger(escribir_loguear, l_debug,"EL directorio es %s", *directorio);

	int j = 0;
	while(path_a_separar[j]){
		free(path_a_separar[j]);
		j++;
	}

	free(path_a_separar);
}

void iniciar_config(int cantidad_parametros, char ** parametros) {
	char * directorio=string_new();
	char * nombre_archivo=string_new();
	if (cantidad_parametros>1){
		separar_path_pasado_por_parametro(&nombre_archivo, &directorio, parametros);
		k_config = config_create(parametros[1]);
	} else {
		free(directorio);
		free(nombre_archivo);
		directorio=string_duplicate(DIRECTORIO_CONFIG_DEFAULT);
		nombre_archivo=string_duplicate(NOMBRE_ARCH_CONFIG_DEFAULT);
		char* path = reconstruir_path_archivo(directorio, nombre_archivo);
		k_config = config_create(path);
		free(path);
	}
	validar_apertura_archivo_configuracion();
	iniciar_escucha_cambios_conf(directorio, nombre_archivo);
}

void validar_apertura_archivo_configuracion() {
	if (k_config == NULL) {
		logger(escribir_loguear,l_error, "No encontré el archivo de configuración");
		log_destroy(LOG_KERNEL);
		exit(EXIT_FAILURE);
	}
}

void iniciar_escucha_cambios_conf(char * directorio, char * nombre_archivo){
	pthread_t hilo_cambios_conf;
	t_path_archivo_conf * str_ruta_archivo_conf=malloc(sizeof(t_path_archivo_conf));
	str_ruta_archivo_conf->directorio=directorio;
	str_ruta_archivo_conf->nombre_archivo=nombre_archivo;
	int resultado_de_hacer_el_hilo = pthread_create (&hilo_cambios_conf, NULL
			, escuchar_cambios_conf, str_ruta_archivo_conf);
	if(resultado_de_hacer_el_hilo!=0){
		if(resultado_de_hacer_el_hilo!=0){
			logger(escribir_loguear,l_error
					,"Error al crear el hilo de escucha de cambios de conf, levantá la memoria de nuevo");
			free(str_ruta_archivo_conf);
			terminar_programa(EXIT_FAILURE);
		}
	}
	pthread_detach(hilo_cambios_conf);
}

void *escuchar_cambios_conf(void * estructura_path){

	pthread_mutex_lock(&M_RUTA_ARCHIVO_CONF);
	ruta_archivo_conf=estructura_path;
	pthread_mutex_unlock(&M_RUTA_ARCHIVO_CONF);

	char buffer[BUF_LEN];

	pthread_mutex_lock(&M_CONF_FD);
	conf_fd = inotify_init();
	pthread_mutex_unlock(&M_CONF_FD);

	if (conf_fd < 0) {
		logger(escribir_loguear,l_error, "No se van a poder escuchar cambios en el archivo de configuración");
		free(ruta_archivo_conf);
		return EXIT_SUCCESS;
	}
	pthread_mutex_lock(&M_WATCH_DESCRIPTOR);
	watch_descriptor = inotify_add_watch(conf_fd, ruta_archivo_conf->directorio, IN_MODIFY);
	pthread_mutex_unlock(&M_WATCH_DESCRIPTOR);

	int length = read(conf_fd, buffer, BUF_LEN);
	if (length < 0) {
		logger(escribir_loguear,l_error, "No se van a poder escuchar cambios en el archivo de configuración");
		free(ruta_archivo_conf);
		return EXIT_SUCCESS;
	}

	pthread_mutex_lock(&M_PATH_ARCHIVO_CONFIGURACION);
	path_archivo_configuracion=reconstruir_path_archivo(ruta_archivo_conf->directorio, ruta_archivo_conf->nombre_archivo);
	pthread_mutex_unlock(&M_PATH_ARCHIVO_CONFIGURACION);

	while (length>0) {
		struct inotify_event *event = (struct inotify_event *) &buffer[0];
		if (event->len>0 && (event->mask & IN_MODIFY)
				&& string_equals_ignore_case(event->name, ruta_archivo_conf->nombre_archivo)) {

			reloadConfig();
			length = read(conf_fd, buffer, BUF_LEN);
			if (conf_fd < 0) {
				logger(escribir_loguear,l_error, "No se van a escuchar más cambios en el archivo de configuración");
				return EXIT_SUCCESS;
			}
		}
	}

	return EXIT_SUCCESS;
}

int levantarConfiguracionInicialDelKernel(){

	if(k_config!=NULL){
		logger(escribir_loguear, l_info,"El archivo de configuracion existe");
	}else{
		logger(escribir_loguear, l_error,"No existe el archivo de configuracion en: %s",path_archivo_configuracion);
		logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	logger(escribir_loguear, l_info,"Abriendo el archivo de configuracion del Kernel");

	//Recupero el IP de la Memoria
	if(!config_has_property(k_config,"IP_MEMORIA")) {
		logger(escribir_loguear, l_error,"No esta el IP_MEMORIA en el archivo de configuracion");
		config_destroy(k_config);
		logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	char* ipMemoria = config_get_string_value(k_config,"IP_MEMORIA");
	configKernel.ipMemoria = malloc(strlen(ipMemoria)+1);
	strcpy(configKernel.ipMemoria, ipMemoria);
	logger(escribir_loguear, l_info,"IP_MEMORIA del archivo de configuracion del Kernel recuperado: %s",
			configKernel.ipMemoria);
	//Recupero el puerto de la memoria
	if(!config_has_property(k_config,"PUERTO_MEMORIA")) {
		logger(escribir_loguear, l_error,"No esta el PUERTO_MEMORIA en el archivo de configuracion");
		config_destroy(k_config);
		logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	char * puertoMemoria = config_get_string_value(k_config, "PUERTO_MEMORIA");
	configKernel.puertoMemoria = malloc(strlen(puertoMemoria)+1);
	strcpy(configKernel.puertoMemoria, puertoMemoria);
	logger(escribir_loguear, l_info,"PUERTO_MEMORIA del archivo de configuracion del Kernel recuperado: %s",
		configKernel.puertoMemoria);
	//Recupero el quantum
	if(!config_has_property(k_config,"QUANTUM")) {
		logger(escribir_loguear, l_error,"No esta el QUANTUM en el archivo de configuracion");
		config_destroy(k_config);
		logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.quantum = config_get_int_value(k_config,"QUANTUM");
	logger(escribir_loguear, l_info,"Quantum del archivo de configuracion del Kernel recuperado: %d",
			configKernel.quantum);
	quantum = configKernel.quantum;

	//Recupero el valor de multiprocesamiento
	if(!config_has_property(k_config,"MULTIPROCESAMIENTO")) {
		logger(escribir_loguear, l_error,"No esta el MULTIPROCESAMIENTO en el archivo de configuracion");
		config_destroy(k_config);
		logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.multiprocesamiento = config_get_int_value(k_config,"MULTIPROCESAMIENTO");
	logger(escribir_loguear, l_info,"Multiprocesamiento del archivo de configuracion del Kernel recuperado: %d",
			configKernel.multiprocesamiento);

	//Recupero el tiempo refresh metadata
	if(!config_has_property(k_config,"REFRESH_METADATA")) {
		logger(escribir_loguear, l_error,"No esta el REFRESH_METADATA en el archivo de configuracion");
		config_destroy(k_config);
		logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
		return EXIT_FAILURE;
		}
	configKernel.refreshMetadata = config_get_int_value(k_config,"REFRESH_METADATA");
	logger(escribir_loguear, l_info,"refresh metadata del archivo de configuracion del Kernel recuperado: %d",
			configKernel.refreshMetadata);

	//Recupero el tiempo de retardo del ciclo
		if(!config_has_property(k_config,"RETARDO_CICLO")) {
			logger(escribir_loguear, l_error,"No esta el RETARDO_CICLO en el archivo de configuracion");
			config_destroy(k_config);
			logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
			return EXIT_FAILURE;
			}
		configKernel.retardoCiclo = config_get_int_value(k_config,"RETARDO_CICLO");
		logger(escribir_loguear, l_info,"retardo_ciclo del archivo de configuracion del Kernel recuperado: %d",
				configKernel.retardoCiclo);
		retardo = configKernel.retardoCiclo;

	//Recupero el tiempo de gossip
		if(!config_has_property(k_config,"GOSSIP_TIME")) {
			logger(escribir_loguear, l_error,"No esta el GOSSIP_TIME en el archivo de configuracion");
			config_destroy(k_config);
			logger(escribir_loguear, l_error,"No se pudo levantar la configuracion del Kernel, abortando");
			return EXIT_FAILURE;
			}
		configKernel.gossip_time = config_get_int_value(k_config,"GOSSIP_TIME");
		logger(escribir_loguear, l_info,"gossip_time del archivo de configuracion del Kernel recuperado: %d",
				configKernel.gossip_time);

	config_destroy(k_config);
	logger(escribir_loguear, l_info,"Configuracion del Kernel recuperada exitosamente");

	return EXIT_SUCCESS;
	}

int actualizarQuantum(int nuevoQuantum){
	pthread_mutex_lock(&mutexVariableQuantum);
	configKernel.quantum = nuevoQuantum;
	pthread_mutex_unlock(&mutexVariableQuantum);
	return EXIT_SUCCESS;
	}

int actualizarRefresh(int refresh){
	pthread_mutex_lock(&mutexVariableRefresh);
	configKernel.refreshMetadata = refresh;
	pthread_mutex_unlock(&mutexVariableRefresh);
	return EXIT_SUCCESS;
	}

int actualizarRetardo(int retardoNuevo){
	pthread_mutex_lock(&mutexVariableRetardo);
	configKernel.retardoCiclo = retardoNuevo;
	pthread_mutex_unlock(&mutexVariableRetardo);
	return EXIT_SUCCESS;
	}

int actualizarGossip(int gossip){
	pthread_mutex_lock(&mutexVariableGossip);
	configKernel.gossip_time = gossip;
	pthread_mutex_unlock(&mutexVariableGossip);
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

	sem_init(&NEW,0,0);
	sem_init(&READY,0,0);

	pthread_mutex_init(&mutex_New, NULL);
	pthread_mutex_init(&mutex_Ready, NULL);
	pthread_mutex_init(&mutex_Exec, NULL);
	pthread_mutex_init(&mutex_Exit, NULL);
	pthread_mutex_init(&mutex_MemConectadas, NULL);
	pthread_mutex_init(&mutex_SC, NULL);
	pthread_mutex_init(&mutex_EC, NULL);
	pthread_mutex_init(&mutex_HC, NULL);
	pthread_mutex_init(&mutex_tablas, NULL);
	pthread_mutex_init(&mutexVariableQuantum, NULL);
	pthread_mutex_init(&mutexVariableRefresh, NULL);
	pthread_mutex_init(&mutexVariableRetardo, NULL);
	pthread_mutex_init(&mutexVariableGossip, NULL);
	pthread_mutex_init(&M_RUTA_ARCHIVO_CONF, NULL);
	pthread_mutex_init(&M_CONF_FD, NULL);
	pthread_mutex_init(&M_WATCH_DESCRIPTOR, NULL);
	pthread_mutex_init(&M_PATH_ARCHIVO_CONFIGURACION, NULL);
	return EXIT_SUCCESS;
}

int conectarse_con_primera_memoria(char* ip, char * puerto){
	logger(escribir_loguear, l_info, "Conectandose a la memoria en ip %s y puerto %s",
			ip, puerto);
	int socket_mem = abrir_socket_memoria(ip, puerto);
	int numero_de_memoria = prot_recibir_int(socket_mem);

	tp_memo_del_pool_kernel entrada_tabla_memorias = calloc(1, sizeof(t_memo_del_pool_kernel));
	entrada_tabla_memorias->ip = ip;
	entrada_tabla_memorias->puerto = puerto;
	entrada_tabla_memorias->numero_memoria = numero_de_memoria;
	entrada_tabla_memorias->socket = socket_mem;

	list_add(listaMemConectadas, entrada_tabla_memorias);

	socket_primera_memoria = socket_mem;

	describeAll(socket_mem);

	return EXIT_SUCCESS;
}

int abrir_socket_memoria(char* ip, char* puerto) {
	int socket_mem = conectarseA(ip, atoi(puerto));
	if (socket_mem < 0) {
		logger(escribir_loguear, l_error, "No se puede conectar con la memoria de ip %s y puerto %s", ip, puerto);
		terminar_programa(EXIT_SUCCESS);
	}
	enviar_handshake(socket_mem);
	return socket_mem;
}

int conectarse_con_memoria(char* ip, char * puerto){
	logger(escribir_loguear, l_info, "Conectandose a la memoria en ip %s y puerto %s",
			ip, puerto);
	int socket_mem = abrir_socket_memoria(ip, puerto);
	int numero_de_memoria = prot_recibir_int(socket_mem);
	logger(escribir_loguear, l_info, "Me conecte a la memoria numero %d", numero_de_memoria);


	tp_memo_del_pool_kernel entrada_tabla_memorias = calloc(1, sizeof(t_memo_del_pool_kernel));
	entrada_tabla_memorias->ip = ip;
	entrada_tabla_memorias->puerto = puerto;
	entrada_tabla_memorias->numero_memoria = numero_de_memoria;
	entrada_tabla_memorias->socket = socket_mem;

	pthread_mutex_lock(&mutex_MemConectadas);
	list_add(listaMemConectadas, entrada_tabla_memorias);
	pthread_mutex_unlock(&mutex_MemConectadas);

	describeAll(socket_mem);

	return EXIT_SUCCESS;
}

int conectar_con_memoria(char* ip, char * puerto){
	logger(escribir_loguear, l_info, "Conectando request a la memoria en ip %s y puerto %s",
			ip, puerto);
	int sock = 0;
	int socket_mem = conectarseA(ip, atoi(puerto));
	if(socket_mem < 0){
		logger(escribir_loguear, l_error, "No se puede conectar con la memoria de ip %s", ip);
	}
	enviar_handshake(socket_mem);

	int numero_de_memoria = prot_recibir_int(socket_mem);

	tp_memo_del_pool_kernel mem = buscar_memorias_segun_numero(listaMemConectadas, numero_de_memoria);

	if(mem->numero_memoria == numero_de_memoria){
		return socket_mem;
	}

	return sock;
}

void enviar_handshake(int socket){
	logger(escribir_loguear, l_debug, "Se intenta enviar handshake a memoria");
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
		char * tabla=split[1];
		if(tabla!=NULL){
			size_t tamanio_nombre_tabla = strlen(tabla);
			tabla = realloc(tabla, tamanio_nombre_tabla + 1);
			char barra_cero='\0';
			memcpy(tabla+ tamanio_nombre_tabla, &barra_cero,1);
			resultado_de_parsear.parametros.describe.nombre_tabla = tabla;
		}else{
			resultado_de_parsear.parametros.describe.nombre_tabla = NULL;
		}
	} else if(string_equals_ignore_case(tipo_de_operacion, "drop")){
		resultado_de_parsear.tipo_de_operacion = _DROP;
		//Le agrego un \0 al final porque parece que no lo pone en el describe

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


void remover_pcb_de_lista(t_list* lista, tp_lql_pcb pcb){
	bool coincidePath(void* nodo){
		return string_equals_ignore_case(((tp_lql_pcb) nodo)->path, pcb->path);
	}
	list_remove_by_condition(lista,coincidePath);
}

void operacion_select(char* nombre_tabla, uint16_t key, tp_lql_pcb pcb, int socket_memoria){

	if(existeTabla(nombre_tabla)){

		logger(escribir_loguear, l_info, "Voy a realizar la operacion select");
		prot_enviar_select(nombre_tabla, key, socket_memoria);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		t_cabecera rta_pedido = recibirCabecera(socket_memoria);

		if(rta_pedido.tipoDeMensaje == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "La memoria realizo el select correctamente");
			tp_select_rta seleccion = prot_recibir_respuesta_select(rta_pedido.tamanio, socket_memoria);

			logger(escribir_loguear, l_info, "Esta es la informacion recibida:");
			logger(escribir_loguear, l_info, "\nValue: %s\n", seleccion->value);


			//Libero la estructura que recibi
			free(seleccion->value);
			free(seleccion);

		}

		if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
			logger(escribir_loguear, l_error, "No existe la tabla en el FS");
		}else if(rta_pedido.tipoDeMensaje == NO_HAY_MAS_MARCOS_EN_LA_MEMORIA){
			logger(escribir_loguear, l_info, "La memoria esta llena, procedo a pedir un Journal");
			operacion_journal(socket_memoria);
			logger(escribir_loguear, l_info, "Luego del JOURNAL se vuelve a enviar el SELECT");
			operacion_select(nombre_tabla, key, pcb, socket_memoria);
		}

	}else{//termino el script
		logger(escribir_loguear, l_error, "No existe la tabla %s\n", nombre_tabla);
		pthread_mutex_lock(&mutex_Exec);
		remover_pcb_de_lista(listaExec, pcb);
		pthread_mutex_unlock(&mutex_Exec);
		pthread_mutex_lock(&mutex_Exit);
		list_add(listaExit, pcb);
		pthread_mutex_unlock(&mutex_Exit);
		logger(escribir_loguear, l_info, "El pcb %s fue terminado\n", pcb->path);
	}

}

double obtenerTimestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec)*1000+((unsigned long long)tv.tv_usec)/1000);
	double a = result;
	return a;
}

void operacion_insert(char* nombre_tabla, int key, char* value, tp_lql_pcb pcb, int socket_memoria){

	if(existeTabla(nombre_tabla)){
		logger(escribir_loguear, l_info, "Voy a realizar la operacion insert");

		double timestamp;
		timestamp=obtenerTimestamp();
		logger(escribir_loguear, l_info,"El timestamp fue '%.0f'",timestamp);

		prot_enviar_insert(nombre_tabla, key, value, timestamp, socket_memoria);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		enum MENSAJES insercion = prot_recibir_respuesta_insert(socket_memoria);

		if(insercion == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "La memoria realizo el insert correctamente");
		}else if(insercion == NO_HAY_MAS_MARCOS_EN_LA_MEMORIA){
			logger(escribir_loguear, l_info, "La memoria esta llena, procedo a pedir un Journal");
			operacion_journal(socket_memoria);
			logger(escribir_loguear, l_info, "Luego del JOURNAL se vuelve a enviar el INSERT");
			operacion_insert(nombre_tabla, key, value, pcb, socket_memoria);
		}
	}else{//terminar script
		logger(escribir_loguear, l_error, "No existe la tabla %s\n", nombre_tabla);
		pthread_mutex_lock(&mutex_Exec);
		remover_pcb_de_lista(listaExec, pcb);
		pthread_mutex_unlock(&mutex_Exec);
		pthread_mutex_lock(&mutex_Exit);
		list_add(listaExit, pcb);
		pthread_mutex_unlock(&mutex_Exit);
		logger(escribir_loguear, l_info, "El pcb %s fue terminado\n", pcb->path);

	}

}

void operacion_create(char* nombre_tabla, char* tipo_consistencia, int num_particiones, int compaction_time, tp_lql_pcb pcb, int socket_memoria){

	if(!existeTabla(nombre_tabla)){
		logger(escribir_loguear, l_info, "Se le solicita a la memoria crear la tabla: %s", nombre_tabla);
		prot_enviar_create(nombre_tabla, tipo_consistencia, num_particiones, compaction_time, socket_memoria);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		enum MENSAJES rta_creacion = prot_recibir_respuesta_create(socket_memoria);

		if(rta_creacion == REQUEST_SUCCESS){
			logger(escribir_loguear, l_info, "La memoria realizo el create correctamente");
			tp_entrada_tabla_creada entrada = calloc(1, sizeof(t_entrada_tabla_creada));
			entrada->nombre_tabla = string_duplicate(nombre_tabla);
			entrada->criterio = string_duplicate(tipo_consistencia);
			pthread_mutex_lock(&mutex_tablas);
			list_add(listaTablasCreadas, entrada);
			pthread_mutex_unlock(&mutex_tablas);
			logger(escribir_loguear, l_info, "Tabla %s agregada a metadata\n", entrada->nombre_tabla);
			mostrar_lista_tablas();

		}
		if(rta_creacion == TABLA_YA_EXISTIA){
			logger(escribir_loguear, l_info, "Ya existe la tabla que queres crear");
		}

	}else{//terminar script

		logger(escribir_loguear, l_error, "Ya existe la tabla en la metadata del Kernel %s\n", nombre_tabla);
		pthread_mutex_lock(&mutex_Exec);
		remover_pcb_de_lista(listaExec, pcb);
		pthread_mutex_unlock(&mutex_Exec);
		pthread_mutex_lock(&mutex_Exit);
		list_add(listaExit, pcb);
		pthread_mutex_unlock(&mutex_Exit);
		logger(escribir_loguear, l_info, "El pcb %s fue terminado\n", pcb->path);

	}
}

int obtener_pos_tabla(char* tabla){
	int pos = 0;

	bool coincideTabla(void* nodo){
			if(string_equals_ignore_case(((tp_entrada_tabla_creada) nodo)->nombre_tabla, tabla)){
				return true;
			}
			pos++;
			return false;
	}

			if(list_any_satisfy(listaTablasCreadas, coincideTabla)){
				return pos;
			}
		return -1;
}

void describeAll(int socket_memoria) {
	//PIDIERON UN DESCRIBE ALL
	prot_enviar_describeAll(socket_memoria);

	//Recibo rta
	t_cabecera rta_pedido = recibirCabecera(socket_memoria);


	if (rta_pedido.tipoDeMensaje == REQUEST_SUCCESS) {

		tp_describeAll_rta info_de_las_tablas = prot_recibir_respuesta_describeAll(rta_pedido.tamanio, socket_memoria);

		//actualizo metadata



		void actualizarTabla(void* nodo) {
			if(!existeTabla(((tp_describe_rta) nodo)->nombre)){
				tp_entrada_tabla_creada tabla = calloc(1, sizeof(t_entrada_tabla_creada));
				tabla->nombre_tabla = string_duplicate(((tp_describe_rta) nodo)->nombre);
				tabla->criterio = string_duplicate(((tp_describe_rta) nodo)->consistencia);
				pthread_mutex_lock(&mutex_tablas);
				list_add(listaTablasCreadas, tabla);
				pthread_mutex_unlock(&mutex_tablas);
			}
		}

		list_iterate(info_de_las_tablas->lista, actualizarTabla);

		void removerTabla(void*nodo){
			if(tablaFueBorrada(((tp_entrada_tabla_creada) nodo)->nombre_tabla, info_de_las_tablas->lista)){
				int pos = obtener_pos_tabla(((tp_entrada_tabla_creada) nodo)->nombre_tabla);
				pthread_mutex_lock(&mutex_tablas);
				list_remove(listaTablasCreadas, pos);
				pthread_mutex_unlock(&mutex_tablas);
			}

		list_iterate(listaTablasCreadas, removerTabla);

		}
		//Libero la lista
		prot_free_tp_describeAll_rta(info_de_las_tablas);

	}

	if (rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA) {
		logger(escribir_loguear, l_error, "No hay tablas en el FS");
	}
}

void mostrar_lista_tablas(){
	int i;
	int size = list_size(listaTablasCreadas);

	for (i = 0; i < size; ++i) {

		tp_entrada_tabla_creada tabla = list_get(listaTablasCreadas, i);
		printf("Tabla: %s. Criterio: %s\n", tabla->nombre_tabla, tabla->criterio);

	}
}

void operacion_describe(char* nombre_tabla, tp_lql_pcb pcb, int socket_memoria){
	if(nombre_tabla != NULL){
		logger(escribir_loguear, l_info, "Se le solicita a la memoria el describe de la tabla: %s", nombre_tabla);

		mostrar_lista_tablas();

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

			tp_entrada_tabla_creada entrada_tabla = buscarTablaEnMetadata(info_tabla->nombre);
			if(entrada_tabla != NULL){
				logger(escribir_loguear, l_info, "Existe la tabla %s en metadata y se va a actualizar", info_tabla->nombre);
				entrada_tabla->criterio = string_duplicate(info_tabla->consistencia);
			}else{
				logger(escribir_loguear, l_info, "No existe la tabla %s en metadata y se va a crear", info_tabla->nombre);
				entrada_tabla->nombre_tabla = string_duplicate(info_tabla->nombre);
				entrada_tabla->criterio = string_duplicate(info_tabla->consistencia);
			}


			free(info_tabla->consistencia);
			free(info_tabla);
		}

		if(rta_pedido.tipoDeMensaje == TABLA_NO_EXISTIA){
			logger(escribir_loguear, l_error, "No existe la tabla");
		}


	}

	if(nombre_tabla == NULL){
		//PIDIERON UN DESCRIBE ALL
		describeAll(socket_memoria);
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

void operacion_journal(int socket){
	prot_enviar_journal(socket);
	enum MENSAJES respuesta = prot_recibir_respuesta_journal(socket);
	if(respuesta == REQUEST_SUCCESS){
		logger(escribir_loguear, l_info, "La memoria realizo un JOURNAL correctamente");
	}else{
		logger(escribir_loguear, l_info, "Error al realizar el JOURNAL en la memoria");
	}

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
			operacion_journal(socket_memoria);
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
			logger(escribir_loguear, l_trace ,"Se creo el hilo para la planificacion a largo plazo");
			return EXIT_SUCCESS;
			}

	return EXIT_SUCCESS;
}

void* funcionHiloPLP(){
	char *ret="Cerrando hilo PLP";
	while(1){
		logger(escribir_loguear, l_trace, "El PLP esta bloqueado\n"); /// para salto de linea es barra invertida

		sem_wait(&NEW);
		logger(escribir_loguear, l_trace, "Se desbloqueo el PLP\n");
		tp_lql_pcb nuevo_pcb;
		pthread_mutex_lock(&mutex_New);
		nuevo_pcb = list_remove(listaNew, 0); //remueve el primer elemento y lo retorna
		pthread_mutex_unlock(&mutex_New);
		pthread_mutex_lock(&mutex_Ready);
		list_add(listaReady, nuevo_pcb); //pasa el nuevo pcb a Ready
		pthread_mutex_unlock(&mutex_Ready);
		sem_post(&READY);// paso un LQL a ready, habilito PCP
		logger(escribir_loguear, l_info, "El LQL %s pasa a Ready\n", nuevo_pcb->path);
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
				logger(escribir_loguear, l_trace ,"Se creo el hilo para la planificacion a corto plazo");
				return EXIT_SUCCESS;
				}

		return EXIT_SUCCESS;
}

void* funcionHiloPCP(){
	char *ret="Cerrando hilo PCP";
	while(1){
		sem_wait(&READY);
		if(list_size(listaReady) > 0 && list_size(listaExec) < configKernel.multiprocesamiento){
			//sem_wait(&READY);
			logger(escribir_loguear, l_trace, "Se activa el PCP");
			tp_lql_pcb pcb_a_planificar;
			pthread_mutex_lock(&mutex_Ready);
			pcb_a_planificar = list_remove(listaReady, 0); //devuelve el primer elemento de la lista de Ready
			pthread_mutex_unlock(&mutex_Ready);
			pthread_mutex_lock(&mutex_Exec);
			list_add(listaExec, pcb_a_planificar);//Agrega el pcb a la lista de ejecutando
			pthread_mutex_unlock(&mutex_Exec);

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
		logger(escribir_loguear, l_trace ,"Se creo el hilo para la planificacion del LQL %s\n", pcb->path);
		return EXIT_SUCCESS;
		}

return EXIT_SUCCESS;

}

void* funcionHiloRequest(void* pcb){
	char *ret="Cerrando hilo Request";
	char *ret2="No hay memoria asignada al criterio, se cierra el hilo";
	int i;
	char* linea_a_ejecutar;

	while(!list_is_empty((*(tp_lql_pcb) pcb).lista)){//mientras no sea fin de archivo
		for (i = 0; i < quantum; ++i) {

		linea_a_ejecutar = list_remove((*(tp_lql_pcb) pcb).lista, 0); //lo saca de la lista y lo devuelve, de esta manera controlamos la prox linea a ejecutar

			if(linea_a_ejecutar!=NULL){
				//Parsear la linea
				t_operacion rdo_del_parseado = parsear(linea_a_ejecutar);

				//Elegir memoria de acuerdo a la tabla

				tp_memo_del_pool_kernel memoria = decidir_memoria_a_utilizar(rdo_del_parseado);
				if(memoria == NULL){
					logger(escribir_loguear, l_error, "Se cierra el hilo del LQL");
					pthread_exit(ret2);
				}

				//TODO controlar estado de la memoria. FULL: forzar journal. JOURNALING: esperar.

				usleep(retardo * 1000);

				int sockMem = conectar_con_memoria(memoria->ip, memoria->puerto);

				if(sockMem > 0){
					realizar_operacion(rdo_del_parseado, pcb, sockMem);
					close(sockMem);
					logger(escribir_loguear, l_info, "Se cierra el socket %i con la memoria %i", sockMem, memoria->numero_memoria);

				}


			}
		}

		if(!pcbEstaEnLista(listaExit, pcb)){
			pthread_mutex_lock(&mutex_Exec);
			remover_pcb_de_lista(listaExec, pcb);
			pthread_mutex_unlock(&mutex_Exec);
			pthread_mutex_lock(&mutex_Ready);
			list_add(listaReady, pcb);
			pthread_mutex_unlock(&mutex_Ready);
			logger(escribir_loguear, l_trace, "El PCB %s vuelve a READY\n", ((tp_lql_pcb) pcb)->path);
			sem_post(&READY);
			pthread_exit(ret);
			return EXIT_SUCCESS;
		}
	}

	logger(escribir_loguear, l_warning, "HASTA ACA LLEGA EL LQL");
	pthread_mutex_lock(&mutex_Exec);
	remover_pcb_de_lista(listaExec, pcb);
	pthread_mutex_unlock(&mutex_Exec);
	pthread_mutex_lock(&mutex_Exit);
	list_add(listaExit, pcb);
	pthread_mutex_unlock(&mutex_Exit);
	logger(escribir_loguear, l_warning, "El LQL %s pasa a Exit\n", ((tp_lql_pcb) pcb)->path);
	sem_post(&READY);
	pthread_exit(ret);
	return EXIT_SUCCESS;
}

char* obtenerTabla(t_operacion resultado_del_parseado){
	char* tabla = string_new();
	switch(resultado_del_parseado.tipo_de_operacion){
		case _SELECT:
			tabla = string_duplicate(resultado_del_parseado.parametros.select.nombre_tabla);
			break;
		case _INSERT:
			tabla = string_duplicate(resultado_del_parseado.parametros.insert.nombre_tabla);
			break;
		case _CREATE:
			tabla = string_duplicate(resultado_del_parseado.parametros.create.nombre_tabla);
			break;
		case _DESCRIBE:
			if(resultado_del_parseado.parametros.describe.nombre_tabla != NULL){
				tabla = string_duplicate(resultado_del_parseado.parametros.describe.nombre_tabla);
			}else{
				tabla = NULL;
			}
			break;
		case _DROP:
			tabla = string_duplicate(resultado_del_parseado.parametros.drop.nombre_tabla);
			break;
	}
	return tabla;
}

bool existeTabla(char* tabla){

	bool coincideNombre(void* nodo){
		return (string_equals_ignore_case(((tp_entrada_tabla_creada) nodo)->nombre_tabla, tabla));
	}

	return list_any_satisfy(listaTablasCreadas, coincideNombre);
}

bool tablaFueBorrada(char* tabla, t_list* lista){

	bool mismoNombre(void*nodo){
		return (string_equals_ignore_case(((tp_describe_rta)nodo)->nombre, tabla));
	}

	return list_any_satisfy(lista, mismoNombre);
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

tp_memo_del_pool_kernel decidir_memoria_a_utilizar(t_operacion operacion){
	tp_memo_del_pool_kernel memoria = calloc(1, sizeof(t_memo_del_pool_kernel));
	char* criterio = string_new();
	tp_entrada_tabla_creada entrada;

	//memoria = list_get(listaSC, 0);

	char* tabla = obtenerTabla(operacion);


	if(tabla == NULL){
		memoria = list_get(listaMemConectadas, 0);
		return memoria;

	}
	if(operacion.tipo_de_operacion == _CREATE){
		criterio = string_duplicate(operacion.parametros.create.tipo_consistencia);

	}else{
	//buscar tabla en listaTablasCreadas y obtener el criterio

		entrada = buscarTablaEnMetadata(tabla);


		criterio = string_duplicate(entrada->criterio);
	}
	logger(escribir_loguear, l_info, "Eligiendo memoria para la tabla %s\n", tabla);
	//buscar las memorias que tengan ese criterio asignado y elegir
	if((operacion.tipo_de_operacion == _CREATE) || (entrada != NULL)){
		if((string_equals_ignore_case(criterio, "SC")) && (!list_is_empty(listaSC))) {
				pthread_mutex_lock(&mutex_SC);
				memoria = list_get(listaSC, 0);
				pthread_mutex_unlock(&mutex_SC);
				logger(escribir_loguear, l_info, "Se eligio la memoria %i para el criterio SC", memoria->numero_memoria);
				//free(criterio);
				return memoria;
		}else if(string_equals_ignore_case(criterio, "HC") && (!list_is_empty(listaHC))){
				if((operacion.tipo_de_operacion == _SELECT)){
					int numHash = calcularHash(operacion.parametros.select.key);
					pthread_mutex_lock(&mutex_HC);
					memoria = list_get(listaHC, numHash);
					pthread_mutex_unlock(&mutex_HC);
					logger(escribir_loguear, l_info, "Se eligio la memoria %i para el criterio HC", memoria->numero_memoria);
					return memoria;
				}else if((operacion.tipo_de_operacion == _INSERT)){
					int numHash = calcularHash(operacion.parametros.insert.key);
					pthread_mutex_lock(&mutex_HC);
					memoria = list_get(listaHC, numHash);
					pthread_mutex_unlock(&mutex_HC);
					logger(escribir_loguear, l_info, "Se eligio la memoria %i para el criterio HC", memoria->numero_memoria);
					return memoria;
				}else{
					int num = (rand() % list_size(listaHC));
					pthread_mutex_lock(&mutex_HC);
					memoria = list_get(listaHC, num);
					pthread_mutex_unlock(&mutex_HC);
					logger(escribir_loguear, l_info, "Se eligio la memoria %i para el criterio HC", memoria->numero_memoria);
					return memoria;
				}
		}else if(string_equals_ignore_case(criterio, "EC") && (!list_is_empty(listaEC))){
				bool entra = true;
				while(entra){
				int num = (rand() % list_size(listaEC)); // calcula un random entre 0 y list size
				pthread_mutex_lock(&mutex_EC);
				memoria = list_get(listaEC, num);
				pthread_mutex_unlock(&mutex_EC);
				if(memoria->numero_memoria != ultima_memoria_EC){
					ultima_memoria_EC = memoria->numero_memoria;
					entra = false;
				}
				}
				logger(escribir_loguear, l_info, "Se eligio la memoria %i para el criterio EC", memoria->numero_memoria);
				return memoria;
		}}

	logger(escribir_loguear, l_error, "No hay ninguna memoria asignada al criterio %s\n", criterio);
	memoria = NULL;
	//free(criterio);
	//free(entrada);

	return memoria;
}

int calcularHash(int key){
	int hash = (key % list_size(listaHC));
	return hash;
}

tp_memo_del_pool_kernel buscar_memorias_segun_numero(t_list* lista, int numero){

	bool coincideNumero(void* nodo){
			if(((tp_memo_del_pool_kernel) nodo)->numero_memoria == numero){
				return true;
			}
			return false;
		}

	tp_memo_del_pool_kernel memo = list_find(lista, coincideNumero);
	return memo;

}

tp_entrada_tabla_creada buscarTablaEnMetadata(char* tabla){
	tp_entrada_tabla_creada entrada = calloc(1, sizeof(t_entrada_tabla_creada));

	bool coincideNombre2(void* nodo){
			if(strcmp(((tp_entrada_tabla_creada) nodo)->nombre_tabla, tabla)==0){
				return true;
			}
			return false;
		}

	entrada = list_find(listaTablasCreadas, coincideNombre2);
	return entrada;
}

void iniciar_proceso_describe_all(){
	pthread_t hiloDescribeAll;
		int resultado_de_crear_el_hilo = pthread_create (&hiloDescribeAll, NULL, hacer_describe, NULL);
		if(resultado_de_crear_el_hilo!=0){
			logger(escribir_loguear,l_error
					,"Error al crear el hilo de describe all, el kernel se suicida");
			terminar_programa(EXIT_FAILURE);
		}
		pthread_detach(hiloDescribeAll);
}

void* hacer_describe(){
	while(1){
		usleep(configKernel.refreshMetadata*1000);
		if(list_is_empty(listaMemConectadas)){
			continue;
		}
		logger(escribir_loguear, l_debug, "Se le solicita la memoria de conf un describe all");
		int socket_mem = abrir_socket_memoria(configKernel.ipMemoria, configKernel.puertoMemoria);
		prot_recibir_int(socket_mem);
		describeAll(socket_mem);
		close(socket_mem);
	}
	return EXIT_SUCCESS;
}

void iniciar_pedido_gossip(){
	pthread_t hiloGossip;
	int resultado_de_crear_el_hilo = pthread_create (&hiloGossip, NULL, pedir_gossip, NULL);
	if(resultado_de_crear_el_hilo!=0){
		logger(escribir_loguear,l_error
				,"Error al crear el hilo de pedido de gossip, el kernel se suicida");
		terminar_programa(EXIT_FAILURE);
	}
	pthread_detach(hiloGossip);
}

void* pedir_gossip(){
	while(1){
		usleep(configKernel.gossip_time*1000);
		logger(escribir_loguear, l_info, "Voy a pedir la tabla de gossip");
		int socket_mem = abrir_socket_memoria(configKernel.ipMemoria, configKernel.puertoMemoria);
		prot_recibir_int(socket_mem);
		prot_enviar_pedido_tabla_gossiping(socket_mem);

		logger(escribir_loguear, l_info, "Espero la rta de memoria...");
		t_cabecera rta_pedido = recibirCabecera(socket_mem);

		if(rta_pedido.tipoDeMensaje == PEDIDO_KERNEL_GOSSIP){
			logger(escribir_loguear, l_info, "La memoria envio la tabla de gossip correctamente");
			tp_tabla_gossiping tabla_nueva = prot_recibir_tabla_gossiping(rta_pedido.tamanio, socket_mem);

			//conectarme a las memorias nuevas de la tabla gossip
			conectarse_a_memorias_gossip(tabla_nueva->lista);

			logger(escribir_loguear, l_info, "Esta es la informacion recibida:");

			void imprimir_informacion_memoria_ajena(void * memoria_ajena){
				logger(escribir_loguear, l_debug, "El ip de la memoria es: %s", (*(t_memo_del_pool*)memoria_ajena).ip);
				logger(escribir_loguear, l_debug, "El puerto de la memoria es: %s", (*(t_memo_del_pool*)memoria_ajena).puerto);
			}

			list_iterate(tabla_nueva->lista, imprimir_informacion_memoria_ajena);

			//Libero la estructura que recibi
			free(tabla_nueva);

		}
		close(socket_mem);

	}

}

void conectarse_a_memorias_gossip(t_list* lista_gossip){

	void verificar_si_memoria_existe_en_mi_tabla_para_agregarla(void * memoria_gossip){
			bool memoria_existe_en_mi_lista(void * memoria){
				return string_equals_ignore_case(((tp_memo_del_pool)memoria_gossip)->ip,((tp_memo_del_pool_kernel)memoria)->ip)
						&& string_equals_ignore_case(((tp_memo_del_pool)memoria_gossip)->puerto,((tp_memo_del_pool_kernel)memoria)->puerto);
			}
			if(!list_any_satisfy(listaMemConectadas, memoria_existe_en_mi_lista)){
				logger(escribir_loguear, l_debug, "Voy a conectarme a la memoria");
				conectarse_con_memoria(string_duplicate(((t_memo_del_pool*)memoria_gossip)->ip),
								string_duplicate(((t_memo_del_pool*)memoria_gossip)->puerto));
			}
		}
		list_iterate(lista_gossip, verificar_si_memoria_existe_en_mi_tabla_para_agregarla);

}



