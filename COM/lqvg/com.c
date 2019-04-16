#include "com.h"

int escucharEn(int puerto){
	int escucha;
	struct sockaddr_in miDireccion;
	int yes=1;
	if ((escucha = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	if (setsockopt(escucha, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	miDireccion.sin_family = AF_INET;
	miDireccion.sin_addr.s_addr = INADDR_ANY;
	miDireccion.sin_port = htons(puerto);
	memset(&(miDireccion.sin_zero), '\0', 8);
	if (bind(escucha, (struct sockaddr *)&miDireccion, sizeof(miDireccion)) == -1) {
		perror("Falla el bind");
		exit(1);
	}

	if (listen(escucha, 30) == -1) {
		perror("listen");
		exit(1);
	}

	return escucha;
}

int conectarseA(char *ip, int puerto){
	int sock;
	struct sockaddr_in DireccionServidor;
	DireccionServidor.sin_family = AF_INET;
	DireccionServidor.sin_addr.s_addr = inet_addr(ip);
	DireccionServidor.sin_port = htons(puerto);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sock, (void*) &DireccionServidor, sizeof(DireccionServidor)) != 0) {
		perror("No se pudo conectar");
		return -1;
	}
	return sock;
}

int aceptarConexion(int escucha){
	struct sockaddr_in direccionRemota;
	int longitudDeLaDir;
	int sock;
	longitudDeLaDir = sizeof(direccionRemota);
	while ((sock = accept(escucha, (struct sockaddr*) &direccionRemota, &longitudDeLaDir)) == -1){
		//perror("Error en el accept");
		//return -1;
	}
	return sock;
}

int enviar(int sock, void *mensaje, int tamanio){
	int bytesEnviados;
	bytesEnviados = send(sock, mensaje, tamanio, 0);
	if (bytesEnviados <= 0) {
		perror("Error en el send");
		return -1;
	}
	return bytesEnviados;
}

int recibir(int sock, void *mensaje, int tamanio){
	int bytesRecibidos;
	if((bytesRecibidos = recv(sock, mensaje, tamanio, 0)) < 0) {
		perror("Error en el recv.\n");
		return -1;
	}
	return bytesRecibidos;
}

void cerrarConexion(int sock){
	close(sock);
}

int enviarHandshake(enum PROCESO emisor,enum PROCESO procesoEsperado,int sock){
	enviar(sock,&emisor,sizeof(emisor));
	enum PROCESO procesoRecibido;
	recibir(sock,&procesoRecibido,sizeof(procesoRecibido));
	if(procesoRecibido != procesoEsperado) {
		perror("Connected to wrong server");
		return false;
	}
	else
		return true;
}

int recibirHandshake(enum PROCESO receptor, enum PROCESO procesoEsperado, int sock){
	enum PROCESO procesoRecibido;
	recibir(sock,&procesoRecibido,sizeof(procesoRecibido));
	enviar(sock, &receptor, sizeof(receptor));
	if(procesoRecibido != procesoEsperado) {
		perror("Wrong client connected");
		return false;
	}
	else
		return true;
}

int enviarCabecera(int sock, enum MENSAJES tipoDeMensaje,int tamanio){
	t_cabecera cabecera;
	cabecera.tamanio = tamanio;
	cabecera.tipoDeMensaje = tipoDeMensaje;
	return enviar(sock,&cabecera,sizeof(cabecera));
}

t_cabecera recibirCabecera(int sock){
	t_cabecera cabecera;
	int bytesRecibidos = recibir(sock,&cabecera,sizeof(cabecera));
	cabecera.tamanio=bytesRecibidos;
	return cabecera;
}
