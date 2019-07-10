/*
 * funcionesAuxiliares.c
 *
 *  Created on: 24 abr. 2019
 *      Author: utnso
 */

#include "funcionesAuxiliares.h"

int crearDirectorioParaLaTabla(char* nombreDeLaTabla){
	// Crear el directorio para dicha tabla.
	char* directorioDeLaTabla=string_new();
	string_append(&directorioDeLaTabla, configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeLaTabla, "/Tables");
	mkdir(directorioDeLaTabla,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	string_append(&directorioDeLaTabla, "/");
	string_append(&directorioDeLaTabla, nombreDeLaTabla);
	mkdir(directorioDeLaTabla,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGERFS,"Directorio %s creado", directorioDeLaTabla);
	free(directorioDeLaTabla);
	return EXIT_SUCCESS;
}

int crearMetadataParaLaTabla(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion){
	// Crear el archivo Metadata asociado al mismo.
	// Grabar en dicho archivo los parámetros pasados por el request.
	// Ejemplo de archivo de metadata:
	// CONSISTENCY=SC
	// PARTITIONS=4
	// COMPACTION_TIME=60000
	char* nombreDelArchivo=string_new();
	string_append(&nombreDelArchivo, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivo, "/Tables/");
	string_append(&nombreDelArchivo, nombreDeLaTabla);
	string_append(&nombreDelArchivo, "/Metadata");
	log_info(LOGGERFS,"Voy a crear el archivo %s con la metadata", nombreDelArchivo);

	FILE * archivoTemp = fopen(nombreDelArchivo,"w");
	fclose(archivoTemp);

	t_config* configuracion = config_create(nombreDelArchivo);
	config_set_value(configuracion, "CONSISTENCY", tipoDeConsistencia);
	char* aux=string_itoa(numeroDeParticiones);
	config_set_value(configuracion, "PARTITIONS", aux);
	free(aux);
	aux=string_itoa(tiempoDeCompactacion);
	config_set_value(configuracion, "COMPACTION_TIME", aux);
	free(aux);
	config_save(configuracion);
	config_destroy(configuracion);
	log_info(LOGGERFS,"Archivo %s con la metadata creado", nombreDelArchivo);
	free(nombreDelArchivo);
	return EXIT_SUCCESS;
}

bool dirExists(char* path){
	DIR* dir = opendir(path);
	if (dir) {
	    /* Directory exists. */
	    closedir(dir);
	    return true;
	} else if (ENOENT == errno) {
	    /* Directory does not exist. */
		return false;
	} else {
	    /* failed for some other reason. */
		return false;
	}
}

int cargarParticionATabla(char* nombreTabla,int numParticion,int size,t_list* bloques){
	//ej tabla1,3,100,lista de int 3 , 6 , 10
	char* nombreDelBinario=string_new();
	string_append(&nombreDelBinario, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelBinario, "/Tables/");
	string_append(&nombreDelBinario, nombreTabla);
	if(!dirExists(nombreDelBinario)){
		free(nombreDelBinario);
		return 0;
	}
	string_append(&nombreDelBinario, "/");
	char* auxitoa=string_itoa(numParticion);
	string_append(&nombreDelBinario, auxitoa);
	free(auxitoa);
	string_append(&nombreDelBinario, ".bin");
	log_info(LOGGERFS,"Cargo el nuevo binario %s , para la tabla %s , size=%d , que tiene %d bloques",
			nombreDelBinario,nombreTabla,size,bloques->elements_count);
	FILE * archivoBinario = fopen(nombreDelBinario,"w");
	fclose(archivoBinario);
	t_config* configuracion = config_create(nombreDelBinario);
	char* auxSize = string_itoa(size);
	config_set_value(configuracion, "SIZE", auxSize);
	free(auxSize);

	char* lineaBloques=string_new();
	string_append(&lineaBloques, "[");
	char* tmpBloq = string_itoa((int)list_get(bloques,0));
	string_append(&lineaBloques,tmpBloq);
	free(tmpBloq);

	for(int i=1;i<bloques->elements_count;i++){
		string_append(&lineaBloques,",");
		tmpBloq = string_itoa((int)list_get(bloques,i));
		string_append(&lineaBloques,tmpBloq);
		free(tmpBloq);
	}

	string_append(&lineaBloques,"]");
	config_set_value(configuracion, "BLOCKS", lineaBloques);
	config_save(configuracion);
	config_destroy(configuracion);
	free(nombreDelBinario);
	return 1;
}

int crearArchivosBinariosYAsignarBloques(char* nombreDeLaTabla,
		int numeroDeParticiones){
	// Crear los archivos binarios asociados a cada partición de la tabla
	// y asignar a cada uno un bloque
	FILE * archivoTemp;
	char* nombreDelBinario;
	int bloqueLibre;
	t_config* configuracion;
	char* cadenaTemp;

	for(int i=0;i<numeroDeParticiones;i++){
		nombreDelBinario=string_new();
		string_append(&nombreDelBinario, configuracionDelFS.puntoDeMontaje);
		string_append(&nombreDelBinario, "/Tables/");
		string_append(&nombreDelBinario, nombreDeLaTabla);
		string_append(&nombreDelBinario, "/");
		char* auxitoa=string_itoa(i);
		string_append(&nombreDelBinario, auxitoa);
		free(auxitoa);
		string_append(&nombreDelBinario, ".bin");
		log_info(LOGGERFS,"Creando el archivo binario %s", nombreDelBinario);
		archivoTemp = fopen(nombreDelBinario,"w");
		fclose(archivoTemp);
		log_info(LOGGERFS,"Escribiendo en el archivo binario %s", nombreDelBinario);

		pthread_mutex_lock(&mutexBitmap);

		bloqueLibre=obtenerBloqueLibreDelBitMap();

		if(bloqueLibre!=-1){
			ocuparBloqueDelBitmap(bloqueLibre);
			// ejemplo del formato de cada bin
			// SIZE=250
			// BLOCKS=[40,21,82,3]
			crearArchivoDeBloque(bloqueLibre);
			configuracion = config_create(nombreDelBinario);
			char* stringAux = string_itoa(0);
			config_set_value(configuracion, "SIZE", stringAux);
			free(stringAux);
			cadenaTemp=string_new();
			string_append(&cadenaTemp, "[");
			stringAux=string_itoa(bloqueLibre);
			string_append(&cadenaTemp, stringAux);
			free(stringAux);
			string_append(&cadenaTemp, "]");
			config_set_value(configuracion, "BLOCKS", cadenaTemp);
			config_save(configuracion);
			log_info(LOGGERFS,"Archivo %s binario creado", nombreDelBinario);
			config_destroy(configuracion);
			free(cadenaTemp);
			free(nombreDelBinario);
		}else{
			log_error(LOGGERFS,"No hay mas bloques libres");
			pthread_mutex_unlock(&mutexBitmap);
			return EXIT_FAILURE;
			}
		pthread_mutex_unlock(&mutexBitmap);
		}
	return EXIT_SUCCESS;
}

int crearArchivoDeBloque(int bloqueLibre){
	char* nombreDelArchivoDeBloque=string_new();
	string_append(&nombreDelArchivoDeBloque, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivoDeBloque, "/Blocks/");
	char* auxitoa=string_itoa(bloqueLibre);
	string_append(&nombreDelArchivoDeBloque, auxitoa);
	free(auxitoa);
	string_append(&nombreDelArchivoDeBloque, ".bin");
	FILE * archivoDeBloque = fopen(nombreDelArchivoDeBloque,"w");
	fclose(archivoDeBloque);
	log_info(LOGGERFS,"Archivo de bloque %s creado",nombreDelArchivoDeBloque);
	free(nombreDelArchivoDeBloque);
	return EXIT_SUCCESS;
}

int eliminarDirectorioYArchivosDeLaTabla(char* nombreDeLaTabla){
	pthread_mutex_lock(&mutexDeDump);
	if((liberarBloquesYParticiones(nombreDeLaTabla)==EXIT_SUCCESS) &&
			(eliminarTemporales(nombreDeLaTabla)==EXIT_SUCCESS) &&
			(eliminarArchivoDeMetada(nombreDeLaTabla)==EXIT_SUCCESS) &&
			(eliminarDirectorio(nombreDeLaTabla)==EXIT_SUCCESS)&&
			(eliminarDeLaMemtable(nombreDeLaTabla)==EXIT_SUCCESS)
			){
		log_info(LOGGERFS,"La tabla %s se borro correctamente", nombreDeLaTabla);
		pthread_mutex_unlock(&mutexDeDump);
		return EXIT_SUCCESS;
	}else{
		log_error(LOGGERFS,"Hubo algun error al borrar la tabla %s", nombreDeLaTabla);
		pthread_mutex_unlock(&mutexDeDump);
		return EXIT_FAILURE;
		}
}


int eliminarDeLaMemtable(char* nombreDeLaTabla){
	//implementar
	return EXIT_SUCCESS;
}

int eliminarDirectorio(char* nombreDeLaTabla){
	char* directorioABorrar=string_new();
	string_append(&directorioABorrar, configuracionDelFS.puntoDeMontaje);
	string_append(&directorioABorrar, "/Tables/");
	string_append(&directorioABorrar, nombreDeLaTabla);
	char* infoFile = string_new();
	string_append(&infoFile,directorioABorrar);
	string_append(&infoFile,"/");
	string_append(&infoFile,nombreArchivoInfoMsBloqueada);
	remove(infoFile);
	free(infoFile);
	int resultado=rmdir(directorioABorrar);

	if(resultado==0){
		log_info(LOGGERFS,"Se borro el directorio %s", directorioABorrar);
		free(directorioABorrar);
		return EXIT_SUCCESS;
	}else{
		log_error(LOGGERFS,"No se borro el directorio %s", directorioABorrar);
		free(directorioABorrar);
		return EXIT_FAILURE;
		}
}

int setearEstadoDeFinalizacionDelSistema(bool estadoDeFinalizacion){
	pthread_mutex_lock(&mutexEstadoDeFinalizacionDelSistema);
	estadoDeFinalizacionDelSistema=estadoDeFinalizacion;
	pthread_mutex_unlock(&mutexEstadoDeFinalizacionDelSistema);
	return EXIT_SUCCESS;
}

bool obtenerEstadoDeFinalizacionDelSistema(){
	pthread_mutex_lock(&mutexEstadoDeFinalizacionDelSistema);
	bool estado = estadoDeFinalizacionDelSistema;
	pthread_mutex_unlock(&mutexEstadoDeFinalizacionDelSistema);
	return estado;
}

int eliminarArchivoDeMetada(char* nombreDeLaTabla){
	char* nombreDelArchivoDeMetaData=string_new();
	string_append(&nombreDelArchivoDeMetaData, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivoDeMetaData, "/Tables/");
	string_append(&nombreDelArchivoDeMetaData, nombreDeLaTabla);
	string_append(&nombreDelArchivoDeMetaData, "/Metadata");
	int resultado=remove(nombreDelArchivoDeMetaData);
	if(resultado==0){
		//Archivo removido
		log_info(LOGGERFS,"Archivo %s borrado", nombreDeLaTabla);
		free(nombreDelArchivoDeMetaData);
		return EXIT_SUCCESS;
	}else{
		log_error(LOGGERFS,"El archivo %s no se pudo borrar", nombreDeLaTabla);
		free(nombreDelArchivoDeMetaData);
		return EXIT_FAILURE;
	}
}

int eliminarTemporales(char* nombreDeLaTabla){
	char* aux=string_new();
	string_append(&aux, configuracionDelFS.puntoDeMontaje);
	string_append(&aux, "/Tables/");
	string_append(&aux, nombreDeLaTabla);
	string_append(&aux, "/");
	string_append(&aux, nombreDeLaTabla);
	char* pathDelTemp;
	char* directorioDeBloques= string_new();
	string_append(&directorioDeBloques,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeBloques,"/Blocks/");
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

			////////////////////
			/*
			char* directorio=string_new();
			string_append(&directorio, configuracionDelFS.puntoDeMontaje);
			string_append(&directorio, "/Tables/");
			string_append(&directorio, nombreDeLaTabla);
			string_append(&directorio, "/");
			char* ubicacionDelBloque;
			char* directorioDeBloques= string_new();
			string_append(&directorioDeBloques,configuracionDelFS.puntoDeMontaje);
			string_append(&directorioDeBloques,"/Blocks/");
			char* auxatoi;
			for(int i=0;i<metadataDeLaTabla.particiones;i++){
				char* nombreDelArchivo=string_new();
				string_append(&nombreDelArchivo, directorio);
				auxatoi=string_itoa(i);
				string_append(&nombreDelArchivo, auxatoi);
				free(auxatoi);
				string_append(&nombreDelArchivo, ".bin");
				t_config* configuracion = config_create(nombreDelArchivo);
				char** arrayDeBloques = config_get_array_value(configuracion,"BLOCKS");
				config_destroy(configuracion);
				pthread_mutex_lock(&mutexBitmap);
				for(int i=0;arrayDeBloques[i]!=NULL;i++){
					ubicacionDelBloque=string_new();
					log_info(LOGGERFS,"Marcando como libre el bloque: %d", atoi(arrayDeBloques[i]));
					liberarBloqueDelBitmap(atoi(arrayDeBloques[i]));
					string_append(&ubicacionDelBloque,directorioDeBloques);
					string_append(&ubicacionDelBloque,arrayDeBloques[i]);
					string_append(&ubicacionDelBloque,".bin");
					log_info(LOGGERFS,"Borrando el archivo %s", ubicacionDelBloque);
					remove(ubicacionDelBloque);
					free(ubicacionDelBloque);
					free(arrayDeBloques[i]);
					}
				free(arrayDeBloques);
				pthread_mutex_unlock(&mutexBitmap);
				log_info(LOGGERFS,"Borrando el archivo %s", nombreDelArchivo);
				remove(nombreDelArchivo);
				free(nombreDelArchivo);
				}
			free(directorio);
			free(metadataDeLaTabla.consistencia);
			free(directorioDeBloques);
			*/
			///////////////////


			remove(pathDelTemp);
			log_info(LOGGERFS,"Voy a borrar el archivo %s", pathDelTemp);
		}else{
			log_info(LOGGERFS,"No hay mas archivos temporales para borrar");
			}
		}
	free(aux);
	return EXIT_SUCCESS;
}

