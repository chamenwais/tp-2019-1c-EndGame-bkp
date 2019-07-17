/*
 ============================================================================
 Name        : Memoria.c
 Author      : Tamara-Francisco
 Version     :
 Copyright   : UTN-FRBA-2019
 Description : Proceso memoria
 ============================================================================
 */
#include "Memoria.h"


int  main(int argc, char ** argv) {

	if(argc>1 && string_equals_ignore_case(argv[1],"run-tests")){
		return correr_tests();
	} else {
		ejecutar_programa_memoria(argc, argv);
	}
	return EXIT_SUCCESS;
}

void ejecutar_programa_memoria(int argc, char ** argv){
	iniciar_logger();
	inicializar_semaforos();
	iniciar_config(argc,argv);
	leer_config();
	configurar_signals();
	int socket_lfs = comunicarse_con_lissandra();
	stdin_no_bloqueante();
	inicializar_conexiones_cliente();

	iniciar_la_memoria_principal();
	iniciar_el_proceso_de_gossiping();
	iniciar_el_proceso_de_journaling(socket_lfs);

	int server_memoria = iniciar_servidor();

	escuchar_clientes(server_memoria, socket_lfs);
}

void iniciar_la_memoria_principal(){
	MEMORIA_PRINCIPAL = reservar_total_memoria();

	int tamanio_marco = obtener_tamanio_marco();
	logger(escribir_loguear,l_debug
						,"Tamaño de marco: %d",tamanio_marco);
	int cantidad_marcos = obtener_cantidad_marcos_en_MP(tamanio_marco);
	logger(escribir_loguear,l_info
					,"Memoria inicializada con %d marcos",cantidad_marcos);
	inicializar_bitmap_marcos();
	inicializar_tabla_segmentos();

}

char *reservar_total_memoria(){
	return malloc(TAMANIO_MEMORIA);
}

void iniciar_el_proceso_de_gossiping(){
	pthread_t idHilo;
	int resultado_de_hacer_el_hilo = pthread_create (&idHilo, NULL, realizar_gossiping, NULL);
	if(resultado_de_hacer_el_hilo!=0){
		logger(escribir_loguear,l_error
				,"Error al crear el hilo de gossiping, vas a tener que levantar la memoria de nuevo");
		terminar_programa(EXIT_FAILURE);
	}
	pthread_detach(idHilo);
}

t_list * conectarse_a_seeds(){
	t_list * tabla_de_gossip=list_create();
	int cantidad_de_seeds = list_size(seeds);

	for(int i=0;i<cantidad_de_seeds;i++){
		t_memo_del_pool * memoria_a_utilizar = list_get(seeds,i);

		bool esta_la_memoria(void* memoria_consultar){
			return ((t_memo_del_pool *) memoria_consultar == memoria_a_utilizar);
		}

		if(!list_any_satisfy(mi_tabla_de_gossip,esta_la_memoria)){
			logger(escribir_loguear, l_trace, "Se va a tratar de conectar a la ip %s, puerto %s"
					,memoria_a_utilizar->ip, memoria_a_utilizar->puerto);

			int memoria_conectada = conectarseA(memoria_a_utilizar->ip, atoi(memoria_a_utilizar->puerto));

			if(memoria_conectada>0){
				logger(escribir_loguear,l_debug, "Me conecte con una memoria de los seeds");
				agregar_conexion_lista_clientes(memoria_conectada, crear_direccion_cliente());
				enviarHandshake(MEMORIA, MEMORIA, memoria_conectada);
				memoria_a_utilizar->socket=memoria_conectada;
				list_add(tabla_de_gossip, memoria_a_utilizar);
			}
		}
		if(memoria_a_utilizar->socket>0){
			enviar_y_esperar_tabla_gossip(memoria_a_utilizar->socket);
		}
	}

	return tabla_de_gossip;
}

void *realizar_gossiping(){
	pthread_mutex_lock(&M_TABLA_GOSSIP);
	mi_tabla_de_gossip = list_create();
	pthread_mutex_unlock(&M_TABLA_GOSSIP);
	t_memo_del_pool* memoria_a_utilizar = crear_memo_del_pool(conocer_ip_propia(),PUERTO_ESCUCHA);
	pthread_mutex_lock(&M_TABLA_GOSSIP);
	list_add(mi_tabla_de_gossip, memoria_a_utilizar);
	pthread_mutex_unlock(&M_TABLA_GOSSIP);
	while(1){
		usleep(TIEMPO_GOSSIPING*1000);
		logger(escribir_loguear, l_trace, "\nProcedo a realizar el gossiping...\n");
		t_list* nuevas_memorias_conectadas = conectarse_a_seeds();
		pthread_mutex_lock(&M_TABLA_GOSSIP);
		list_add_all(mi_tabla_de_gossip, nuevas_memorias_conectadas);
		pthread_mutex_unlock(&M_TABLA_GOSSIP);
		list_destroy(nuevas_memorias_conectadas);
	}
	return EXIT_SUCCESS;
}

