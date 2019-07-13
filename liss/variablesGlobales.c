/*
 * variablesGlobales.c
 *
 *  Created on: 30 mar. 2019
 *      Author: utnso
 */

#include "variablesGlobales.h"

t_metadataDelFS metadataDelFS;
t_configuracionDelFS configuracionDelFS;
t_log* LOGGERFS;
char* directorioConLaMetadata;//el directorio
char* archivoDeBitmap;
char* archivoDeLaMetadata;//el archivo
char* pathDeMontajeDelPrograma;
char* nombreArchivoInfoMsBloqueada;
pthread_t threadConsola, threadCompactador, threadDumps, threadMonitoreadorDeArchivos;
pthread_mutex_t mutexVariableTiempoDump, mutexVariableRetardo, mutexBitmap,
	mutexEstadoDeFinalizacionDelSistema, mutexDeLaMemtable, mutexDeDump;
t_bitarray *bitmap;
int sizeDelBitmap;
char * srcMmap;
char * bufferArchivo;
t_list* memTable;
t_list* dumpTables;
//tablas fs
pthread_mutex_t mutexListaTablasFS;
t_list* tablasFS;
//
bool estadoDeFinalizacionDelSistema;

pthread_t threadServer;//thread para el server de lissandra

bool esTablaNuevaFS(char* nuevaTabla){
	bool retorno=false;
	bool _stringCompare(void* elem){
		return !strcmp(((tp_tablaDeFS)elem)->nombreTabla,nuevaTabla);
	}
	pthread_mutex_lock(&mutexListaTablasFS);
	retorno = !list_any_satisfy(tablasFS,_stringCompare);
	pthread_mutex_unlock(&mutexListaTablasFS);
	return retorno;
}

void liberarTablaFS(tp_tablaDeFS tablaALiberar){
	pthread_rwlock_wrlock(tablaALiberar->mutexTabla);
	pthread_rwlock_unlock(tablaALiberar->mutexTabla);
	pthread_rwlock_destroy(tablaALiberar->mutexTabla);
	free(tablaALiberar->mutexTabla);
	free(tablaALiberar->nombreTabla);
	free(tablaALiberar);
}

t_list* bloquearListaDeTablasFS(){
	pthread_mutex_lock(&mutexListaTablasFS);
	return tablasFS;
}

void desbloquearListaDeTablasFS(){
	pthread_mutex_unlock(&mutexListaTablasFS);
}

void desbloquearSharedTablaFS(pthread_rwlock_t* mutexTabla){
	//printf("voy a desbloquear shared\n");
	pthread_rwlock_unlock(mutexTabla);
	//printf("desbloquee shared\n");
}
void desbloquearExclusiveTablaFS(pthread_rwlock_t* mutexTabla){
	//printf("voy a desbloquear exclusive\n");
	pthread_rwlock_unlock(mutexTabla);
	//printf("desbloquee exclusive\n");
}

pthread_rwlock_t* bloquearSharedTablaFS(char* nombreTabla){
	bool _stringCompare(void* elem){
		return !strcmp(((tp_tablaDeFS)elem)->nombreTabla,nombreTabla);
	}
	pthread_mutex_lock(&mutexListaTablasFS);
	tp_tablaDeFS tablaPedida = (tp_tablaDeFS)list_find(tablasFS,_stringCompare);
	pthread_mutex_unlock(&mutexListaTablasFS);//capaz conviene desbloquearlo dsps q bloquee la tabla@@
	if(tablaPedida!=NULL){
		//printf("voy a bloquear shared %s\n",nombreTabla);
		pthread_rwlock_rdlock(tablaPedida->mutexTabla);
		//printf("bloquee shared %s\n",nombreTabla);
		return tablaPedida->mutexTabla;
	} else return NULL;
}