int liberarBloquesYParticiones(char* nombreDeLaTabla){
	log_info(LOGGERFS,"Voy a borrar los bloques y las particiones");
	t_metadataDeLaTabla metadataDeLaTabla=obtenerMetadataDeLaTabla(nombreDeLaTabla);
	//char* nombreDelArchivo;
	char* directorio=string_new();
	string_append(&directorio, configuracionDelFS.puntoDeMontaje);
	string_append(&directorio, "/Tables/");
	string_append(&directorio, nombreDeLaTabla);
	string_append(&directorio, "/");
	char* ubicacionDelBloque;
	char* directorioDeBloques= string_new();
	string_append(&directorioDeBloques,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeBloques,"/Blocks/");
	char* auxatoi;
	for(int i=0;i<metadataDeLaTabla.particiones;i++){
		char* nombreDelArchivo=string_new();
		string_append(&nombreDelArchivo, directorio);
		auxatoi=string_itoa(i);
		string_append(&nombreDelArchivo, auxatoi);
		free(auxatoi);
		string_append(&nombreDelArchivo, ".bin");
		t_config* configuracion = config_create(nombreDelArchivo);
		char** arrayDeBloques = config_get_array_value(configuracion,"BLOCKS");
		config_destroy(configuracion);
		pthread_mutex_lock(&mutexBitmap);
		for(int i=0;arrayDeBloques[i]!=NULL;i++){
			ubicacionDelBloque=string_new();
			log_info(LOGGERFS,"Marcando como libre el bloque: %d", atoi(arrayDeBloques[i]));
			liberarBloqueDelBitmap(atoi(arrayDeBloques[i]));
			string_append(&ubicacionDelBloque,directorioDeBloques);
			string_append(&ubicacionDelBloque,arrayDeBloques[i]);
			string_append(&ubicacionDelBloque,".bin");
			log_info(LOGGERFS,"Borrando el archivo %s", ubicacionDelBloque);
			remove(ubicacionDelBloque);
			free(ubicacionDelBloque);
			free(arrayDeBloques[i]);
			}
		free(arrayDeBloques);
		pthread_mutex_unlock(&mutexBitmap);
		log_info(LOGGERFS,"Borrando el archivo %s", nombreDelArchivo);
		remove(nombreDelArchivo);
		free(nombreDelArchivo);
		}
	free(directorio);
	free(metadataDeLaTabla.consistencia);
	free(directorioDeBloques);
	return EXIT_SUCCESS;
}

