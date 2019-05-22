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
