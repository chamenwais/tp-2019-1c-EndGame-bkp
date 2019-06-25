/*
 * dump.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "dump.h"

int dump(char* nombreDeLaTabla){
	char* nombreDelArchivoTemp;
	int sizeDelTemporal = 0;
	int punteroDelBloque=0;
	int bloqueActual=-1;
	bool hayBloquesLibres=true;
	char* cadenaFinal;
	char* bloques;

	bool esMiNodo(void* nodo) {
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		}

	void agregarALaCadenaFinal(void* nodo){
		char* cadenaAInsertar = string_new();
		char* auxitoa = string_itoa(((tp_nodoDeLaTabla)nodo)->timeStamp);
		string_append(&cadenaAInsertar, auxitoa);
		free(auxitoa);
		string_append(&cadenaAInsertar, ";");
		auxitoa=string_itoa(((tp_nodoDeLaTabla)nodo)->key);
		string_append(&cadenaAInsertar, auxitoa);
		free(auxitoa);
		string_append(&cadenaAInsertar, ";");
		string_append(&cadenaAInsertar, ((tp_nodoDeLaTabla)nodo)->value);
		string_append(&cadenaAInsertar, "\n");
		string_append(&cadenaFinal, cadenaAInsertar);
		free(cadenaAInsertar);
	}

	int insertarCadenaEnLosBloques(){
		bool termine=false;
		bool esElPrimero=true;
		char*auxitoa;
		while(termine==false){
			pthread_mutex_lock(&mutexBitmap);
			bloqueActual=obtenerBloqueLibreDelBitMap();
			ocuparBloqueDelBitmap(bloqueActual);
			pthread_mutex_unlock(&mutexBitmap);
			bajarADiscoBitmap();
			if(bloqueActual==-1){
				log_error(LOGGERFS,"Alerta, no hay mas bloques libres!!!!!");
				hayBloquesLibres=false;
				return NO_HAY_MAS_BLOQUES_EN_EL_FS;
			}else{

				int length=0;
				if(metadataDelFS.blockSize<=string_length(cadenaFinal)){
					//lleno todo el bloque
					length=metadataDelFS.blockSize;
				}else{
					//lleno parte del bloque, es el ultimo
					length=string_length(cadenaFinal);
					}
				char* data=string_substring(cadenaFinal, 0, length);
				if(metadataDelFS.blockSize<=string_length(cadenaFinal)){
					//no es el ultimo a insertar
					char* aux=string_substring_from(cadenaFinal, length);
					free(cadenaFinal);
					cadenaFinal=aux;
				}else{
					//es el ultimo, ya termine
					free(cadenaFinal);
					termine=true;
					}
				log_info(LOGGERFS,"Guardando \n%s\n en el archivo de bloque %d",
						data, bloqueActual);
				insertarDatosEnElBloque(data, bloqueActual);
				free(data);
				//Actulizo la cadena de bloques
				if(esElPrimero){
					esElPrimero=false;
				}else{
					string_append(&bloques, ",");
					}
				auxitoa=string_itoa(bloqueActual);
				string_append(&bloques, auxitoa);
				free(auxitoa);
				}
			}
		return EXIT_SUCCESS;
		}
	pthread_mutex_lock(&mutexDeLaMemtable);
	tp_nodoDeLaMemTable nodoDeLaMem = list_find(memTable,esMiNodo);
	if(nodoDeLaMem==NULL){
		log_info(LOGGERFS,"No hay nada para dumpear en %s", nombreDeLaTabla);
		return DUMP_CORRECTO;
		}
	bloques = string_new(); //va a tener el formato: [2,3,7,10]
	string_append(&bloques, "[");
	log_info(LOGGERFS,"Duempeando la tabla %s",nombreDeLaTabla);
	log_info(LOGGERFS,"Block size del FS %d",metadataDelFS.blockSize);
	log_info(LOGGERFS,"Voy a dumpear la tabla", nodoDeLaMem->nombreDeLaTabla);
	//t_metadataDeLaTabla metadataDeLaTabla = obtenerMetadataDeLaTabla(nodoDeLaMem->nombreDeLaTabla);
	nombreDelArchivoTemp=buscarNombreDelTempParaDumpear(nodoDeLaMem->nombreDeLaTabla);
	cadenaFinal = string_new();
	//meto todo en un gran bodoque (en cadena cadenaFinal)
	list_iterate(nodoDeLaMem->listaDeDatosDeLaTabla,agregarALaCadenaFinal);
	sizeDelTemporal=string_length(cadenaFinal);
	log_info(LOGGERFS,"Cadena final a insertar: \n%s",cadenaFinal);
	insertarCadenaEnLosBloques();
	string_append(&bloques, "]");
	crearElTemp(nombreDelArchivoTemp, bloques, sizeDelTemporal);
	log_info(LOGGERFS,"Tabla %s dumpeada, en el temp %s",nombreDeLaTabla, nombreDelArchivoTemp);
	free(nombreDelArchivoTemp);
	//setearEstadoDeFinalizacionDeDumpeo(nombreDeLaTabla, true);
	liberarMemoriaDelNodo(nombreDeLaTabla);
	free(bloques);
	pthread_mutex_unlock(&mutexDeLaMemtable);
	if(hayBloquesLibres){
		log_info(LOGGERFS,"Dump correcto");
		return DUMP_CORRECTO;
	}else{
		log_error(LOGGERFS,"Se hizo el dump pero en em medio del proceso se acabaron los bloques libres, no se puede asegurar la consistencia de los datos");
		return NO_HAY_MAS_BLOQUES_EN_EL_FS;
	}
}

int liberarMemoriaDelNodo(char* liberarMemoriaDelNodo){
	//implementar
	//no hace falta, al final borro toda la memtable
	return EXIT_SUCCESS;
}

int lanzarDumps(){
	log_info(LOGGERFS,"Iniciando hilo de consola");
	int resultadoDeCrearHilo = pthread_create( &threadDumps, NULL,
			funcionHiloDump, "Hilo dump");
	if(resultadoDeCrearHilo){
		log_error(LOGGERFS,"Error al crear el hilo del dump, return code: %d",
				resultadoDeCrearHilo);
		exit(EXIT_FAILURE);
	}else{
		log_info(LOGGERFS,"El hilo de dump se creo exitosamente");
		return EXIT_SUCCESS;
		}
	return EXIT_SUCCESS;
}

void funcionHiloDump(void *arg){

	void dumpearAEseNodo(void* nodo) {
		dump(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla);
		}

	int tiempoDeSleep;
	while(!obtenerEstadoDeFinalizacionDelSistema()){
		tiempoDeSleep=obtenerTiempoDump();
		sleep(tiempoDeSleep);
		log_trace(LOGGERFS,"Iniciando un dumpeo");
		pthread_mutex_lock(&mutexDeDump);
		list_iterate(memTable, dumpearAEseNodo);
		vaciarMemTable();
		memTable=list_create();
		pthread_mutex_unlock(&mutexDeDump);
		log_trace(LOGGERFS,"Dumpeo finalizado");
		}
	log_info(LOGGERFS,"Finalizando hilo de dumpeo");
	return;
	}

int insertarDatosEnElBloque(char* cadenaAInsertar,int bloqueActual){
	//le pasas un numero de bloque y una cadena con los datos a insertar y los manda
	//a continuacion de lo q haya
	char* nombreDelArchivoDeBloque=string_new();
	string_append(&nombreDelArchivoDeBloque, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivoDeBloque, "/Blocks/");
	char* auxitoa=string_itoa(bloqueActual);
	string_append(&nombreDelArchivoDeBloque, auxitoa);
	free(auxitoa);
	string_append(&nombreDelArchivoDeBloque, ".bin");
	FILE* archivo=fopen(nombreDelArchivoDeBloque,"a");
	log_info(LOGGERFS,"Guardando %s en el archivo %s", cadenaAInsertar, nombreDelArchivoDeBloque);
	fprintf(archivo,"%s",cadenaAInsertar);
	free(nombreDelArchivoDeBloque);
	fclose(archivo);
	return EXIT_SUCCESS;
}

int crearElTemp(char* nombreDelArchivo,char* bloques,int size){
	 //bloques tiene q venir formateado de la forma: [2,3,7,10]
	 log_info(LOGGERFS,"Voy a crear el archivo %s", nombreDelArchivo);
	 FILE * archivoTemp = fopen(nombreDelArchivo,"w");
	 fclose(archivoTemp);
	 t_config* configuracion = config_create(nombreDelArchivo);
	 char* auxitoa=string_itoa(size);
	 config_set_value(configuracion, "SIZE", auxitoa);
	 free(auxitoa);
	 config_set_value(configuracion, "BLOCKS", bloques);
	 config_save(configuracion);
	 config_destroy(configuracion);
	 log_info(LOGGERFS,"Archivo %s creado, de tamaño %d, para los bloques %s",
			 nombreDelArchivo, size, bloques);
	 return EXIT_SUCCESS;
}

char* buscarNombreDelTempParaDumpear(char* nombreDeLaTabla){
	//me da el nombre del siguiente archivo para dumpear que no este usado
	char* aux=string_new();
	string_append(&aux, configuracionDelFS.puntoDeMontaje);
	string_append(&aux, "/Tables/");
	string_append(&aux, nombreDeLaTabla);
	string_append(&aux, "/");
	string_append(&aux, nombreDeLaTabla);
	char* pathDelTemp;
	//FILE* archivo;
	bool encontrado=true;
	for(int i=1;encontrado==true;i++){
		pathDelTemp=string_new();
		string_append(&pathDelTemp, aux);
		char * auxitoa=string_itoa(i);
		string_append(&pathDelTemp, auxitoa);
		free(auxitoa);
		string_append(&pathDelTemp, ".tmp");
		log_info(LOGGERFS,"Viendo si existe el archivo %s", pathDelTemp);
		encontrado=existeElArchivo(pathDelTemp);
		if(encontrado==true){
			log_error(LOGGERFS,"El nombre %s ya esta usado en otro dump", pathDelTemp);
		}else{
			log_info(LOGGERFS,"Encontrado el nombre del proximo archivo temp: %s para el dump", pathDelTemp);
			}
		}
	log_info(LOGGERFS,"Encontrado el nombre del proximo archivo temp: %s para el dump", pathDelTemp);
	free(aux);
	return pathDelTemp;
}
