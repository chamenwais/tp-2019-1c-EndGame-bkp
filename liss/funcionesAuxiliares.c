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
	config_set_value(configuracion, "PARTITIONS", string_itoa(numeroDeParticiones));
	config_set_value(configuracion, "COMPACTION_TIME", string_itoa(tiempoDeCompactacion));
	config_save(configuracion);
	config_destroy(configuracion);
	log_info(LOGGERFS,"Archivo %s con la metadata creado", nombreDelArchivo);
	free(nombreDelArchivo);
	return EXIT_SUCCESS;
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

	for(int i=1;i<=numeroDeParticiones;i++){
		nombreDelBinario=string_new();
		string_append(&nombreDelBinario, configuracionDelFS.puntoDeMontaje);
		string_append(&nombreDelBinario, "/Tables/");
		string_append(&nombreDelBinario, nombreDeLaTabla);
		string_append(&nombreDelBinario, "/");
		string_append(&nombreDelBinario, string_itoa(i));
		string_append(&nombreDelBinario, ".bin");
		log_info(LOGGERFS,"Creando el archivo binario %s", nombreDelBinario);
		archivoTemp = fopen(nombreDelBinario,"w");
		fclose(archivoTemp);
		log_info(LOGGERFS,"Escribiendo en el archivo binario %s", nombreDelBinario);

		bloqueLibre=obtenerBloqueLibreDelBitMap();

		if(bloqueLibre!=-1){
			ocuparBloqueDelBitmap(bloqueLibre);
			// ejemplo del formato de cada bin
			// SIZE=250
			// BLOCKS=[40,21,82,3]
			configuracion = config_create(nombreDelBinario);
			config_set_value(configuracion, "SIZE", string_itoa(0));
			cadenaTemp=string_new();
			string_append(&cadenaTemp, "[");
			string_append(&cadenaTemp, string_itoa(bloqueLibre));
			string_append(&cadenaTemp, "]");
			config_set_value(configuracion, "PARTITIONS", cadenaTemp);
			config_save(configuracion);
			log_info(LOGGERFS,"Archivo %s binario creado", nombreDelBinario);
			config_destroy(configuracion);
			free(cadenaTemp);
			free(nombreDelBinario);
		}else{
			log_error(LOGGERFS,"No hay mas bloques libres");
			return EXIT_FAILURE;
			}

		}
	return EXIT_SUCCESS;
}

int eliminarDirectorioYArchivosDeLaTabla(char* nombreDeLaTabla){
	bloquearTabla(nombreDeLaTabla);
	if((liberarBloquesYParticiones(nombreDeLaTabla)==EXIT_SUCCESS) &&
			(eliminarArchivoDeMetada(nombreDeLaTabla)==EXIT_SUCCESS) &&
			(eliminarDirectorio(nombreDeLaTabla)==EXIT_SUCCESS)){
		log_info(LOGGERFS,"La tabla %s se borro correctamente", nombreDeLaTabla);
		return EXIT_SUCCESS;
	}else{
		log_error(LOGGERFS,"Hubo algun error al borrar la tabla %s", nombreDeLaTabla);
		return EXIT_FAILURE;
		}
}

