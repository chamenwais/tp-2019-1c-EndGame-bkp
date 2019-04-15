/*
 * Consola.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "Consola.h"

void consola_select(){
	//hacer
}

void consola_insert(){
	//hacer
}

void consola_create(){
	//hacer
}

void consola_describe(){
	//hacer
}

void consola_drop(){
	//hacer
}

void consola_journal(){
	//hacer
}

int consola_obtener_key_comando(char* comando)
{
	int key = -1;

	if(comando == NULL)
		return key;

	if(!strcmp(comando, "select")){
		key = SELECT;
	}
	if(!strcmp(comando, "insert")){
		key = INSERT;
	}
	if(!strcmp(comando, "create")){
		key = CREATE;
	}
	if(!strcmp(comando, "describe")){
		key = DESCRIBE;
	}
	if(!strcmp(comando, "drop")){
		key = DROP;
	}
	if(!strcmp(comando, "journal")){
		key = JOURNAL;
	}else{
		logger(escribir_loguear, l_error,"No conozco ese comando, proba de nuevo\n");
	}

	return key;
}

void consola_obtener_parametros(char* buffer, char** comando, char** parametro1, char** parametro2){
	char** comandos;
	int i,j=0;

	comandos = string_n_split(buffer,3," ");

	while(comandos[j])
	{
		switch(j)
		{
			case 0:
				*comando = comandos[j];
				break;
			case 1:
				*parametro1 = comandos[j];
				break;
			case 2:
				*parametro2 = comandos[j];
				break;
		}

		j++;
	}

	for(i=0;i>j;i++)
	{
		//log_info(logger,"parte %d: %s\n", j,comandos[j]);
		free(comandos[j]);
	}

	free(comandos);
}


void validar_parametro_consola(char ** parametro){
	if(*parametro!=NULL){
		return;
	}
	*parametro="-1";
}

int consola_derivar_comando(char * buffer){

	int comando_key;
	char *comando = NULL;
	char *parametro1 = NULL;
	char *parametro2 = NULL;
	int res = 0;

	// Separa la linea de consola en comando y sus parametros
	consola_obtener_parametros(buffer, &comando, &parametro1, &parametro2);

	// Obtiene la clave del comando a ejecutar para el switch
	comando_key = consola_obtener_key_comando(comando);

	validar_parametro_consola(&parametro1);
	validar_parametro_consola(&parametro2);

	switch(comando_key){
		case SELECT:
			consola_select();
			break;
		case INSERT:
			consola_insert();
			break;
		case CREATE:
			consola_create();
			break;
		case DESCRIBE:
			consola_describe();
			break;
		case DROP:
			consola_drop();
			break;
		case JOURNAL:
			consola_journal();
			break;
	}

	//Limpio el parametro 1
		if(parametro1 != NULL)
		{
			free(parametro1);
			parametro1 = NULL;
		}

		//Limpio el parametro 2
		if(parametro2 != NULL)
		{
			free(parametro2);
			parametro2 = NULL;
		}

		free(comando);

	return res;
}

void *consola() {

	int res = 0;
	char *buffer = NULL;

	logger(escribir_loguear,l_info,"\nAbriendo consola...\n");

	while(TRUE){

		//Trae la linea de consola
		buffer = readline(">");
		//consola_leer_stdin(buffer, MAX_LINEA);

		res = consola_derivar_comando(buffer);

		free(buffer);

		//Sale de la consola con exit
		if(res)
			break;
	}

	pthread_exit(0);
	return 0;
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

