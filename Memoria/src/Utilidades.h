/*
 * Utilidades.h
 *
 *  Created on: 8 abr. 2019
 *      Author: utnso
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include "Contexto.h"

void iniciar_logger(void);
void iniciar_config(int, char **);
void leer_config(void);
void configurar_signals(void);
void captura_signal(int);
void logger(int tipo_esc, int tipo_log, const char* mensaje, ...);
void validar_apertura_archivo_configuracion();
void obtener_valor_configuracion(char*, void(*f)(void));
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
void recibir_handshake_kernel(int);

#endif /* UTILIDADES_H_ */