void liberarBloque(char* numeroBloque){
	pthread_mutex_lock(&mutexBitmap);
	log_info(LOGGERFS,"Marcando como libre el bloque: %s", numeroBloque);
	liberarBloqueDelBitmap(atoi(numeroBloque));

	char* directorioDeBloques= string_new();
	string_append(&directorioDeBloques,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeBloques,"/Blocks/");

	char* ubicacionDelBloque=string_new();
	string_append(&ubicacionDelBloque,directorioDeBloques);
	string_append(&ubicacionDelBloque,numeroBloque);
	string_append(&ubicacionDelBloque,".bin");
	log_info(LOGGERFS,"Borrando el archivo %s", ubicacionDelBloque);
	remove(ubicacionDelBloque);
	free(directorioDeBloques);
	free(ubicacionDelBloque);
	pthread_mutex_unlock(&mutexBitmap);
}

void liberarBloquesTmpc(char* pathCompletoTmpc){
	t_config* conf = config_create(pathCompletoTmpc);
	if(conf==NULL)return;
	char** arrayDeBloques = config_get_array_value(conf,"BLOCKS");
	config_destroy(conf);
	for(int i=0;arrayDeBloques[i]!=NULL;i++){
		liberarBloque(arrayDeBloques[i]);
		free(arrayDeBloques[i]);
	}
}