pthread_rwlock_t* bloquearExclusiveTablaFS(char* nombreTabla){
	bool _stringCompare(void* elem){
		return !strcmp(((tp_tablaDeFS)elem)->nombreTabla,nombreTabla);
	}
	pthread_mutex_lock(&mutexListaTablasFS);
	tp_tablaDeFS tablaPedida = (tp_tablaDeFS)list_find(tablasFS,_stringCompare);
	pthread_mutex_unlock(&mutexListaTablasFS);//capaz conviene desbloquearlo dsps q bloquee la tabla@@
	if(tablaPedida!=NULL){
		//printf("voy a bloquear exclusive %s\n",nombreTabla);
		pthread_rwlock_wrlock(tablaPedida->mutexTabla);
		//printf("bloquee exclusive %s\n",nombreTabla);
		return tablaPedida->mutexTabla;
	} else return NULL;
}

bool eliminarDeListaDeTablasFS(char* tablaABorrar){
	bool eliminada=false;
	bool _stringCompare(void* elem){
		bool result =!strcmp(((tp_tablaDeFS)elem)->nombreTabla,tablaABorrar);
		if(result)eliminada=true;
		return result;
	}
	pthread_mutex_lock(&mutexListaTablasFS);
	list_remove_and_destroy_by_condition(tablasFS,_stringCompare,liberarTablaFS);
	pthread_mutex_unlock(&mutexListaTablasFS);
	return eliminada;
}

bool agregarAListaDeTablasFS(char* nuevaTabla){//tal vez deberia bloquear la tabla durante toda la ejecucion de esta funcion@@
	if(esTablaNuevaFS(nuevaTabla)){
		tp_tablaDeFS tabla = malloc(sizeof(t_tablaDeFS));
		tabla->nombreTabla = string_new();
		string_append(&(tabla->nombreTabla),nuevaTabla);
		tabla->mutexTabla = malloc(sizeof(pthread_rwlock_t));
		if(pthread_rwlock_init(tabla->mutexTabla, NULL) != 0) {
			log_error(LOGGERFS,"CUIDADO: No se pudo inicializar un semaforo para una tabla del FS");
			free(tabla->nombreTabla);
			free(tabla->mutexTabla);
			free(tabla);
			return false;
		}
		pthread_mutex_lock(&mutexListaTablasFS);
		list_add(tablasFS,tabla);
		pthread_mutex_unlock(&mutexListaTablasFS);
		return true;//la agrego
	}
	else return false;//esa tabla ya existe
}

char* recortarHastaUltimaBarraFS(char* path){
	int indice = (int)(strrchr(path,'/')-path)+1;
	char* ultimoNombre = string_substring_from(path,indice);
	return ultimoNombre;
}

int buscarTablasFS(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){

	if (ftwbuf->level == 0) return FTW_CONTINUE;
	if (ftwbuf->level > 1) return FTW_SKIP_SUBTREE;

	if(tflag == FTW_D) {
		char* path_nombre = recortarHastaUltimaBarraFS(fpath);

		if(agregarAListaDeTablasFS(path_nombre)){
			//encontre una tabla nueva
			log_info(LOGGERFS,"Se carga la tabla %s",path_nombre);
		}
		free(path_nombre);
		return FTW_SKIP_SUBTREE;
	}

	return FTW_CONTINUE;
}

void buscarTablasYaCreadasFS(){
	char* main_directorio=string_new();
	string_append(&main_directorio, configuracionDelFS.puntoDeMontaje);
	string_append(&main_directorio, "/Tables/");
	nftw(main_directorio,buscarTablasFS,20,FTW_ACTIONRETVAL|FTW_PHYS);
	free(main_directorio);
}

