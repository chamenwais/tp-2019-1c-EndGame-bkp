/*
 ============================================================================
 Name        : lissandra.c
 Author      : andres
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "lissandra.h"

int main(int argc,char** argv) {
	if(inicializarVariablesGlobales()==EXIT_SUCCESS){
	inicializarLogDelFS();
	obtenerPathDeMontajeDelPrograma(argc,argv);
	if(levantarConfiguracionInicialDelFS()==EXIT_SUCCESS){
	if(levantarMetadataDelFS()==EXIT_SUCCESS){
	if(lanzarConsola()==EXIT_SUCCESS){
	if(lanzarCompactador()==EXIT_SUCCESS){
	if(esperarAQueTermineLaConsola()==EXIT_FAILURE);
		liberarRecursos();
	}}}}}
	return EXIT_SUCCESS;
}
