/*
 ============================================================================
 Name        : Memoria.c
 Author      : Tamara-Francisco
 Version     :
 Copyright   : Your copyright notice
 Description : Proceso memoria
 ============================================================================
 */
#include "Memoria.h"


int main(int argc, char ** argv) {

	iniciar_logger();
	iniciar_config(argc,argv);
	leer_config();
	configurar_signals();
	int socket_lfs = comunicarse_con_lissandra();
	stdin_no_bloqueante();
	inicializar_conexiones_cliente();

	int server_memoria = iniciar_servidor();

	escuchar_clientes(server_memoria, socket_lfs);
	return EXIT_SUCCESS;
}

void crear_hilo_conexion(int socket, void*funcion_a_ejecutar(int)){
	pthread_t hilo;
	pthread_create(&hilo,NULL,(void*)funcion_a_ejecutar,(void*)socket);
	pthread_detach(hilo);
}

void inicializar_conexiones_cliente(void){
	for (int i = 0; i < MAX_CLIENTES; ++i){
		conexiones_cliente[i].socket = NO_SOCKET;
	}
}

int atender_nuevo_cliente(int serv_socket){
	struct sockaddr_in client_addr;

	//Setea la direccion en 0
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	//Acepta la nueva conexion
	int new_client_sock = accept(serv_socket, (struct sockaddr *)&client_addr, &client_len);
	if (new_client_sock < 0) {
		logger(escribir_loguear,l_error,"Error al aceptar un nuevo cliente :(\n");
		return -1;
	}

	logger(escribir_loguear,l_trace,"\nSe aceptó un nuevo cliente, conexión (%d)\n", new_client_sock);

	//Lo agrego a la lista de conexiones con clientes actuales
	for (int i = 0; i < MAX_CLIENTES; ++i) {

		if (conexiones_cliente[i].socket == NO_SOCKET) {
			conexiones_cliente[i].socket = new_client_sock;
			conexiones_cliente[i].addres = client_addr;

	        return 0;
	    }
	}

	logger(escribir_loguear,l_error,"Demasiadas conexiones. Cerrando nueva conexion\n");
	close(new_client_sock);

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
	return socket_liss;
}

void escuchar_clientes(int server_memoria, int socket_lfs) {
	fd_set readset, writeset, exepset;
	int max_fd;
	char read_buffer[MAX_LINEA];
	struct timeval tv = { 0, 500 };
	logger(escribir_loguear, l_debug, "Se pone a escuchar clientes...");
	while (true) {
		//Inicializa los file descriptor
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_ZERO(&exepset);
		tv.tv_sec = 5;
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
		logger(loguear, l_debug, "Resultado del select: %d\n", result); //Revisar rendimiento del CPU cuando select da > 1
		if (result < 0) {
			logger(escribir_loguear, l_error, "Error en select\n");
			break;
		} else if (errno == EINTR) {
			logger(escribir_loguear, l_error, "Me mataron! salgo del select\n");
			break;
		} else if (result > 0) //Hubo un cambio en algun fd
				{
			//Aceptar nuevas conexiones de clientes
			if (FD_ISSET(server_memoria, &readset)) {
				atender_nuevo_cliente(server_memoria);
			}
			//Se ingresó algo a la consola
			if (FD_ISSET(STDIN_FILENO, &readset)) {
				consola_leer_stdin(read_buffer, MAX_LINEA);
				if (*((char*) read_buffer) == '\0') {
					//Solo apretaron enter, no hay que interpretar nada
					continue;
				}
				int res = consola_derivar_comando(read_buffer);
				if (res) {
					terminar_programa();
					break;
				}
			}
			/*				//Manejo de conexiones ya existentes
			 for(int i = 0; i < MAX_CLIENTES; ++i){
			 if(conexiones_cliente[i].socket != NO_SOCKET ){
			 //Mensajes nuevos de algun cliente
			 if (FD_ISSET(conexiones_cliente[i].socket, &readset)) {
			 if(hay un error al recibir un mensaje)
			 {
			 logger(escribir_loguear,l_trace,"Conexión con cliente %d cerrada\n",conexiones_cliente[i].pid);
			 close(conexiones_cliente[i].socket);
			 conexiones_cliente[i].socket = NO_SOCKET;
			 continue;
			 }
			 }

			 //Excepciones del cliente, para la desconexion
			 if (FD_ISSET(conexiones_cliente[i].socket, &exepset)) {
			 if(hay un error al recibir un mensaje)
			 {
			 logger(escribir_loguear,l_trace,"Conexión con cliente %d cerrada\n",conexiones_cliente[i].pid);
			 close(conexiones_cliente[i].socket);
			 conexiones_cliente[i].socket = NO_SOCKET;
			 continue;
			 }
			 }//if isset
			 }// if NO_SOCKET
			 }//for conexiones_cliente */
		}
	}
}
