/*
 * Consola.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "Consola.h"

void loguear_cant_incorrecta_params(char *nombre_comando) {
	logger(escribir_loguear, l_error,
			"La cantidad de parámetros es incorrecta para el comando %s",nombre_comando);
}

void loguear_comienzo_ejecucion_sentencia(char *nombre_comando) {
	logger(escribir_loguear, l_trace, "Se ejecutará la sentencia %s",nombre_comando);
}

void consola_select(char** comandos){
	int cant_param_correcta = 0;
	char *nombre_tabla = NULL;
	char *key = NULL;
	obtener_dos_parametros(comandos, &nombre_tabla, &key);
	cant_param_correcta=validar_parametro_consola(&nombre_tabla);
	cant_param_correcta=validar_parametro_consola(&key);
	if(cant_param_correcta<0){
		loguear_cant_incorrecta_params(_SELECT);
		return;
	}

	loguear_comienzo_ejecucion_sentencia(_SELECT);
	//TODO hacer algo

	//Limpio el nombre_tabla
	limpiar_parametro(nombre_tabla);

	//Limpio la key
	limpiar_parametro(key);

}

void consola_insert(char** comandos){
	int cant_param_correcta = 0;
	char *nombre_tabla = NULL;
	char *key = NULL;
	char *value = NULL;
	char *timestamp = NULL;
	obtener_cuatro_parametros(comandos, &nombre_tabla, &key, &value, &timestamp);
	cant_param_correcta=validar_parametro_consola(&nombre_tabla);
	cant_param_correcta=validar_parametro_consola(&key);
	cant_param_correcta=validar_parametro_consola(&value);
	cant_param_correcta=validar_parametro_consola(&timestamp);
	if(cant_param_correcta<0){
		loguear_cant_incorrecta_params(_INSERT);
		return;
	}

	loguear_comienzo_ejecucion_sentencia(_INSERT);
	//TODO hacer algo

	//Limpio el nombre_tabla
	limpiar_parametro(nombre_tabla);

	//Limpio la key
	limpiar_parametro(key);

	//Limpio la value
	limpiar_parametro(value);

	//Limpio la timestamp
	limpiar_parametro(timestamp);

}

void consola_create(char** comandos){
	int cant_param_correcta = 0;
	char *nombre_tabla = NULL;
	char *tipo_consistencia = NULL;
	char *numero_particiones = NULL;
	char *compaction_time = NULL;
	obtener_cuatro_parametros(comandos, &nombre_tabla, &tipo_consistencia, &numero_particiones, &compaction_time);
	cant_param_correcta=validar_parametro_consola(&nombre_tabla);
	cant_param_correcta=validar_parametro_consola(&tipo_consistencia);
	cant_param_correcta=validar_parametro_consola(&numero_particiones);
	cant_param_correcta=validar_parametro_consola(&compaction_time);
	if(cant_param_correcta<0){
		loguear_cant_incorrecta_params(_CREATE);
		return;
	}

	loguear_comienzo_ejecucion_sentencia(_CREATE);
	//TODO hacer algo

	//Limpio el nombre_tabla
	limpiar_parametro(nombre_tabla);

	//Limpio la tipo_consistencia
	limpiar_parametro(tipo_consistencia);

	//Limpio la numero_particiones
	limpiar_parametro(numero_particiones);

	//Limpio la compaction_time
	limpiar_parametro(compaction_time);

}

void consola_describe(char** comandos){
	int cant_param_correcta = 0;
	char *nombre_tabla = NULL;
	obtener_un_parametro(comandos, &nombre_tabla);
	cant_param_correcta=validar_parametro_consola(&nombre_tabla);
	if(cant_param_correcta<0){
		loguear_cant_incorrecta_params(_DESCRIBE);
		return;
	}

	loguear_comienzo_ejecucion_sentencia(_DESCRIBE);
	//TODO hacer algo

	//Limpio el nombre_tabla
	limpiar_parametro(nombre_tabla);

}

void consola_drop(char** comandos){
	int cant_param_correcta = 0;
	char *nombre_tabla = NULL;
	obtener_un_parametro(comandos, &nombre_tabla);
	cant_param_correcta=validar_parametro_consola(&nombre_tabla);
	if(cant_param_correcta<0){
		loguear_cant_incorrecta_params(_DROP);
		return;
	}

	loguear_comienzo_ejecucion_sentencia(_DROP);
	//TODO hacer algo

	//Limpio el nombre_tabla
	limpiar_parametro(nombre_tabla);

}

void consola_journal(){

	loguear_comienzo_ejecucion_sentencia(_JOURNAL);
	//TODO hacer algo

}

int consola_obtener_key_comando(char** comandos)
{
	int key = -1;
	char *comando=comandos[0];

	if(comando == NULL)
		return key;

	if(string_equals_ignore_case(comando, "select")){
		consola_select(comandos);
		return 0;
	}
	if(string_equals_ignore_case(comando, "insert")){
		consola_insert(comandos);
		return 0;
	}
	if(string_equals_ignore_case(comando, "create")){
		consola_create(comandos);
		return 0;
	}
	if(string_equals_ignore_case(comando, "describe")){
		consola_describe(comandos);
		return 0;
	}
	if(string_equals_ignore_case(comando, "drop")){
		consola_drop(comandos);
		return 0;
	}
	if(string_equals_ignore_case(comando, "journal")){
		consola_journal();
		return 0;
	}

	return key;
}

void limpiar_elementos_comando(int cant_parametros, char** elementos) {
	for (int i = 0; i > cant_parametros; i++) {
		free(elementos[i]);
	}
}

void obtener_un_parametro(char** comandos, char** parametro1){
	int j=0;

	while(comandos[j])
	{
		switch(j)
		{
			case 1:
				*parametro1 = string_duplicate(comandos[j]);
				break;
		}

		j++;
	}

	limpiar_elementos_comando(j, comandos);

	free(comandos);
}

void obtener_dos_parametros(char** comandos, char** parametro1, char** parametro2){
	int j=0;

	while(comandos[j])
	{
		switch(j)
		{
			case 1:
				*parametro1 = string_duplicate(comandos[j]);
				break;
			case 2:
				*parametro2 = string_duplicate(comandos[j]);
				break;
		}

		j++;
	}

	limpiar_elementos_comando(j, comandos);

	free(comandos);
}

void obtener_cuatro_parametros(char** comandos, char** parametro1,
		char** parametro2, char** parametro3, char** parametro4){
	int j=0;

	while(comandos[j])
	{
		switch(j)
		{
			case 1:
				*parametro1 = string_duplicate(comandos[j]);
				break;
			case 2:
				*parametro2 = string_duplicate(comandos[j]);
				break;
			case 3:
				*parametro3 = string_duplicate(comandos[j]);
				break;
			case 4:
				*parametro4 = string_duplicate(comandos[j]);
				break;
		}

		j++;
	}

	limpiar_elementos_comando(j, comandos);

	free(comandos);
}

int validar_parametro_consola(char ** parametro){
	if(*parametro!=NULL){
		return 0;
	}
	*parametro="-1";
	return -1;
}

void limpiar_parametro(char* parametro) {
	//Limpio el parametro
	if (parametro != NULL && !string_equals_ignore_case(parametro, "-1")) {
		free(parametro);
		parametro = NULL;
	}
}

int consola_derivar_comando(char * buffer){

	int comando_key;
	int res = 0;
	char** comandos = string_n_split(buffer,10," ");
	if(comandos[0]==NULL){
		logger(escribir_loguear, l_error,"No pude interpretar el comando, proba de nuevo");
		return res;
	}

	// Obtiene la clave del comando a ejecutar y manda a ejecutarlo
	comando_key = consola_obtener_key_comando(comandos);

	if(comando_key<0){
		logger(escribir_loguear, l_error,"No conozco ese comando, proba de nuevo");
		return res;
	}

	return res;
}

int consola_leer_stdin(char *read_buffer, size_t max_len)
{
	char c = '\0';
	int i = 0;

	memset(read_buffer, 0, max_len);

	ssize_t read_count = 0;
	ssize_t total_read = 0;

	do{
		read_count = read(STDIN_FILENO, &c, 1);

		if (read_count < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
			logger(escribir_loguear,l_error,"Error en read() desde STDIN");
		  return -1;
		}
		else if (read_count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		  break;
		}
		else if (read_count > 0) {
		  total_read += read_count;

		  read_buffer[i] = c;
		  i++;

		  if (total_read > max_len) {
			//log_info(logger,"Message too large and will be chopped. Please try to be shorter next time.\n");
			fflush(STDIN_FILENO);
			break;
		  }
		}

	}while ((read_count > 0) && (total_read < max_len) && (c != '\n'));

	size_t len = strlen(read_buffer);
	if (len > 0 && read_buffer[len - 1] == '\n'){
		read_buffer[len - 1] = '\0';
	}

	//log_info(logger,"Read from stdin %zu bytes. Let's prepare message to send.\n", strlen(read_buffer));

	return 0;
}