t_metadataDeLaTabla obtenerMetadataDeLaTabla(char* nombreDeLaTabla){
	t_metadataDeLaTabla metadataDeLaTabla;
	metadataDeLaTabla.consistencia=NULL;
	metadataDeLaTabla.particiones=-1;
	metadataDeLaTabla.tiempoDeCompactacion=-1;
	char* nombreDelArchivoDeMetaData=string_new();
	string_append(&nombreDelArchivoDeMetaData, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivoDeMetaData, "/Tables/");
	string_append(&nombreDelArchivoDeMetaData, nombreDeLaTabla);
	string_append(&nombreDelArchivoDeMetaData, "/Metadata");

	t_config* configuracion = config_create(nombreDelArchivoDeMetaData);

	if(configuracion!=NULL){
		//log_info(LOGGERFS,"El archivo de configuracion existe %s",nombreDelArchivoDeMetaData);
	}else{
		log_error(LOGGERFS,"No existe el archivo de metadata en: %s",
				nombreDelArchivoDeMetaData);
		free(nombreDelArchivoDeMetaData);
		return metadataDeLaTabla;
		}
	log_info(LOGGERFS,"Archivo de metadata abierto en: %s",
			nombreDelArchivoDeMetaData);

	//Recupero PARTITIONS
	if(!config_has_property(configuracion,"PARTITIONS")) {
		log_error(LOGGERFS,"No esta PARTITIONS en el archivo de metadata");
		config_destroy(configuracion);
		free(nombreDelArchivoDeMetaData);
		metadataDeLaTabla.consistencia=NULL;
		metadataDeLaTabla.particiones=-1;
		metadataDeLaTabla.tiempoDeCompactacion=-1;
		return metadataDeLaTabla;
		}
	metadataDeLaTabla.particiones = config_get_int_value(configuracion,"PARTITIONS");

	//Recupero COMPACTION_TIME
	if(!config_has_property(configuracion,"COMPACTION_TIME")) {
		log_error(LOGGERFS,"No esta COMPACTION_TIME en el archivo de metadata");
		config_destroy(configuracion);
		free(nombreDelArchivoDeMetaData);
		metadataDeLaTabla.consistencia=NULL;
		metadataDeLaTabla.particiones=-1;
		metadataDeLaTabla.tiempoDeCompactacion=-1;
		return metadataDeLaTabla;
		}
	metadataDeLaTabla.tiempoDeCompactacion = config_get_int_value(configuracion,"COMPACTION_TIME");

	//Recupero el CONSISTENCY
	if(!config_has_property(configuracion,"CONSISTENCY")) {
		log_error(LOGGERFS,"No esta el CONSISTENCY en el archivo de metadata");
		config_destroy(configuracion);
		free(nombreDelArchivoDeMetaData);
		metadataDeLaTabla.consistencia=NULL;
		metadataDeLaTabla.particiones=-1;
		metadataDeLaTabla.tiempoDeCompactacion=-1;
		return metadataDeLaTabla;
		}
	char* consistencia = config_get_string_value(configuracion,"CONSISTENCY");
	metadataDeLaTabla.consistencia = malloc(strlen(consistencia)+1);
	strcpy(metadataDeLaTabla.consistencia,consistencia);
	log_info(LOGGERFS,"Info de la tabla %s recuperada, particiones %d, consistencia %s, tiempo de compactacion %d",
			nombreDeLaTabla, metadataDeLaTabla.particiones,
			metadataDeLaTabla.consistencia, metadataDeLaTabla.tiempoDeCompactacion);

	config_destroy(configuracion);
	free(nombreDelArchivoDeMetaData);
	return metadataDeLaTabla;
}

bool verSiExisteListaConDatosADumpear(char* nombreDeLaTabla){

	bool esMiNodo(void* nodo) {
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		}
	bool resultado;
	pthread_mutex_lock(&mutexDeLaMemtable);
	if(!list_is_empty(memTable)){
		resultado = list_any_satisfy(memTable, esMiNodo);
	}else{
		resultado = false;
	}
	if(resultado==false){
		log_info(LOGGERFS,"La tabla %s no estaba en la memtable", nombreDeLaTabla);
	}else{
		log_info(LOGGERFS,"La tabla %s si estaba en la memtable", nombreDeLaTabla);
		}
	pthread_mutex_unlock(&mutexDeLaMemtable);
	return resultado;
}

bool hayQueFinalizar(){
	return false;
}

