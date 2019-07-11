/*
 * actualizarConfig.c
 *
 *  Created on: 9 jul. 2019
 *      Author: utnso
 */


#include "actualizarConfig.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) + 200 )
#define BUF_LEN ( 1024 * EVENT_SIZE )

int lanzarMonitoreadorDeArchivo(){
	logger(escribir_loguear, l_info,"Iniciando hilo monitoreador de archivo");
	int resultadoDeCrearHilo = pthread_create( &threadMonitoreadorDeArchivos, NULL,
			funcionMonitorDeArchivos, "Hilo monitor de archivos");
	pthread_detach(threadMonitoreadorDeArchivos);
	if(resultadoDeCrearHilo){
		log_error(LOG_KERNEL,"Error al crear el hilo monitor de archivos, return code: %d",
				resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
	}else{
		logger(escribir_loguear, l_info,"El hilo monitor de archivos se creo exitosamente");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

int funcionMonitorDeArchivos(){
	char buffer[BUF_LEN];
	int offset;

	int file_descriptor;
	char* directorioDeConfig = string_new();
	string_append(&directorioDeConfig, "/home/utnso/Escritorio/tp-2019-1c-EndGame/Kernel/Debug/Configuracion");
	logger(escribir_loguear, l_info,"El directorio sobre el que va a trabajar el inotify es %s", directorioDeConfig);

	file_descriptor = inotify_init();
	if(file_descriptor<0){
		perror("inotify_init");
		}
	int watch_descriptor = inotify_add_watch(file_descriptor, directorioDeConfig, IN_MODIFY | IN_CREATE | IN_DELETE);
	int length = read(file_descriptor, buffer, BUF_LEN);
	if (length < 0) {
		perror("read");
		}
	offset = 0;
	while((offset<length)){
		logger(escribir_loguear, l_info,"Aguardando por una modificacion");
		struct inotify_event *event = (struct inotify_event *) &buffer[offset];
		// El campo "len" nos indica la longitud del tamaño del nombre
		if(event->len){
			if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR) {
					logger(escribir_loguear, l_info,"El directorio %s fue modificado", event->name);
				}else{
					//log_error(LOG_KERNEL,"1 El archivo %s fue modificado", event->name);
					if(strcmp(event->name,"kernel.config")==0){
						logger(escribir_loguear, l_info,"El archivo %s fue modificado", event->name);
						reloadConfig();
						}
					}
				}
			}
		length = read(file_descriptor, buffer, BUF_LEN);
		//offset += sizeof (struct inotify_event) + event->len;
		}
	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);
	logger(escribir_loguear, l_info,"Finalizando funcion funcionMonitorDeArchivos");
	return EXIT_SUCCESS;

}


