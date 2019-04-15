/*
 * Utilidades.c
 *
 *  Created on: 8 abr. 2019
 *      Author: utnso
 */
#include "Utilidades.h"

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/memoria.log", "Memoria", false , LOG_LEVEL_DEBUG);

	logger(escribir_loguear, l_info,"Se comenzo a iniciar el proceso memoria");
}

void iniciar_config(int cantidad_parametros, char ** parametros) {
	if (cantidad_parametros>1){
		g_config = config_create(parametros[1]);
	} else {
		g_config = config_create("../memoria.config");
	}
	validar_apertura_archivo_configuracion();

}

void validar_apertura_archivo_configuracion() {
	if (g_config == NULL) {
		logger(escribir_loguear,l_error, "No encontré el archivo de configuración");
		log_destroy(g_logger);
		exit(EXIT_FAILURE);
	}
}

void obtener_valor_configuracion(char* clave, void(*obtener)(void)){
	if (config_has_property(g_config, clave)){
		obtener();
	}
}

void obtener_puerto_escucha() {
	PUERTO_ESCUCHA = config_get_int_value(g_config,
			CLAVE_CONFIG_PUERTO_ESCUCHA);
	logger(escribir_loguear, l_debug, "Se obtuvo configuración 'Puerto de escucha': %d",PUERTO_ESCUCHA);
}

void obtener_ip_filesystem(){
	IP_FILESYSTEM=config_get_string_value(g_config, CLAVE_CONFIG_IP_FILESYSTEM);
	logger(escribir_loguear, l_debug, "Se obtuvo configuración 'IP de filesystem': %s",IP_FILESYSTEM);
}

void obtener_puerto_filesystem(){
	PUERTO_FILESYSTEM=config_get_int_value(g_config, CLAVE_CONFIG_PUERTO_FILESYSTEM);
	logger(escribir_loguear, l_debug, "Se obtuvo configuración 'Puerto de filesystem': %d",PUERTO_FILESYSTEM);
}

void obtener_tamanio_memoria(){
	TAMANIO_MEMORIA=config_get_int_value(g_config, CLAVE_CONFIG_TAMANIO_MEMORIA);
	logger(escribir_loguear, l_debug, "Se obtuvo configuración 'Tamanio memoria': %d",TAMANIO_MEMORIA);
}

void obtener_numero_memoria(){
	NUMERO_MEMORIA=config_get_int_value(g_config, CLAVE_CONFIG_NUMERO_MEMORIA);
	logger(escribir_loguear, l_debug, "Se obtuvo configuración 'Numero de memoria': %d",NUMERO_MEMORIA);
}

void leer_config(void) {
	logger(escribir_loguear, l_info,"Cargando archivo de configuración...");
	obtener_valor_configuracion(CLAVE_CONFIG_PUERTO_ESCUCHA, obtener_puerto_escucha);
	obtener_valor_configuracion(CLAVE_CONFIG_IP_FILESYSTEM, obtener_ip_filesystem);
	obtener_valor_configuracion(CLAVE_CONFIG_PUERTO_FILESYSTEM, obtener_puerto_filesystem);
	obtener_valor_configuracion(CLAVE_CONFIG_TAMANIO_MEMORIA, obtener_tamanio_memoria);
	obtener_valor_configuracion(CLAVE_CONFIG_NUMERO_MEMORIA, obtener_numero_memoria);
	logger(escribir_loguear, l_info,"Se cargó  archivo de configuración exitosamente");
}

void configurar_signals(void) {
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_sigpipe;
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

void captura_sigpipe(int signo){

    if(signo == SIGINT)
    {
    	logger(escribir_loguear, l_warning,"Finalizando proceso memoria...");
    	terminar_programa();
    	exit(EXIT_FAILURE);
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
			log_info(g_logger, msj_salida);
		} else if (tipo_log == l_warning) {
			log_warning(g_logger, msj_salida);
		} else if (tipo_log == l_error) {
			log_error(g_logger, msj_salida);
		} else if (tipo_log == l_debug) {
			log_debug(g_logger, msj_salida);
		} else if (tipo_log == l_trace) {
			log_trace(g_logger, msj_salida);
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

void terminar_programa(){
	logger(escribir_loguear, l_warning,"Has elegido finalizar la memoria %d. Bye.", NUMERO_MEMORIA);
	log_destroy(g_logger);
	config_destroy(g_config);
}