int aLocarMemoriaParaLaTabla(char* nombreDeLaTabla){
	log_info(LOGGERFS,"Alocando memoria en la tabla %s para la memtable", nombreDeLaTabla);
	tp_nodoDeLaMemTable nodo = malloc(sizeof(t_nodoDeLaMemTable));
	nodo->nombreDeLaTabla=malloc(strlen(nombreDeLaTabla)+1);
	strcpy(nodo->nombreDeLaTabla,nombreDeLaTabla);
	nodo->listaDeDatosDeLaTabla=list_create();
	pthread_mutex_lock(&mutexDeLaMemtable);
	list_add(memTable,nodo);
	pthread_mutex_unlock(&mutexDeLaMemtable);
	log_info(LOGGERFS,"Memoria alocada");
	return EXIT_SUCCESS;
}

tp_nodoDeLaMemTable obtenerNodoDeLaMemtable(char* nombreDeLaTabla){

	bool esMiNodo(void* nodo) {
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla, nombreDeLaTabla);
	}
	tp_nodoDeLaMemTable nodo = (tp_nodoDeLaMemTable)list_find(memTable, esMiNodo);
	return nodo;
}

int hacerElInsertEnLaMemoriaTemporal(char* nombreDeLaTabla, uint16_t key, char* value,
		unsigned timeStamp){
	log_info(LOGGERFS,"Voy a hacer el insert de los datos en la tabla %s de la memtable",
			nombreDeLaTabla);
	pthread_mutex_lock(&mutexDeLaMemtable);
	tp_nodoDeLaMemTable nodoDeLaMemtable = obtenerNodoDeLaMemtable(nombreDeLaTabla);
	if(nodoDeLaMemtable!=NULL){
		tp_nodoDeLaTabla nuevoNodo=malloc(sizeof(t_nodoDeLaTabla));
		nuevoNodo->key=key;
		nuevoNodo->timeStamp=timeStamp;
		nuevoNodo->value=malloc(strlen(value)+1);
		strcpy(nuevoNodo->value,value);
		list_add(nodoDeLaMemtable->listaDeDatosDeLaTabla,nuevoNodo);

		pthread_mutex_unlock(&mutexDeLaMemtable);
		log_info(LOGGERFS,"Datos insertados");
		return EXIT_SUCCESS;
	}else{
		pthread_mutex_unlock(&mutexDeLaMemtable);
		log_error(LOGGERFS,"Error al insertar los datos, algo se corrompio, no se encontro la tabla en la memtable");
		return EXIT_FAILURE;
	}
}


t_list* escanearPorLaKeyDeseada(uint16_t key, char* nombreDeLaTabla, int numeroDeParticionQueContieneLaKey){
	t_list* listadoDeKeys = list_create();

	log_info(LOGGERFS,"Voy a escanear todo el FS a ver donde existe la key %d para la tabla %s",
			key, nombreDeLaTabla);

	//pthread_mutex_lock(&mutexDeDump);

	pthread_mutex_t* mutexTabla = bloquearTablaFS(nombreDeLaTabla);
	if(mutexTabla!=NULL)
		log_info(LOGGERFS,"Tabla %s bloqueada", nombreDeLaTabla);
	else
		log_error(LOGGERFS,"Tabla %s no bloqueada", nombreDeLaTabla);

	t_list* keysTemporales = escanearPorLaKeyDeseadaMemTable(key, nombreDeLaTabla);
	list_add_all(listadoDeKeys,keysTemporales);
	list_destroy(keysTemporales);


	keysTemporales = escanearPorLaKeyDeseadaArchivosTemporales(key, nombreDeLaTabla);
	list_add_all(listadoDeKeys,keysTemporales);
	list_destroy(keysTemporales);


	if(mutexTabla!=NULL){
		desbloquearTablaFS(mutexTabla);
		log_info(LOGGERFS,"Tabla %s desbloqueada", nombreDeLaTabla);
		}

	//pthread_mutex_unlock(&mutexDeDump);

	keysTemporales = escanearPorLaKeyDeseadaParticionCorrespondiente(key,
			numeroDeParticionQueContieneLaKey, nombreDeLaTabla);
	list_add_all(listadoDeKeys,keysTemporales);
	list_destroy(keysTemporales);

	int sizeDeLaLista = list_size(listadoDeKeys);
	log_info(LOGGERFS,"Keys obtenidas en total, son %d",sizeDeLaLista);

	return listadoDeKeys;
}

t_list* escanearPorLaKeyDeseadaMemTable(uint16_t key, char* nombreDeLaTabla){
	t_list* listaResultante;

	bool esMiKey(void* nodo) {
		return (((tp_nodoDeLaTabla) nodo)->key==key);
		}

	bool esMiTabla(void* nodo){
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		}
	log_info(LOGGERFS,"Escaneando memtable");
	if(!list_is_empty(memTable)){
		log_info(LOGGERFS,"La memtable no esta vacia");
		tp_nodoDeLaMemTable tabla = list_find(memTable, esMiTabla);
		listaResultante = list_filter(tabla->listaDeDatosDeLaTabla,esMiKey);
	}else{
		log_info(LOGGERFS,"Memtable vacia");
		listaResultante = list_create();
	}
	log_info(LOGGERFS,"Cantidad de keys obtenidas de la memtable: %d",list_size(listaResultante));
	log_info(LOGGERFS,"Memtable escaneada");
	return listaResultante;
}

