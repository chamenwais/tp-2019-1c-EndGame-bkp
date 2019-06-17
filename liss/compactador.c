/*
 * compactador.c
 *
 *  Created on: 17 may. 2019
 *      Author: franco
 */
#include "compactador.h"
t_list* tablas;//lista de strings con las tablas q estan detectadas, solo los nombres NO todo el path
pthread_mutex_t tablas_mutex;

int lanzarCompactador(){

	log_info(LOGGERFS,"Iniciando hilo de compactador de lissandra");
	int resultado = pthread_create( &threadCompactador, NULL, crearCompactadorLissandra, NULL);
	pthread_detach(threadCompactador);

	if(resultado){
		log_error(LOGGERFS,"Error al crear hilo del compactador de lissandra, return code: %d",
				resultado);
		exit(EXIT_FAILURE);
	}else{
		log_info(LOGGERFS,"Hilo compactador de lissandra creado exitosamente");
		return EXIT_SUCCESS;
	}
}
bool esTablaNueva(char* nuevaTabla){
	bool retorno=false;
	bool _stringCompare(void* elem){
		return !strcmp((char*)elem,nuevaTabla);
	}
	pthread_mutex_lock(&tablas_mutex);
	retorno = !list_any_satisfy(tablas,_stringCompare);
	pthread_mutex_unlock(&tablas_mutex);
	return retorno;
}

bool agregarATablas(char* nuevaTabla){
	if(esTablaNueva(nuevaTabla)){
		//char* nueva = string_new();
		//string_append(&nueva,nuevaTabla);
		pthread_mutex_lock(&tablas_mutex);
		list_add(tablas,nuevaTabla);
		pthread_mutex_unlock(&tablas_mutex);
		return true;
	}
	else return false;
}
void eliminarDeTablas(char* tablaABorrar){
	bool _stringCompare(void* elem){
		return !strcmp((char*)elem,tablaABorrar);
	}
	pthread_mutex_lock(&tablas_mutex);
	list_remove_and_destroy_by_condition(tablas,_stringCompare,free);
	pthread_mutex_unlock(&tablas_mutex);
}

void* crearCompactadorLissandra(){
	log_info(LOGGERFS,"Iniciando compactador de lissandra");

	log_info(LOGGERFS,"[Compactador]Compactador listo");

	tablas = list_create();

	if(pthread_mutex_init(&tablas_mutex, NULL) != 0) {
		log_error(LOGGERFS,"[Compactador]No se pudo inicializar el semaforo tablas_mutex");
		pthread_exit(0);
	}

	while(1){
		sleep(5);//tmb podria chequear por cambios en la carpeta para arrancar
		compactarNuevasTablas();
	}

	log_info(LOGGERFS,"[Compactador]Finalizando compactador");
	pthread_exit(0);
}

void compactarNuevasTablas(){
	//log_info(LOGGERFS,"[Compactador]Busco nuevas tablas");
	char* main_directorio=string_new();
	string_append(&main_directorio, configuracionDelFS.puntoDeMontaje);
	string_append(&main_directorio, "/Tables/");
	nftw(main_directorio,crearCompactadorDeTablas,20,FTW_ACTIONRETVAL|FTW_PHYS);
	free(main_directorio);
}

int crearCompactadorDeTablas(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){

	if (ftwbuf->level == 0) return FTW_CONTINUE;
	if (ftwbuf->level > 1) return FTW_SKIP_SUBTREE;

	if(tflag == FTW_D) {
		char* path_nombre;//NO hacer free, la list se queda con sus datos
		path_nombre = recortarHastaUltimaBarra(fpath);

		if(agregarATablas(path_nombre)){
			//lanzo un thread que controle la compactacion en esa tabla
			log_info(LOGGERFS,"[Compactador]Iniciando thread Compactador para la tabla %s",path_nombre);

			pthread_t* compactador = (pthread_t*) malloc(sizeof(pthread_t));
			int resultadoDeCrearHilo = pthread_create( compactador, NULL, compactadorTabla, (void*)path_nombre);
			pthread_detach(*compactador);
			free(compactador);
			if(resultadoDeCrearHilo)
				log_error(LOGGERFS,"[Compactador]Error al crear thread de compactador para la tabla %s, return code: %d",path_nombre,resultadoDeCrearHilo);
		}

		return FTW_SKIP_SUBTREE;
	}

	return FTW_CONTINUE;
}

int obtenerTiempoCompactacion(char* nombreTabla){
	int tiempoCompactacion=-1;

	char* nombreDelArchivoDeMetaData=string_new();
	string_append(&nombreDelArchivoDeMetaData, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivoDeMetaData, "/Tables/");
	string_append(&nombreDelArchivoDeMetaData, nombreTabla);
	string_append(&nombreDelArchivoDeMetaData, "/Metadata");

	t_config* configuracion = config_create(nombreDelArchivoDeMetaData);
	if(configuracion==NULL){
		free(nombreDelArchivoDeMetaData);
		return -1;
	}
	//Recupero COMPACTION_TIME
	if(!config_has_property(configuracion,"COMPACTION_TIME")) {
		log_error(LOGGERFS,"[Compactador]No esta COMPACTION_TIME en el archivo de metadata %s",nombreDelArchivoDeMetaData);
		config_destroy(configuracion);
		free(nombreDelArchivoDeMetaData);
		return -1;
	}
	tiempoCompactacion = config_get_int_value(configuracion,"COMPACTION_TIME");
	config_destroy(configuracion);
	free(nombreDelArchivoDeMetaData);
	return tiempoCompactacion;
}

bool existeTmp(char* tabla){
	bool existe=false;
	char* main_directorio=string_new();
	string_append(&main_directorio, configuracionDelFS.puntoDeMontaje);
	string_append(&main_directorio, "/Tables/");
	string_append(&main_directorio, tabla);
	string_append(&main_directorio, "/");

	int buscarTmp(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
		int length=strlen(fpath);
		if(length<4)return FTW_CONTINUE;
		length--;
		if(fpath[length-3]=='.' && fpath[length-2]=='t' && fpath[length-1]=='m' && fpath[length]=='p'){
			existe=true;
			return FTW_STOP;
		}
		return FTW_CONTINUE;
	}
	nftw(main_directorio,buscarTmp,20,FTW_ACTIONRETVAL|FTW_PHYS);
	return existe;
}

void* compactadorTabla(char* tabla){//solo recibe el nombre, necesita configuracionDelFS.puntoDeMontaje+/Tables/ si quiero el path completo

	int tiempoCompactacion=0;

	while(1){
		tiempoCompactacion = obtenerTiempoCompactacion(tabla);

		if(tiempoCompactacion==-1){
			log_info(LOGGERFS,"[Compactador]Finalizando thread Compactador para la tabla %s",tabla);
			eliminarDeTablas(tabla);
			pthread_exit(0);
		}
		if(existeTmp(tabla)){
			//hay archivos para compactar
		}



		usleep(tiempoCompactacion*1000);
	}

}