void iniciar_el_proceso_de_journaling(int socket_LFS){
	pthread_t idHilo;
	int resultado_de_hacer_el_hilo = pthread_create (&idHilo, NULL, realizar_journaling, &socket_LFS);
	if(resultado_de_hacer_el_hilo!=0){
		logger(escribir_loguear,l_error
				,"Error al crear el hilo de journal, vas a tener que levantar la memoria de nuevo");
		terminar_programa(EXIT_FAILURE);
	}
	pthread_detach(idHilo);
}

void *realizar_journaling(void * args){
	int LFS_fd=*((int*)args);
	while(1){
		usleep(TIEMPO_JOURNAL*1000);
		logger(escribir_loguear, l_trace,
				"\nProcedo a realizar el journal ya que pasaron %d miliseg...\n", TIEMPO_JOURNAL);

		//Hace el journaling
		notificar_escrituras_en_memoria_LFS(LFS_fd);

	}
	return EXIT_SUCCESS;
}

void inicializar_conexiones_cliente(void){
	for (int i = 0; i < MAX_CLIENTES; ++i){
		conexiones_cliente[i].socket = NO_SOCKET;
	}
}

struct sockaddr_in crear_direccion_cliente() {
	struct sockaddr_in client_addr;
	//Setea la direccion en 0
	memset(&client_addr, 0, sizeof(client_addr));
	return client_addr;
}

void atender_clientes(int serv_socket){
	struct sockaddr_in client_addr = crear_direccion_cliente();
	socklen_t client_len = sizeof(client_addr);

	//Acepta la nueva conexion
	int socket_cliente = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);
	if (socket_cliente < 0) {
		logger(escribir_loguear,l_error,"Error al aceptar al cliente :(");
	}

	logger(escribir_loguear,l_trace,"Se aceptó al cliente, conexión (%d)", socket_cliente);

	recibir_handshakes(socket_cliente);

	int cliente_aceptado=agregar_conexion_lista_clientes(socket_cliente, client_addr);

	if(cliente_aceptado==-1){
		logger(escribir_loguear,l_error,"Demasiadas conexiones. Cerrando nueva conexion\n");
		close(socket_cliente);
	}
}

int agregar_conexion_lista_clientes(int socket_conexion,struct sockaddr_in direccion_cliente){
	//Lo agrego a la lista de conexiones con clientes actuales
	for (int i = 0; i < MAX_CLIENTES; ++i) {

		if (conexiones_cliente[i].socket == NO_SOCKET) {
			conexiones_cliente[i].socket = socket_conexion;
			conexiones_cliente[i].addres = direccion_cliente;

			return socket_conexion;
		}
	}

	return -1;
}

void stdin_no_bloqueante(void){
	  /* Set nonblock for stdin. */
	  int flag = fcntl(STDIN_FILENO, F_GETFL, 0);
	  flag |= O_NONBLOCK;
	  fcntl(STDIN_FILENO, F_SETFL, flag);
}

void obtener_tamanio_value(int socket_liss) {
	TAMANIO_VALUE = prot_recibir_int(socket_liss);
	logger(escribir_loguear, l_debug, "El tamaño máximo de un value es de %d",
			TAMANIO_VALUE);
}

int comunicarse_con_lissandra(void){
	mostrar_mensaje_previa_conexion_con(LFS, IP_FILESYSTEM, PUERTO_FILESYSTEM);
	int socket_liss = conectarseA(IP_FILESYSTEM, PUERTO_FILESYSTEM);
	validar_comunicacion(socket_liss, LFS);
	mandar_handshake_a(LFS, socket_liss, LISSANDRA);
	obtener_tamanio_value(socket_liss);
	SOCKET_LISS = socket_liss;
	return socket_liss;
}