t_list* obtenerListaDeDatosDeArchivo(char* nombreDelArchivo, char* nombreDeLaTabla, uint16_t key){
	t_list* listaResultante;// = list_create();
	char* ubicacionDelBloque;
	char* archivoTempUbicacion = string_new();
	t_config* configuracion = config_create(nombreDelArchivo);
	char** arrayDeBloques = config_get_array_value(configuracion,"BLOCKS");
	config_destroy(configuracion);
	char* directorioDeTrabajo= string_new();
	string_append(&directorioDeTrabajo,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeTrabajo,"/Blocks/");
	string_append(&archivoTempUbicacion,directorioDeTrabajo);
	string_append(&archivoTempUbicacion,"archTemp");
	FILE* archivoTemp=fopen(archivoTempUbicacion,"w");

	for(int i=0;arrayDeBloques[i]!=NULL;i++){
		char ch;
		ubicacionDelBloque=string_new();
		string_append(&ubicacionDelBloque,directorioDeTrabajo);
		string_append(&ubicacionDelBloque,arrayDeBloques[i]);
		string_append(&ubicacionDelBloque,".bin");
		log_info(LOGGERFS,"Voy a unir las keys del bloque %s",ubicacionDelBloque);
		FILE* archivoDeBloque=fopen(ubicacionDelBloque,"r");
		while((ch =fgetc(archivoDeBloque))!=EOF)
			fputc(ch, archivoTemp);
		fclose(archivoDeBloque);
		//list_add_all(listaResultante, recuperarKeysDelArchivoFinal(ubicacionDelBloque, key));
		free(ubicacionDelBloque);
		free(arrayDeBloques[i]);
	}
	free(arrayDeBloques);
	fclose(archivoTemp);
	listaResultante=recuperarKeysDelArchivoFinal(archivoTempUbicacion, key);

	remove(archivoTempUbicacion);
	free(archivoTempUbicacion);
	free(directorioDeTrabajo);
	log_info(LOGGERFS,"Keys rescatadas de los bloques: %d",list_size(listaResultante));
	return listaResultante;
}


t_list* recuperarKeysDelArchivoFinal(char* nombreDelArchivo, uint16_t key){
	/*Dada la ubicacion de un archivo con datos como:
	* 123;1231;asd
	* 1111;11;ddd
	* 5434;111;asddas
	* Me devuelve dentro de una lista estos datos que contiene el archivo
	* */
	tp_nodoDeLaTabla nuevoNodo;
	FILE* archivo = fopen(nombreDelArchivo,"r");
	char** lineaParseada;
	t_list* listaResultante = list_create();
	log_info(LOGGERFS,"Archivo %s abierto",nombreDelArchivo);
	char *linea = NULL;
	char *aux = NULL;
	char **aux2 = NULL;
	size_t linea_buf_size = 0;
	ssize_t linea_size;
	linea_size = getline(&aux, &linea_buf_size, archivo);
	if((linea_size>0)&&(aux!=NULL))
		aux2=string_split(aux,"\n");
	while((linea_size>0)&&(aux!=NULL)&&(aux2!=NULL)&&(aux2[0]!=NULL)){
		log_info(LOGGERFS,"Linea %s recuperada",aux2[0]);
		linea=aux2[0]; //hago esto para sacarle el \n
		//linea=string_duplicate(aux2[0]);

		lineaParseada = string_split(linea, ";");
		if(lineaParseada!=NULL){
			log_info(LOGGERFS,"TimeStamp:%s | Key:%s | Value:%s",
					lineaParseada[0], lineaParseada[1], lineaParseada[2]);
			if(key==atoi(lineaParseada[1])){
				nuevoNodo=malloc(sizeof(t_nodoDeLaTabla));
				nuevoNodo->key=atoi(lineaParseada[1]);
				nuevoNodo->timeStamp=atoi(lineaParseada[0]);
				nuevoNodo->value=malloc(strlen(lineaParseada[2])+1);
				strcpy(nuevoNodo->value,lineaParseada[2]);
				list_add(listaResultante,nuevoNodo);
				}
			for(int j=0;lineaParseada[j]!=NULL;j++) free(lineaParseada[j]);
			free(lineaParseada);
			}
		for(int y=0;aux2[y]!=NULL;y++)
			free(aux2[y]);
		free(aux2);
		linea_size = getline(&aux, &linea_buf_size, archivo);
		aux2=string_split(aux,"\n");
		}
	if(aux!=NULL){
		free(aux);
		}
	if(aux2!=NULL){
		for(int y=0;aux2[y]!=NULL;y++)
			free(aux2[y]);
		free(aux2);
		}
	fclose(archivo);
	log_info(LOGGERFS,"Archivo %s parseado",nombreDelArchivo);
	log_info(LOGGERFS,"Keys rescatadas del bloques %s: %d",
			nombreDelArchivo, list_size(listaResultante));
	return listaResultante;
}


bool existeElArchivo(char* nombreDelArchivo){
	//me dice si existe o no un archivo
	FILE* archivo = fopen(nombreDelArchivo,"r");
	if(archivo!=NULL){
		fclose(archivo);
		return true;
	}else{
		return false;
	}
}

t_list* escanearPorLaKeyDeseadaArchivosTemporales(uint16_t key, char* nombreDeLaTabla){
	t_list* listaResultante = list_create();
	log_info(LOGGERFS,"Escaneando archivos temporales");
	char* directorioDeLasTablas= string_new();
	string_append(&directorioDeLasTablas,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeLasTablas,"/Tables/");
	string_append(&directorioDeLasTablas,nombreDeLaTabla);
	string_append(&directorioDeLasTablas,"/");
	string_append(&directorioDeLasTablas,nombreDeLaTabla);
	char* ubicacionDelTemp;
	bool noHayMas=false;
	char* auxatoi;
	for(int i=1;noHayMas==false;i++){
		ubicacionDelTemp=string_new();
		string_append(&ubicacionDelTemp,directorioDeLasTablas);
		auxatoi=string_itoa(i);
		string_append(&ubicacionDelTemp,auxatoi);
		free(auxatoi);
		string_append(&ubicacionDelTemp,".tmp");
		if(existeElArchivo(ubicacionDelTemp)){
			log_info(LOGGERFS,"Checkeando en el archivo temporal %s",ubicacionDelTemp);
			t_list* listaAux=obtenerListaDeDatosDeArchivo(ubicacionDelTemp, nombreDeLaTabla, key);
			list_add_all(listaResultante,listaAux);
			list_destroy(listaAux);
		}else{
			noHayMas=true;
			}
		free(ubicacionDelTemp);
		}
	free(directorioDeLasTablas);
	log_info(LOGGERFS,"Cantidad de keys obtenidas de los temporales: %d",list_size(listaResultante));
	log_info(LOGGERFS,"Archivos temporales escaneados");
	return listaResultante;
}

