#ifndef COM_H_
#define COM_H_


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <commons/collections/list.h>

enum PROCESO {
	LISSANDRA,
	KERNEL,
	MEMORIA
};

enum MENSAJES {
	TABLA_YA_EXISTIA,
	TABLA_CREADA,
	TABLA_NO_EXISTIA,
	TABLA_BORRADA
};

enum COMANDOS {
	SELECT,
	INSERT,
	CREATE,
	DESCRIBE,
	DROP,
	JOURNAL,
	RUN,
	ADD,
	METRICS
};

struct memo_del_pool {
	char * ip;
	char * puerto;
};
typedef struct memo_del_pool t_memo_del_pool;

typedef struct {
	enum MENSAJES tipoDeMensaje;
	int tamanio;
}__attribute__((packed)) t_cabecera;

int escucharEn(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int escucha);
int enviar(int sock, void *mensaje, int tamanio);
int recibir(int sock, void *mensaje, int tamanio);
void cerrarConexion(int sock);
/* Funcionamiento de enviar y recibir handshake:
 * El cliente hace un enviar handshake al server y el server del otro lado
 * hace un recibir handshake, no hace falta hacer nada mas, por parametro le
 * pasamos los procesos que intervienen y las mismas funciones chequean que sean
 * los procesos que corresponden los que se estan comunicando
 * */
int enviarHandshake(enum PROCESO emisor,enum PROCESO procesoEsperado,int sock);
int recibirHandshake(enum PROCESO receptor, enum PROCESO procesoEsperado, int sock);
int enviarCabecera(int sock, enum MENSAJES tipoDeMensaje,int tamanio);
t_cabecera recibirCabecera(int sock);

#endif
