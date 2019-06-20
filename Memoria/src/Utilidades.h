/*
 * Utilidades.h
 *
 *  Created on: 8 abr. 2019
 *      Author: utnso
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include "Contexto.h"
#include "GestionSegPaginada.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )
#define BUF_LEN     ( 1024 * EVENT_SIZE )

void iniciar_logger(void);
void inicializar_semaforos();
void iniciar_config(int, char **);
void leer_config(void);
void configurar_signals(void);
void captura_signal(int);
void logger(int , int , const char* , ...);
void validar_apertura_archivo_configuracion();
void iniciar_escucha_cambios_conf(char *, char *);
void *escuchar_cambios_conf(void * );
void actualizar_campos_retardo(struct inotify_event*);
void obtener_valor_configuracion(char*, t_config*, void(*f)(void));
void obtener_puerto_escucha();
void obtener_ip_filesystem();
void obtener_puerto_filesystem();
void obtener_tamanio_memoria();
void obtener_numero_memoria();
void obtener_ip_seeds();
void obtener_puerto_seeds();
void obtener_retardo_acceso_memoria();
void obtener_retardo_acceso_filesystem();
void obtener_tiempo_journal();
void obtener_tiempo_gossiping();
void escribir_por_pantalla(int tipo_esc, int tipo_log, char* console_buffer,
		char* log_colors[8], char* msj_salida);
void definir_nivel_y_loguear(int tipo_esc, int tipo_log, char* msj_salida);
void terminar_programa(int);
int iniciar_servidor();
struct addrinfo* crear_addrinfo();
void mostrar_mensaje_previa_conexion_con(char *, char *, int);
void validar_comunicacion(int, char*);
void cerrar_socket_y_terminar(int);
void mandar_handshake_a(char *, int, enum PROCESO);
void recibir_handshake_cliente(int);
void construir_lista_seeds();
void apagar_semaforos();
enum MENSAJES notificar_escrituras_en_memoria_LFS(int);
void separar_path_pasado_por_parametro(char **, char **, char ** parametros);
void recibir_handshakes(int);

/*Globales inotify para liberar recursos*/
int watch_descriptor,conf_fd;
char * path_archivo_configuracion;
t_path_archivo_conf *ruta_archivo_conf;

#endif /* UTILIDADES_H_ */