t_list* escanearPorLaKeyDeseadaParticionCorrespondiente(uint16_t key,
		int numeroDeParticionQueContieneLaKey, char* nombreDeLaTabla){
	t_list* listaResultante;//= list_create();
	log_info(LOGGERFS,"Escaneando particion correspondiente, es la %d, de la tabla %s",
			numeroDeParticionQueContieneLaKey, nombreDeLaTabla);
	char* directorioDeLasTablas= string_new();
	//char* ubicacionDelArchivo;
	string_append(&directorioDeLasTablas,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeLasTablas,"/Tables/");
	string_append(&directorioDeLasTablas,nombreDeLaTabla);
	string_append(&directorioDeLasTablas,"/");
	char* auxitoa = string_itoa(numeroDeParticionQueContieneLaKey);
	string_append(&directorioDeLasTablas,auxitoa);
	free(auxitoa);
	string_append(&directorioDeLasTablas,".bin");
	log_info(LOGGERFS,"Checkeando en el archivo %d de las particiones, %s",
								numeroDeParticionQueContieneLaKey, directorioDeLasTablas);
	if(existeElArchivo(directorioDeLasTablas)){
		listaResultante=obtenerListaDeDatosDeArchivo(directorioDeLasTablas, nombreDeLaTabla, key);
		}

	free(directorioDeLasTablas);
	log_info(LOGGERFS,"Cantidad de keys obtenidas de la particion: %d",list_size(listaResultante));
	log_info(LOGGERFS,"Particion correspondiente escaneada");
	return listaResultante;
}

tp_nodoDeLaTabla obtenerKeyConTimeStampMasGrande(t_list* keysObtenidas){
	tp_nodoDeLaTabla keyObtenida = NULL;
	unsigned tiempo;

	bool esLaMayor(void* nodo){
		bool sonTodosMenores(void* nodo2){
			return (tiempo>=((tp_nodoDeLaTabla)nodo2)->timeStamp);
			}
		tiempo = ((tp_nodoDeLaTabla)nodo)->timeStamp;
		return list_all_satisfy(keysObtenidas,sonTodosMenores);
		}
	bool esMiNodoMayor(void* nodo) {
		if(((((tp_nodoDeLaTabla)nodo)->key)==keyObtenida->key)&&
				(!strcmp(((tp_nodoDeLaTabla) nodo)->value,keyObtenida->value))&&
				((((tp_nodoDeLaTabla) nodo)->timeStamp)==keyObtenida->timeStamp)
				){
			return true;
			}
		return false;
		}
	log_info(LOGGERFS,"Buscando key con el timestamp mas grande");
	if(!list_is_empty(keysObtenidas)){
		keyObtenida = list_find(keysObtenidas,esLaMayor);
		list_remove_by_condition(keysObtenidas,esMiNodoMayor);
		if(keyObtenida!=NULL){
			log_info(LOGGERFS,"El mayor timestamp para la key %d fue de: %d, con un value de: %s",
				keyObtenida->key, keyObtenida->timeStamp, keyObtenida->value);
			keyObtenida->resultado=KEY_OBTENIDA;
		}else{
			log_info(LOGGERFS,"Lista vacia, key no existia");
			keyObtenida=malloc(sizeof(t_nodoDeLaTabla));
			keyObtenida->resultado=KEY_NO_EXISTE;
			}
		log_info(LOGGERFS,"El key con el timestamp mas grande es: %s", keyObtenida->value);
	}else{
		log_info(LOGGERFS,"No hay ninguna key con ese valor en la tabla");
		keyObtenida=malloc(sizeof(t_nodoDeLaTabla));
		keyObtenida->resultado=KEY_NO_EXISTE;
	}
	return keyObtenida;
}

int vaciarListaDeKeys(t_list* keysObtenidas){
	void vaciarNodo(void* nodo){
		log_info(LOGGERFS,"Liberando memoria de value: %s, key: %d, timestamp: %d",
				((tp_nodoDeLaTabla)nodo)->value,
				((tp_nodoDeLaTabla)nodo)->key,
				((tp_nodoDeLaTabla)nodo)->timeStamp);
		free(((tp_nodoDeLaTabla)nodo)->value);
		free((tp_nodoDeLaTabla)nodo);
		return;
	}
	list_clean_and_destroy_elements(keysObtenidas,vaciarNodo);
	list_destroy(keysObtenidas);
	return EXIT_SUCCESS;
}

char* recortarHastaUltimaBarra(char* path){
	int indice = (int)(strrchr(path,'/')-path)+1;
	char* ultimoNombre = string_substring_from(path,indice);
	return ultimoNombre;
}