int eliminarDirectorio(char* nombreDeLaTabla){
	char* directorioABorrar=string_new();
	string_append(&directorioABorrar, configuracionDelFS.puntoDeMontaje);
	string_append(&directorioABorrar, "/Tables/");
	string_append(&directorioABorrar, nombreDeLaTabla);
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

int liberarBloquesYParticiones(char* nombreDeLaTabla){
	t_metadataDeLaTabla metadataDeLaTabla=obtenerMetadataDeLaTabla(nombreDeLaTabla);
	char* nombreDelArchivo;
	char* directorio=string_new();
	string_append(&directorio, configuracionDelFS.puntoDeMontaje);
	string_append(&directorio, "/Tables/");
	string_append(&directorio, nombreDeLaTabla);
	string_append(&directorio, "/");

	for(int i=1;i<=metadataDeLaTabla.particiones;i++){
		char* nombreDelArchivo=string_new();
		string_append(&nombreDelArchivo, directorio);
		string_append(&nombreDelArchivo, string_itoa(i));
		string_append(&nombreDelArchivo, ".bin");
		remove(nombreDelArchivo);
		free(nombreDelArchivo);
		}
	return EXIT_SUCCESS;
}

int bloquearTabla(char* nombreDeLaTabla){
	// Esta funcion bloquea la tabla para q no sea utilizada por multiples hilos simultaneamente

	return EXIT_SUCCESS;
}

int desbloquearTabla(char* nombreDeLaTabla){
	// Esta funcion desbloquea la tabla para q la pueda usar algun hilo
	return EXIT_SUCCESS;
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
	return resultado;
}

int aLocarMemoriaParaLaTabla(char* nombreDeLaTabla){
	log_info(LOGGERFS,"Alocando memoria en la tabla %s para la memtable", nombreDeLaTabla);
	tp_nodoDeLaMemTable nodo = malloc(sizeof(t_nodoDeLaMemTable));
	nodo->nombreDeLaTabla=malloc(strlen(nombreDeLaTabla)+1);
	strcpy(nodo->nombreDeLaTabla,nombreDeLaTabla);
	nodo->listaDeDatosDeLaTabla=list_create();
	list_add(memTable,nodo);
	log_info(LOGGERFS,"Memoria alocada");
	return EXIT_SUCCESS;
}

tp_nodoDeLaMemTable obtenerNodoDeLaMemtable(char* nombreDeLaTabla){

	bool esMiNodo(void* nodo) {
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla, nombreDeLaTabla);
	}

	return (tp_nodoDeLaMemTable)list_find(memTable, esMiNodo);
}

int hacerElInsertEnLaMemoriaTemporal(char* nombreDeLaTabla, uint16_t key, char* value,
		unsigned timeStamp){
	log_info(LOGGERFS,"Voy a hacer el insert de los datos en la tabla %s de la memtable",
			nombreDeLaTabla);
	tp_nodoDeLaMemTable nodoDeLaMemtable = obtenerNodoDeLaMemtable(nombreDeLaTabla);
	if(nodoDeLaMemtable!=NULL){
		tp_nodoDeLaTabla nuevoNodo=malloc(sizeof(t_nodoDeLaTabla));
		nuevoNodo->key=key;
		nuevoNodo->timeStamp=timeStamp;
		nuevoNodo->value=malloc(strlen(value)+1);
		strcpy(nuevoNodo->value,value);
		list_add(nodoDeLaMemtable->listaDeDatosDeLaTabla,nuevoNodo);
		log_info(LOGGERFS,"Datos insertados");
		return EXIT_SUCCESS;
	}else{
		log_error(LOGGERFS,"Error al insertar los datos, algo se corrompio, no se encontro la tabla en la memtable");
		return EXIT_FAILURE;
	}
}


t_list* escanearPorLaKeyDeseada(uint16_t key, char* nombreDeLaTabla, int numeroDeParticionQueContieneLaKey){
	t_list* listadoDeKeys = list_create();

	log_info(LOGGERFS,"Voy a escanear todo el FS a ver donde existe la key %d para la tabla %s",
			key, nombreDeLaTabla);

	t_list* keysDeLaMemTable = escanearPorLaKeyDeseadaMemTable(key, nombreDeLaTabla);
	list_add_all(listadoDeKeys,keysDeLaMemTable);
	list_destroy(keysDeLaMemTable);

	keysDeLaMemTable = escanearPorLaKeyDeseadaArchivosTemporales(key, nombreDeLaTabla);
	list_add_all(listadoDeKeys,keysDeLaMemTable);
	list_destroy(keysDeLaMemTable);

	keysDeLaMemTable = escanearPorLaKeyDeseadaParticionCorrespondiente(key,
			numeroDeParticionQueContieneLaKey);
	list_add_all(listadoDeKeys,keysDeLaMemTable);
	list_destroy(keysDeLaMemTable);

	log_info(LOGGERFS,"Keys obtenidas");

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
		tp_nodoDeLaMemTable tabla = list_find(memTable, esMiTabla);
		listaResultante = list_filter(tabla->listaDeDatosDeLaTabla,esMiKey);
	}else{
		listaResultante = list_create();
	}
	log_info(LOGGERFS,"Memtable escaneada");
	return listaResultante;
}

