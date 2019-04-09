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
void captura_sigpipe(int);
void logger(int tipo_esc, int tipo_log, const char* mensaje, ...);
void validar_apertura_archivo_configuracion();
void obtener_valor_configuracion(char*, void(*f)(void));
void obtener_puerto_escucha();
void obtener_ip_filesystem();
void obtener_puerto_filesystem();
void obtener_tamanio_memoria();
void obtener_numero_memoria();
void escribir_por_pantalla(int tipo_esc, int tipo_log, char* console_buffer,
		char* log_colors[8], char* msj_salida);
void definir_nivel_y_loguear(int tipo_esc, int tipo_log, char* msj_salida);
void terminar_programa();

#endif /* UTILIDADES_H_ */