void escuchar_clientes(int server_memoria, int socket_lfs) {
	fd_set readset, writeset, exepset;
	int max_fd;
	char read_buffer[MAX_LINEA];
	struct timeval tv = { 0, 500 };

	//Agrego el socket del lfs a la lista de clientes
	agregar_conexion_lista_clientes(socket_lfs, crear_direccion_cliente());

	logger(escribir_loguear, l_debug, "Se pone a escuchar clientes...");
	while (true) {
		//Inicializa los file descriptor
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_ZERO(&exepset);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		//Agrega el fd del socket servidor al set de lectura y excepciones
		FD_SET(server_memoria, &readset);
		FD_SET(server_memoria, &exepset);
		//Agrega el stdin para leer la consola
		FD_SET(STDIN_FILENO, &readset);
		//Seteo el maximo file descriptor necesario para el select
		max_fd = server_memoria;
		//Agrega los conexiones existentes
		for (int i = 0; i < MAX_CLIENTES; i++) {
			if (conexiones_cliente[i].socket != NO_SOCKET) {
				FD_SET(conexiones_cliente[i].socket, &readset);
				FD_SET(conexiones_cliente[i].socket, &exepset);
			}
			if (conexiones_cliente[i].socket > max_fd) {
				max_fd = conexiones_cliente[i].socket;
			}
		}
		int result = select(max_fd + 1, &readset, &writeset, &exepset, &tv);
		logger(loguear, l_debug, "Resultado del select: %d\n", result);
		if (result < 0) {
			logger(escribir_loguear, l_error, "Error en select\n");
			break;
		} else if (errno == EINTR) {
			logger(escribir_loguear, l_error, "Me mataron! salgo del select\n");
			break;
		} else if (result > 0) //Hubo un cambio en algun fd
				{
			//Acepta la conexión del kernel
			if (FD_ISSET(server_memoria, &readset)) {
				atender_clientes(server_memoria);
			}
			//Se ingresó algo a la consola
			else if (FD_ISSET(STDIN_FILENO, &readset)) {
				consola_leer_stdin(read_buffer, MAX_LINEA);
				if (*((char*) read_buffer) == '\0') {
					//Solo apretaron enter, no hay que interpretar nada
					continue;
				}
				int res = consola_derivar_comando(read_buffer, socket_lfs);
				if (res) {
					terminar_programa(EXIT_FAILURE);
					break;
				}
			} else {
				//Manejo de conexiones ya existentes
				for(int i = 0; i < MAX_CLIENTES; ++i){
					if(conexiones_cliente[i].socket != NO_SOCKET ){
						//Mensajes nuevos de algun cliente
						if (FD_ISSET(conexiones_cliente[i].socket, &readset)) {
							int socket_llamador=conexiones_cliente[i].socket;
							t_cabecera cabecera=recibirCabecera(socket_llamador);
							if(cabecera.tamanio<0)
							{
								loguear_cerrar_conexion(socket_llamador,
										socket_lfs, i, readset);
								continue;
							}

							clasificar_y_atender_cabecera(socket_llamador, cabecera.tipoDeMensaje, cabecera.tamanio);

						} else if (FD_ISSET(conexiones_cliente[i].socket, &exepset)) {
								//Excepciones del cliente, para la desconexion
								loguear_cerrar_conexion(conexiones_cliente[i].socket,
										socket_lfs, i, readset);
								continue;
						}
					}// if != NO_SOCKET
				}//for conexiones_cliente */
			}
		}
	}
}

void clasificar_y_atender_cabecera(int socket_cliente, enum MENSAJES tipoDeMensaje, int tamanio){
	pthread_mutex_lock(&M_JOURNALING);
	switch(tipoDeMensaje){
		case CREATE: atender_create(socket_cliente, tamanio);
			break;
		case SELECT:
			atender_select(socket_cliente, tamanio);
			break;
		case INSERT:
			atender_insert(socket_cliente, tamanio);
			break;
		case DROP:
			atender_drop(socket_cliente, tamanio);
			break;
		case DESCRIBE: atender_describe(socket_cliente, tamanio);
			break;
		case JOURNAL: atender_journal(socket_cliente);
			break;
		case RECIBIR_GOSSIPING: recibir_tabla_de_gossip(socket_cliente, tamanio);
			break;
		case ENVIAR_Y_RECIBIR_GOSSIPING: recibir_y_enviar_tabla_de_gossip(socket_cliente, tamanio);
			break;
		case PEDIDO_KERNEL_GOSSIP: atender_gossiping_kernel(socket_cliente);
			break;
		default:
			break;
	}
	pthread_mutex_unlock(&M_JOURNALING);
}

int clasificar_conexion_cerrada(int socket_cerrado, int sock_lfs){
	int es_lfs=0;
	//loguea un mensaje acertado de qué proceso se desconectó
	if(socket_cerrado==sock_lfs){
		logger(escribir_loguear,l_warning,"Conexión con lissandra fs cerrada en socket %d", sock_lfs);
		return 1;
	} else {
		logger(escribir_loguear,l_warning,"Conexión con cliente %d cerrada", socket_cerrado);
		//Pregunto si fue una memoria para sacarla de la tabla Gossip
		remover_memoria_cerrada_de_tabla_gossip(socket_cerrado);
	}
	return es_lfs;
}

void loguear_cerrar_conexion(int socket_llamador, int socket_lfs, int i, fd_set bolsa_de_sockets) {
	int conexion_con_lfs=clasificar_conexion_cerrada(socket_llamador, socket_lfs);
	close(socket_llamador);
	conexiones_cliente[i].socket = NO_SOCKET;
	FD_CLR(socket_llamador,&bolsa_de_sockets);
	validar_conexion_con_lissandra(conexion_con_lfs);
}

void validar_conexion_con_lissandra(int cerrada){
	if(cerrada){
		terminar_programa(EXIT_FAILURE);
	}
}
