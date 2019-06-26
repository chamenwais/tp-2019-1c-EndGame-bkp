/*
 * actualizarConfig.c
 *
 *  Created on: 26 jun. 2019
 *      Author: utnso
 */

#include "actualizarConfig.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) + 200 )
#define BUF_LEN ( 1024 * EVENT_SIZE )

int lanzarMonitoreadorDeArchivo(){
	log_info(LOGGERFS,"Iniciando hilo de monitoreador de archivo");
	int resultadoDeCrearHilo = pthread_create( &threadMonitoreadorDeArchivos, NULL,
			funcionMonitorDeArchivos, "Hilo monitor de archivos");
	if(resultadoDeCrearHilo){
		log_error(LOGGERFS,"Error al crear el hilo monitor de archivos, return code: %d",
				resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
	}else{
		log_info(LOGGERFS,"El hilo monitor de arhivos se creo exitosamente");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

int funcionMonitorDeArchivos(){
	char buffer[BUF_LEN];
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) {
		perror("inotify_init");
		}
	log_info(LOGGERFS,"El directorio sobre el que va a trabajar el inotify es %s", configuracionDelFS.puntoDeMontaje);
	int watch_descriptor = inotify_add_watch(file_descriptor, configuracionDelFS.puntoDeMontaje, IN_MODIFY | IN_CREATE | IN_DELETE);
	int length = read(file_descriptor, buffer, BUF_LEN);
	if (length < 0) {
		perror("read");
		}
	int offset = 0;
	while(offset < length){
		struct inotify_event *event = (struct inotify_event *) &buffer[offset];
		// El campo "len" nos indica la longitud del tamaño del nombre
		if(event->len){
			if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR) {
					log_info(LOGGERFS,"El directorio %s fue modificado", event->name);
				}else{
					log_info(LOGGERFS,"El archivo %s fue modificado", event->name);
					reloadConfig();
					}
				}
			}
		offset += sizeof (struct inotify_event) + event->len;
		}

	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);
	log_info(LOGGERFS,"Finalizando funcion funcionMonitorDeArchivos");
	return EXIT_SUCCESS;

}
