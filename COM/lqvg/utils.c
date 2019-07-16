/*
 * utils.c
 *
 *  Created on: 22 may. 2019
 *      Author: utnso
 */

#include "utils.h"

char * obtener_value_a_insertar(char * comando_insert){
	//Puntero a la primer coincidencia con las comilla doble "
	char * p_first=strchr(comando_insert,'"');
	//Puntero a la última coincidencia con las comilla doble "
	char * p_last=strrchr(comando_insert,'"');
	//Si no hay ninguna comilla, no se interpreta value. Devuelve NULL
	if(p_first==NULL){
		return NULL;
	}
	//Si los punteros son iguales, hay solo una comilla. Devuelve NULL
	if(p_first==p_last){
		return NULL;
	}
	//Cuento cuántos caracteres hay dentro de las comillas y reservo memoria
	int i=0;
	while(p_first[i+1]!=p_last[0]){
		i++;
	}
	char * value_a_insertar=malloc(i+1);
	//Desde una posición después de la primer coincidencia de comillas, voy copiando
	//caracter a caracter
	i=1;
	while(p_first[i]!=p_last[0]){
		value_a_insertar[i-1]=p_first[i];
		i++;
	}
	//Coloco el caracter centinela \0 y devuelvo
	value_a_insertar[i-1]='\0';
	return value_a_insertar;
}

t_list * obtener_lista_lineas_desde_archivo(char * path_archivo){
	t_list * lista_lineas=list_create();
	FILE* archivo;
	if ((archivo=fopen(path_archivo,"r"))==NULL){
		list_destroy(lista_lineas);
		//Si no pude leer el archivo devuelvo NULL
		return NULL;
	}
	ssize_t linea_size;
	size_t linea_buf_size = 0;
	char *linea = NULL;
	char * linea_sucia=NULL;
	linea_size = getline(&linea_sucia, &linea_buf_size, archivo);
	while(!feof(archivo)&&linea_size >= 0){
		linea=(string_split(linea_sucia,"\n"))[0];
		list_add(lista_lineas, linea);
		linea_buf_size = 0;
		linea = NULL;
		linea_sucia=NULL;
		linea_size = getline(&linea_sucia, &linea_buf_size, archivo);
	}
	fclose(archivo);
	return lista_lineas;
}

t_list * obtener_lista_linea(char * linea_original){
	t_list * lista_linea=list_create();

	char *linea = NULL;
	linea=(string_split(linea_original,"\n"))[0];
	list_add(lista_linea, linea);

	return lista_linea;
}

char* conocer_ip_propia() {
	char* ip_address = malloc(15);
	int fd;
	struct ifreq ifr;
	/*AF_INET - to define network interface IPv4*/
	/*Creating soket for it.*/
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	/*AF_INET - to define IPv4 Address type.*/
	ifr.ifr_addr.sa_family = AF_INET;
	/*docker0 - define the ifr_name - port name
	 where network attached.*/
	memcpy(ifr.ifr_name, "docker0", IFNAMSIZ - 1);
	/*Accessing network interface information by
	 passing address using ioctl.*/
	ioctl(fd, SIOCGIFADDR, &ifr);
	/*closing fd*/
	close(fd);
	/*Extract IP Address*/
	strcpy(ip_address, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
	return ip_address;
}

double obtenerTimestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec)*1000+((unsigned long long)tv.tv_usec)/1000);
	double a = result;
	log_info(LOGGERFS,"Timestamo obtenido: %lf", a);
	return a;
}
