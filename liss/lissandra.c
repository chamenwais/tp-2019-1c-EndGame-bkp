/*
 ============================================================================
 Name        : lissandra.c
 Author      : andres
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "lissandra.h"

int main(int argc,char** argv) {
	if(inicializarVariablesGlobales()==EXIT_SUCCESS){
	inicializarLogDelFS();
	obtenerPathDeMontajeDelPrograma(argc,argv);
	//configurar_signals();
	if(levantarConfiguracionInicialDelFS()==EXIT_SUCCESS){
	if(levantarMetadataDelFS()==EXIT_SUCCESS){
	if(levantarBitMap()==EXIT_SUCCESS){
	if(inicializarEstructuras()==EXIT_SUCCESS){
	if(lanzarConsola()==EXIT_SUCCESS){
	if(lanzarServer()==EXIT_SUCCESS){
	if(lanzarDumps()==EXIT_SUCCESS){
	if(lanzarCompactador()==EXIT_SUCCESS){
	if(lanzarMonitoreadorDeArchivo()==EXIT_SUCCESS){
	if(esperarPorHilos()==EXIT_SUCCESS)
		liberarRecursos();
	}}}}}}}}}}
	return EXIT_SUCCESS;
}

void configurar_signals(void) {
	struct sigaction signal_struct;
	signal_struct.sa_handler = captura_signal;
	signal_struct.sa_flags = 0;

	sigemptyset(&signal_struct.sa_mask);

	sigaddset(&signal_struct.sa_mask, SIGINT);
	if (sigaction(SIGINT, &signal_struct, NULL) < 0) {
		log_error(LOGGERFS, "SIGACTION error");
	}
}

void captura_signal(int signo){

    if(signo == SIGINT)
    {
    	log_info(LOGGERFS,"[Finalizando liss]");
		setearEstadoDeFinalizacionDelSistema(true);
    }
}

