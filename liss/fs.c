/*
 * fs.c
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#include "fs.h"

void inicializarLogDelFS(){
	LOGGERFS = log_create("FileSystem.log","FileSystem",true,LOG_LEVEL_INFO);
	log_info(LOGGERFS,"Log del FileSystem iniciado");
	return;
}

int obtenerPathDeMontajeDelPrograma(int argc,char** argv){
	pathDeMontajeDelPrograma=string_new();
	if(argc>1){
		log_info(LOGGERFS,"Path de trabajo recibido por parametro");
		string_append(&pathDeMontajeDelPrograma, argv[1]);
		string_append(&pathDeMontajeDelPrograma, "/");
	}else{
		int indice = (int)(strrchr(argv[0],'/')-argv[0])+1;
		char* defaultPath = string_substring_until(argv[0],indice);

		string_append(&pathDeMontajeDelPrograma,defaultPath);

		free(defaultPath);

		log_info(LOGGERFS,"El path de trabajo es el local");
		}
	return EXIT_SUCCESS;
	}

int levantarConfiguracionInicialDelFS(){
	/* Ejemplo de los datos a levantar:
	 * PUERTO_ESCUCHA=5003
	 * PUNTO_MONTAJE=/home/utnso/endGame/tp-2019-1c-EndGame/liss/Debug
	 * RETARDO=500
	 * TAMAÑO_VALUE=4
	 * TIEMPO_DUMP=5000*/

	char* pathCompletoDelArchivoDeConfiguracion;
	pathCompletoDelArchivoDeConfiguracion=string_new();
	string_append(&pathCompletoDelArchivoDeConfiguracion, pathDeMontajeDelPrograma);
	string_append(&pathCompletoDelArchivoDeConfiguracion, "configuracionFS.cfg");

	t_config* configuracion = config_create(pathCompletoDelArchivoDeConfiguracion);

	if(configuracion!=NULL){
		log_info(LOGGERFS,"El archivo de configuracion existe");
	}else{
		log_error(LOGGERFS,"No existe el archivo de configuracion en: %s",pathCompletoDelArchivoDeConfiguracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	log_info(LOGGERFS,"Abriendo el archivo de configuracion del FS, su ubicacion es: %s",pathCompletoDelArchivoDeConfiguracion);

	//Recupero el puerto de escucha
	if(!config_has_property(configuracion,"PUERTO_ESCUCHA")) {
		log_error(LOGGERFS,"No esta el PUERTO_ESCUCHA en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.puertoEscucha = config_get_int_value(configuracion,"PUERTO_ESCUCHA");
	log_info(LOGGERFS,"Puerto de escucha del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.puertoEscucha);
	//Recupero el punto de montaje
	if(!config_has_property(configuracion,"PUNTO_MONTAJE")) {
		log_error(LOGGERFS,"No esta el PUNTO_MONTAJE en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	char* puntoDeMontaje = config_get_string_value(configuracion,"PUNTO_MONTAJE");
	configuracionDelFS.puntoDeMontaje = malloc(strlen(puntoDeMontaje)+1);
	strcpy(configuracionDelFS.puntoDeMontaje,puntoDeMontaje);
	log_info(LOGGERFS,"Punto de montaje del archivo de configuracion del FS recuperado: \"%s\"",
			configuracionDelFS.puntoDeMontaje);
	//Recupero el retardo
	if(!config_has_property(configuracion,"RETARDO")) {
		log_error(LOGGERFS,"No esta el RETARDO en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.retardo = config_get_int_value(configuracion,"RETARDO");
	log_info(LOGGERFS,"Retardo del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.retardo);

	//Recupero el tamaño value
	if(!config_has_property(configuracion,"TAMAÑO_VALUE")) {
		log_error(LOGGERFS,"No esta el TAMAÑO_VALUE en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.sizeValue = config_get_int_value(configuracion,"TAMAÑO_VALUE");
	log_info(LOGGERFS,"Tamaño value del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.sizeValue);

	//Recupero el tiempo dump
	if(!config_has_property(configuracion,"TIEMPO_DUMP")) {
		log_error(LOGGERFS,"No esta el TIEMPO_DUMP en el archivo de configuracion");
		config_destroy(configuracion);
		log_error(LOGGERFS,"No se pudo levantar la configuracion del FS, abortando");
		return EXIT_FAILURE;
		}
	configuracionDelFS.tiempoDump = config_get_int_value(configuracion,"TIEMPO_DUMP");
	log_info(LOGGERFS,"Tiempo dump del archivo de configuracion del FS recuperado: %d",
			configuracionDelFS.tiempoDump);

	config_destroy(configuracion);
	log_info(LOGGERFS,"Configuracion del FS recuperada exitosamente");

	return EXIT_SUCCESS;
	}

int levantarMetadataDelFS(){
	/* Ejemplo de los datos a levantar:
	BLOCK_SIZE=64
	BLOCKS=5192
	MAGIC_NUMBER=LISSANDRA*/

	directorioConLaMetadata=string_new();
	string_append(&directorioConLaMetadata, configuracionDelFS.puntoDeMontaje);
	string_append(&directorioConLaMetadata, "/Metadata");
	archivoDeLaMetadata=string_new();
	string_append(&archivoDeLaMetadata, directorioConLaMetadata);
	archivoDeBitmap=string_new();
	string_append(&archivoDeBitmap, directorioConLaMetadata);
	string_append(&archivoDeBitmap, "/Bitmap.bin");
	string_append(&archivoDeLaMetadata, "/Metadata.bin");
	t_config* metadataConfig = config_create(archivoDeLaMetadata);

	if(metadataConfig!=NULL){
		log_info(LOGGERFS,"El archivo de de metadata del FS existe");
	}else{
		log_error(LOGGERFS,"No existe el archivo de metadata en: %s",archivoDeLaMetadata);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	log_info(LOGGERFS,"Abriendo el archivo de metadata del FS, su ubicacion es: %s",archivoDeLaMetadata);

	//Recupero el block size
	if(!config_has_property(metadataConfig,"BLOCK_SIZE")) {
		log_error(LOGGERFS,"No esta el BLOCK_SIZE en el archivo de metadata");
		config_destroy(metadataConfig);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	metadataDelFS.blockSize = config_get_int_value(metadataConfig,"BLOCK_SIZE");
	log_info(LOGGERFS,"Block size de la metadata del FS recuperado: %d",
			metadataDelFS.blockSize);

	//Recupero la cantidad de bloques
	if(!config_has_property(metadataConfig,"BLOCKS")) {
		log_error(LOGGERFS,"No esta el valor BLOCKS en el archivo de metadata");
		config_destroy(metadataConfig);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	metadataDelFS.blocks = config_get_int_value(metadataConfig,"BLOCKS");
	log_info(LOGGERFS,"Blocks de la metadata del FS recuperado: %d",
			metadataDelFS.blocks);


	//Recupero el magic number
	if(!config_has_property(metadataConfig,"MAGIC_NUMBER")) {
		log_error(LOGGERFS,"No esta el MAGIC_NUMBER en el archivo de configuracion");
		config_destroy(metadataConfig);
		log_error(LOGGERFS,"No se pudo levantar la metadata del FS, abortando");
		return EXIT_FAILURE;
		}
	char* magicNumberTemp = config_get_string_value(metadataConfig,"MAGIC_NUMBER");
	metadataDelFS.magicNumber = malloc(strlen(magicNumberTemp)+1);
	strcpy(metadataDelFS.magicNumber,magicNumberTemp);
	log_info(LOGGERFS,"Magic number de la metadata recuperada: \"%s\"",
			metadataDelFS.magicNumber);

	config_destroy(metadataConfig);
	log_info(LOGGERFS,"Configuracion del FS recuperada exitosamente");

	return EXIT_SUCCESS;
	}

int actualizarTiempoDump(int tiempoDump){
	//La variable global "configuracionDelFS.tiempoDump" solo debe ser modificada por medio
	//de esta funcion
	pthread_mutex_lock(&mutexVariableTiempoDump);
	configuracionDelFS.tiempoDump=tiempoDump;
	pthread_mutex_unlock(&mutexVariableTiempoDump);
	return EXIT_SUCCESS;
	}

int obtenerTiempoDump(){
	//La variable global "configuracionDelFS.tiempoDump" solo debe ser leida por medio
	//de esta funcion
	int tiempoDump;
	pthread_mutex_lock(&mutexVariableTiempoDump);
	tiempoDump=configuracionDelFS.tiempoDump;
	pthread_mutex_unlock(&mutexVariableTiempoDump);
	return tiempoDump;
	}

int actualizarRetardo(int retardo){
	//La variable global "configuracionDelFS.retardo" solo debe ser modificada por medio
	//de esta funcion
	pthread_mutex_lock(&mutexVariableRetardo);
	configuracionDelFS.retardo=retardo;
	pthread_mutex_unlock(&mutexVariableRetardo);
	return EXIT_SUCCESS;
	}

int obtenerRetardo(){
	//La variable global "configuracionDelFS.retardo" solo debe ser leida por medio
	//de esta funcion
	int retardo;
	pthread_mutex_lock(&mutexVariableRetardo);
	retardo=configuracionDelFS.retardo;
	pthread_mutex_unlock(&mutexVariableRetardo);
	return retardo;
	}

int obtenerBloqueLibreDelBitMap(){
	/* Retorna un numero de bloque libre en el bitmap
	 * Si no hay mas bloques libre retorna -1 */
	int i;
	log_info(LOGGERFS,"Obteniendo bloque libre");
	for(i=0;i<metadataDelFS.blocks;i++){
		if(!bitarray_test_bit(bitmap,i)){
			log_info(LOGGERFS,"El bloque %d esta libre", i);
			return i;
			}
		log_info(LOGGERFS,"El bloque: %d esta ocupado",i);
		}
	log_info(LOGGERFS,"No hay mas bloques libres");
	return -1;
}

int ocuparBloqueDelBitmap(int numeroDeBloque){
	if(!bitarray_test_bit(bitmap,numeroDeBloque)){
		log_info(LOGGERFS,"El bloque %d esta libre y lo voy a ocupar", numeroDeBloque);
		bitarray_set_bit(bitmap,numeroDeBloque);
		log_info(LOGGERFS,"Bloque %d ocupado",numeroDeBloque);
		return EXIT_SUCCESS;
	}else{
		log_error(LOGGERFS,"El bloque %d ya estaba ocupado",numeroDeBloque);
		return EXIT_FAILURE;
		}
}

int levantarBitMap(){
	FILE *archivoBitmap;
	bool estaCreadoElBitmap;
	bool estaVacioElBitmap=false;
	sizeDelBitmap=metadataDelFS.blocks * sizeof(char);
	log_info(LOGGERFS,"Buscando el archivo \"Bitmap.bin\" en el directorio: %s",archivoDeBitmap);

	int FDbitmap = open(archivoDeBitmap, O_RDWR);

	if(FDbitmap==-1){
		log_error(LOGGERFS,"No se pudo abrir el file descriptor del archivo de bitmap %s",archivoDeBitmap);
		archivoBitmap=fopen(archivoDeBitmap, "wb");
		for(int i=0;i<sizeDelBitmap;i++){
			fprintf(archivoBitmap,"0");
			}
		fclose(archivoBitmap);
		FDbitmap = open(archivoDeBitmap, O_RDWR);
		estaCreadoElBitmap=false;
	}else{
		log_info(LOGGERFS,"Se abrio el file descriptor del archivo de bitmap %s",archivoDeBitmap);
		estaCreadoElBitmap=true;
		}

	struct stat mystat;

	if(fstat(FDbitmap, &mystat) < 0) {
	    log_error(LOGGERFS,"Error al establecer fstat");
	    close(FDbitmap);
		}

	if(mystat.st_size==0){
		estaVacioElBitmap=true;
		log_error(LOGGERFS,"El archivo esta vacio y no tiene nada para mapearlo a memoria");
		close(FDbitmap);
		archivoBitmap=fopen(archivoDeBitmap, "wb");
		for(int i=0;i<sizeDelBitmap;i++){
			fprintf(archivoBitmap,"0");
			}
		fclose(archivoBitmap);
		FDbitmap = open(archivoDeBitmap, O_RDWR | O_CREAT, S_IRWXU );
	}else{
		estaVacioElBitmap=false;
		}

	srcMmap = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, FDbitmap, 0);

	if(srcMmap == MAP_FAILED){
		log_error(LOGGERFS,"Error al mapear a memoria: %s",strerror(errno));
		log_info(LOGGERFS,"Es probable que no este creado el archivo o este vacio, paso a crearlo y llenarlo con basura");
	}else{
		log_info(LOGGERFS,"MAP exitoso");
		}

	bufferArchivo = malloc(sizeDelBitmap);

	memcpy(bufferArchivo, srcMmap, sizeDelBitmap);

	if(!estaCreadoElBitmap||estaVacioElBitmap){
		log_info(LOGGERFS,"El archivo de bitmap no existia");
		log_info(LOGGERFS,"Generando el BITMAP con los datos de \"Bitmap.bin\" en: %s",archivoDeBitmap);
		bzero(bufferArchivo,metadataDelFS.blocks);
	}else{
		log_info(LOGGERFS,"El archivo de bitmap existia");
		}

	bitmap = bitarray_create_with_mode(bufferArchivo,sizeDelBitmap,MSB_FIRST);

	if(!estaCreadoElBitmap||estaVacioElBitmap){
		for(int i=0;i<metadataDelFS.blocks;i++){
			bitarray_clean_bit(bitmap,i);
			}

		}
	close(FDbitmap);
	return EXIT_SUCCESS;
}

int bajarADiscoBitmap(){
	memcpy(srcMmap,bufferArchivo,sizeDelBitmap);
	msync(bitmap, sizeDelBitmap, MS_SYNC);
	return EXIT_SUCCESS;
}

int imprimirEstadoDelBitmap(){
	int i;
	int bloquesLibres=0;
	int bloquesOcupados=0;
	printf("Cantidad total de bloques: %d\n",metadataDelFS.blocks);
	printf("Imprimiendo estado de los bloques\n");
	printf("Bloques libres:\n");
	pthread_mutex_lock(&mutexBitmap);
	for(i=0;i<metadataDelFS.blocks;i++){
		if(!bitarray_test_bit(bitmap,i)){
			printf("%d; ",i);
			bloquesLibres++;
			}
		}
	printf("\nBloques ocupados:\n");
	for(i=0;i<metadataDelFS.blocks;i++){
		if(bitarray_test_bit(bitmap,i)){
			printf("%d; ",i);
			bloquesOcupados++;
			}
		}
	printf("\n");
	pthread_mutex_unlock(&mutexBitmap);
	return EXIT_SUCCESS;
}

int inicializarEstructuras(){
	char* dirtectorio;
	dirtectorio=string_new();
	string_append(&dirtectorio, configuracionDelFS.puntoDeMontaje);
	string_append(&dirtectorio, "/Blocks");
	mkdir(dirtectorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGERFS,"Directorio %s creado", dirtectorio);
	free(dirtectorio);
	dirtectorio=string_new();
	string_append(&dirtectorio, configuracionDelFS.puntoDeMontaje);
	string_append(&dirtectorio, "/Tables");
	mkdir(dirtectorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGERFS,"Directorio %s creado", dirtectorio);
	free(dirtectorio);
	dirtectorio=string_new();
	string_append(&dirtectorio, configuracionDelFS.puntoDeMontaje);
	string_append(&dirtectorio, "/Metadata");
	mkdir(dirtectorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	log_info(LOGGERFS,"Directorio %s creado", dirtectorio);
	free(dirtectorio);
	return EXIT_SUCCESS;
}
