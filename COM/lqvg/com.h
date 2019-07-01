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
	SELECT,
	INSERT,
	CREATE,
	DESCRIBE,
	DROP,
	JOURNAL,
	REQUEST_SUCCESS,
	TABLA_YA_EXISTIA,
	TABLA_CREADA,
	TABLA_NO_EXISTIA,
	TABLA_BORRADA,
	POOL_REQUEST,
	NO_HAY_MAS_BLOQUES_EN_EL_FS,
	DUMP_CORRECTO,
	KEY_NO_EXISTE ,
	KEY_OBTENIDA,
	NO_HAY_MAS_MARCOS_EN_LA_MEMORIA,
	GOSSIPING,
	IP_MEMORIA
};

typedef struct {
	enum MENSAJES tipoDeMensaje;
	int tamanio;
}__attribute__((packed)) t_cabecera;

typedef struct
{
	void* stream;
} t_buffer;

typedef struct
{
	t_cabecera cabecera;
	t_buffer* buffer;
} t_paquete;

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
t_paquete* crear_paquete(enum MENSAJES);
void crear_buffer(t_paquete*);
void agregar_string_a_paquete(t_paquete*, void*, int);
void agregar_uint16_t_a_paquete(t_paquete*, uint16_t);
void agregar_int_a_paquete(t_paquete*, int);
void agregar_unsigned_int_a_paquete(t_paquete*, unsigned int);
void agregar_long_a_paquete(t_paquete*, long);
void enviar_paquete(t_paquete*, int);
void* serializar_paquete(t_paquete*, int);
void eliminar_paquete(t_paquete*);

#endif