t_list* obtenerListaDeDatosDeArchivo(char* nombreDelArchivo){
	/*Dada la ubicacion de un archivo con datos como:
	* 123;1231;asd
	* 1111;11;ddd
	* 5434;111;asddas
	* Me devuelve dentro de una lista estos datos que contiene el archivo
	* */
	FILE* archivo = fopen(nombreDelArchivo,"r");
	char** lineaParseada;
	t_list* listaResultante = list_create();
	log_info(LOGGERFS,"Archivo %s abierto",nombreDelArchivo);
	char *linea = NULL;
	size_t linea_buf_size = 0;
	ssize_t linea_size;
	linea_size = getline(&linea, &linea_buf_size, archivo);
	while (linea_size >= 0){
		lineaParseada = string_split(linea, ";");
		log_info(LOGGERFS,"TimeStamp:%s | Key:%s | Value:%s",
		lineaParseada[0], lineaParseada[1], lineaParseada[2]);
		tp_nodoDeLaTabla nuevoNodo=malloc(sizeof(t_nodoDeLaTabla));
		nuevoNodo->key=atoi(lineaParseada[1]);
		nuevoNodo->timeStamp=atoi(lineaParseada[0]);
		nuevoNodo->value=malloc(strlen(lineaParseada[2])+1);
		strcpy(nuevoNodo->value,lineaParseada[2]);
		list_add(listaResultante,nuevoNodo);
		for(int j=0;lineaParseada[j]!=NULL;j++) free(lineaParseada[j]);
		linea_size = getline(&linea, &linea_buf_size, archivo);
		}
	fclose(archivo);
	log_info(LOGGERFS,"Archivo %s parseado",nombreDelArchivo);
	return listaResultante;
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
	for(int i=1;noHayMas==false;i++){
		ubicacionDelTemp=string_new();
		string_append(&ubicacionDelTemp,directorioDeLasTablas);
		string_append(&ubicacionDelTemp,string_itoa(i));
		string_append(&ubicacionDelTemp,".tmp");
		log_info(LOGGERFS,"Checkeando en el archivo %s",ubicacionDelTemp);
		FILE* archivo = fopen(ubicacionDelTemp,"r");
		if(archivo!=NULL){
			fclose(archivo);
			list_add_all(listaResultante,obtenerListaDeDatosDeArchivo(ubicacionDelTemp));
		}else{
			noHayMas=true;
		}
		free(ubicacionDelTemp);
		}
	free(directorioDeLasTablas);
	log_info(LOGGERFS,"Archivos temporales escaneados");
	return listaResultante;
}

t_list* escanearPorLaKeyDeseadaParticionCorrespondiente(uint16_t key, int numeroDeParticionQueContieneLaKey){
	t_list* listaResultante= list_create();
	log_info(LOGGERFS,"Escaneando particion correspondiente");
	log_info(LOGGERFS,"Particion correspondiente escaneada");
	return listaResultante;
}

char* obtenerKeyConTimeStampMasGrande(t_list* keysObtenidas){
	tp_nodoDeLaTabla keyObtenida = NULL;
	unsigned tiempo;
	bool esLaMayor(void* nodo){
		bool sonTodosMenores(void* nodo2){
			return (tiempo>=((tp_nodoDeLaTabla)nodo2)->timeStamp);
			}
		tiempo = ((tp_nodoDeLaTabla)nodo)->timeStamp;
		return list_all_satisfy(keysObtenidas,sonTodosMenores);
		}
	log_info(LOGGERFS,"Buscando key con el timestamp mas grande");
	if(!list_is_empty(keysObtenidas)){
		keyObtenida = list_find(keysObtenidas,esLaMayor);
		if(keyObtenida!=NULL){
			log_info(LOGGERFS,"El mayor timestamp para la key %d fue de: %d, con un value de: %s",
				keyObtenida->key, keyObtenida->timeStamp, keyObtenida->value);
		}else{
			log_info(LOGGERFS,"Lista vacia");
			}
		log_info(LOGGERFS,"El key con el timestamp mas grande es: %s", keyObtenida->value);
		return keyObtenida->value;
	}else{
		log_info(LOGGERFS,"No hay ninguna key en la tabla");
		return string_new();
	}
}

int vaciarListaDeKeys(t_list* keysObtenidas){

	return EXIT_SUCCESS;
}