t_list* obtenerTodosLosDescriptores(){
	char* main_directorio=string_new();
	string_append(&main_directorio, configuracionDelFS.puntoDeMontaje);
	string_append(&main_directorio, "/Tables/");

	t_list* metadata_todos_los_descriptores = list_create();

	//deberia ser static pero gcc no estaba feliz, veo si funca igualmente
	int guardar_metadata_descriptores(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){

		char* path_nombre;
		if (ftwbuf->level == 0) return FTW_CONTINUE;
		if (ftwbuf->level > 1) return FTW_SKIP_SUBTREE;
		if(tflag == FTW_D) {

			path_nombre = recortarHastaUltimaBarra(fpath);

			pthread_mutex_t* mutexTabla = bloquearTablaFS(path_nombre);
			t_metadataDeLaTabla unaMetadata = obtenerMetadataDeLaTabla(path_nombre);
			if(mutexTabla!=NULL)desbloquearTablaFS(mutexTabla);

			if(unaMetadata.consistencia!=NULL){
				tp_describe_rta metadataEncodeada = malloc(sizeof(t_describe_rta));
				metadataEncodeada->nombre = malloc (strlen(path_nombre)+1);
				memcpy(metadataEncodeada->nombre,path_nombre,strlen(path_nombre)+1);
				metadataEncodeada->consistencia= malloc(strlen(unaMetadata.consistencia)+1);
				memcpy(metadataEncodeada->consistencia,unaMetadata.consistencia,strlen(unaMetadata.consistencia)+1);
				metadataEncodeada->particiones=unaMetadata.particiones;
				metadataEncodeada->tiempoDeCompactacion=unaMetadata.tiempoDeCompactacion;
				list_add(metadata_todos_los_descriptores,(void*)metadataEncodeada);
				free(unaMetadata.consistencia);
			}
			return FTW_SKIP_SUBTREE;
			//return FTW_SKIP_SUBTREE;//salta a la proxima carpeta sin mirar los contenidos
		}
		return FTW_CONTINUE;
	}

	nftw(main_directorio,guardar_metadata_descriptores,20,FTW_ACTIONRETVAL|FTW_PHYS);//deberia retornar FTW_STOP
	//@@hacer if con result, ver primero que devuelve
	//@@revisar si devuelve el nombre de la carpeta o todo el path

	free(main_directorio);

	if(metadata_todos_los_descriptores->elements_count==0) {
		list_destroy(metadata_todos_los_descriptores);
		return NULL;
	}
	else return metadata_todos_los_descriptores;
}


void free_tp_describe_rta(void* d){
	tp_describe_rta unDescriptor = d;
	free(unDescriptor->nombre);
	free(unDescriptor->consistencia);
	free(unDescriptor);
}

void liberarYDestruirTablaDeMetadata(t_list* descriptores){//libera y destruye una lista que tenga tp_describe_rta dentro
	list_destroy_and_destroy_elements(descriptores,free_tp_describe_rta);
}

int insertarDatosEnElNuevoBloque(char* cadenaAInsertar,int bloqueActual){
	//le pasas un numero de bloque y una cadena con los datos a insertar y los manda
	//a continuacion de lo q haya
	char* nombreDelArchivoDeBloque=string_new();
	string_append(&nombreDelArchivoDeBloque, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivoDeBloque, "/Blocks/");
	char* auxitoa=string_itoa(bloqueActual);
	string_append(&nombreDelArchivoDeBloque, auxitoa);
	free(auxitoa);
	string_append(&nombreDelArchivoDeBloque, ".bin");
	FILE* archivo=fopen(nombreDelArchivoDeBloque,"w");
	log_info(LOGGERFS,"Guardando %s en el archivo %s", cadenaAInsertar, nombreDelArchivoDeBloque);
	fprintf(archivo,"%s",cadenaAInsertar);
	free(nombreDelArchivoDeBloque);
	fclose(archivo);
	return EXIT_SUCCESS;
}

t_list* insertarCadenaEnNuevosBloques(char* cadenaGigante){//retorna una lista de int 1,2, ..., o NULL si no quedan bloques(si ya tomo algun bloque lo libera)
	int bloqueActual;
	//char* bloques=string_new();
	//string_append(&bloques, "[");

	int cantBloques = (int)ceil((double)strlen(cadenaGigante)/(double)metadataDelFS.blockSize);
	t_list* bloques = list_create();

	if(cantBloques==0){
		pthread_mutex_lock(&mutexBitmap);
		bloqueActual=obtenerBloqueLibreDelBitMap();
		ocuparBloqueDelBitmap(bloqueActual);
		pthread_mutex_unlock(&mutexBitmap);
		bajarADiscoBitmap();
		if(bloqueActual==-1){
			list_destroy(bloques);
			return NULL;
		}
		list_add(bloques,bloqueActual);
		insertarDatosEnElNuevoBloque("", bloqueActual);
		return bloques;
	}

	for(int i=0;i<cantBloques;i++){
		pthread_mutex_lock(&mutexBitmap);
		bloqueActual=obtenerBloqueLibreDelBitMap();
		ocuparBloqueDelBitmap(bloqueActual);
		pthread_mutex_unlock(&mutexBitmap);
		bajarADiscoBitmap();
		if(bloqueActual==-1){
			pthread_mutex_lock(&mutexBitmap);
			list_iterate(bloques,liberarBloqueDelBitmap);
			pthread_mutex_unlock(&mutexBitmap);
			bajarADiscoBitmap();
			list_destroy(bloques);
			return NULL;
		}
		else list_add(bloques,(void*)bloqueActual);//@@no toy seguro si puedo simplemente agregar un int sin que se destruya
	}

	for(int i=0;i<cantBloques;i++){

		char* data = string_substring(cadenaGigante,i*metadataDelFS.blockSize,
				i==0 ? (int)fmin((float)strlen(cadenaGigante),(float)metadataDelFS.blockSize)
						:strlen(cadenaGigante)-metadataDelFS.blockSize*i);
		log_info(LOGGERFS,"Guardando:\n%s\n en el archivo de bloque %d",
				data, list_get(bloques,i));
		insertarDatosEnElNuevoBloque(data, (int)list_get(bloques,i));
		free(data);
	}
	return bloques;
}

void liberarBloquesDelBitmap(t_list* bloques){
	pthread_mutex_lock(&mutexBitmap);
	list_iterate(bloques,liberarBloqueDelBitmap);
	pthread_mutex_unlock(&mutexBitmap);
}