int inicializarVariablesGlobales(){
	configuracionDelFS.puertoEscucha=-1;
	configuracionDelFS.puntoDeMontaje=NULL;
	configuracionDelFS.retardo=-1;
	configuracionDelFS.sizeValue=-1;
	configuracionDelFS.tiempoDump=-1;
	metadataDelFS.blockSize=-1;
	metadataDelFS.blocks=-1;
	metadataDelFS.magicNumber=NULL;
	LOGGERFS=NULL;
	directorioConLaMetadata=NULL;
	pathDeMontajeDelPrograma=NULL;
	nombreArchivoInfoMsBloqueada = "MSBlocked.info";
	if(pthread_mutex_init(&mutexVariableRetardo, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexVariableRetardo");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexVariableTiempoDump, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexVariableTiempoDump");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexBitmap, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexBitmap");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexEstadoDeFinalizacionDelSistema, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexEstadoDeFinalizacionDelSistema");
		return EXIT_FAILURE;
	}else{
		setearEstadoDeFinalizacionDelSistema(false);
		}
	if(pthread_mutex_init(&mutexDeLaMemtable, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexDeLaMemtable");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexDeDump, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexDeOperacionCritica");
		return EXIT_FAILURE;
		}
	if(pthread_mutex_init(&mutexListaTablasFS, NULL) != 0) {
		log_error(LOGGERFS,"No se pudo inicializar el semaforo mutexListaTablasFS");
		return EXIT_FAILURE;
		}

	bitmap=NULL;
	sizeDelBitmap=-1;
	bufferArchivo=NULL;
	archivoDeBitmap=NULL;
	archivoDeLaMetadata=NULL;
	memTable=list_create();
	dumpTables=list_create();
	tablasFS= list_create();
	return EXIT_SUCCESS;
}

void liberarRecursos(){
	log_info(LOGGERFS,"Liberando recursos");

	if(configuracionDelFS.puntoDeMontaje!=NULL)free(configuracionDelFS.puntoDeMontaje);
	if(metadataDelFS.magicNumber!=NULL)free(metadataDelFS.magicNumber);
	free(pathDeMontajeDelPrograma);
	free(directorioConLaMetadata);
	free(archivoDeBitmap);
	free(archivoDeLaMetadata);
	bajarADiscoBitmap();
	log_info(LOGGERFS,"Destruyendo el bitarray");
	pthread_mutex_lock(&mutexBitmap);
	bitarray_destroy(bitmap);
	//free(srcMmap);
	free(bufferArchivo);
	vaciarMemTable();
	vaciarDumpTable();
	log_destroy(LOGGERFS);
	pthread_mutex_destroy(&mutexVariableRetardo);
	pthread_mutex_destroy(&mutexVariableTiempoDump);
	pthread_mutex_destroy(&mutexDeLaMemtable);
	pthread_mutex_destroy(&mutexBitmap);
	pthread_mutex_destroy(&mutexDeDump);
	pthread_mutex_destroy(&mutexEstadoDeFinalizacionDelSistema);

	pthread_mutex_lock(&mutexListaTablasFS);
	list_destroy_and_destroy_elements(tablasFS,liberarTablaFS);
	pthread_mutex_unlock(&mutexListaTablasFS);
	pthread_mutex_destroy(&mutexListaTablasFS);
	printf("Memoria liberada\nPROGRAMA FINALIZADO CORRECTAMENTE\n");
	return;
}

int vaciarMemTable(){
	void destruirTabla(void* nodoDeLaMemtable){
		void vaciarNodos(void* nodoDeUnaTabla){
			free(((tp_nodoDeLaTabla)nodoDeUnaTabla)->value);
			free((tp_nodoDeLaTabla)nodoDeUnaTabla);
			}
		if(!list_is_empty(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla)){
			log_info(LOGGERFS,"Liberando la tabla: %s",
				((tp_nodoDeLaMemTable)nodoDeLaMemtable)->nombreDeLaTabla);
			list_iterate(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla,
				vaciarNodos);
			free(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->nombreDeLaTabla);
			list_destroy(((tp_nodoDeLaMemTable)nodoDeLaMemtable)->listaDeDatosDeLaTabla);
			free((tp_nodoDeLaMemTable)nodoDeLaMemtable);
			}
		}
	log_info(LOGGERFS,"Liberando memtable");
	if(memTable!=NULL){
		if(!list_is_empty(memTable)){
			log_info(LOGGERFS,"La memtable tiene datos asi q paso a liberarla");
			list_clean_and_destroy_elements(memTable,destruirTabla);
		}else{
			log_info(LOGGERFS,"La memtable no tenia datos asi q no libero nada");
			}
		list_destroy(memTable);
	}else{
		log_error(LOGGERFS,"La memtable esta NULL");
		}

	log_info(LOGGERFS,"Memtable liberada");
	return EXIT_SUCCESS;
}

int vaciarDumpTable(){
	//implementar
	return EXIT_SUCCESS;
}
