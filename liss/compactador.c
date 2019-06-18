/*
 * compactador.c
 *
 *  Created on: 17 may. 2019
 *      Author: franco
 */
#include "compactador.h"
t_list* tablas;//lista de strings con las tablas q estan detectadas, solo los nombres NO todo el path
pthread_mutex_t tablas_mutex,nextTmp;

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

	if(pthread_mutex_init(&nextTmp, NULL) != 0) {
		log_error(LOGGERFS,"[Compactador]No se pudo inicializar el semaforo nextTmp");
		pthread_exit(0);
	}

	while(1){
		sleep(5);//tmb podria chequear por cambios en la carpeta para arrancar
		//compactarNuevasTablas(); esta funcion inicia la compatacion@@@@@
	}

	while(pthread_mutex_destroy(&tablas_mutex));
	while(pthread_mutex_destroy(&nextTmp));
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
		free(path_nombre);
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

t_list* obtenerTmps(char* tabla){
	t_list* temps=list_create();
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
			char* temp=string_new();
			string_append(&temp,fpath);
			list_add(temps,temp);
			return FTW_CONTINUE;
		}
		return FTW_CONTINUE;
	}
	nftw(main_directorio,buscarTmp,20,FTW_ACTIONRETVAL|FTW_PHYS);

	free(main_directorio);
	return temps;
}

void* compactadorTabla(char* tabla){//solo recibe el nombre, necesita configuracionDelFS.puntoDeMontaje+/Tables/ si quiero el path completo

	int tiempoCompactacion=0;
	//t_list* bins= list_create();//se q adentro de los bloques de los bins no hay duplicados
	//tmb se q estos son los nros originales de bloques de la tabla, no se si me sirve pa algo dsps
	t_list* tmps;//se q tengo q chequear por duplicados, tmb se q estos bloques son auxiliares y no "pertencen" a la tabla original
	//@@@@x ahora toy operando un tmp a la vez, hacerlo en grupo dsps
	while(1){

		tiempoCompactacion = obtenerTiempoCompactacion(tabla);
		if(tiempoCompactacion==-1){
			log_info(LOGGERFS,"[Compactador]Finalizando thread Compactador para la tabla %s",tabla);
			eliminarDeTablas(tabla);
			pthread_exit(0);
		}

		tmps=obtenerTmps(tabla);
		if(tmps->elements_count!=0){
			//hay archivos para compactar
			//le saco sus blocks y el size total

			//Renombrar a tmpc
			for(int i=0;i<tmps->elements_count;i++){
				char* tmpc = string_new();
				string_append(&tmpc,(char*)list_get(tmps,i));
				string_append(&tmpc,"c");
				if(rename((char*)list_get(tmps,i),tmpc)){
					//hubo error
					free(tmpc);
					log_error(LOGGERFS,"[Compactador]No se pudo renombrar el archivo [%s] a [%s], finalizando compactador para la tabla [%s]",(char*)list_get(tmps,i),tmpc,tabla);
					eliminarDeTablas(tabla);
					pthread_exit(0);
				}
				list_replace_and_destroy_element(tmps,i,tmpc,free);
			}

			//creo temp para tmpc
			char* tempTmpc = crearTempParaTmpcs(tmps);//preferiria ir leyendo directo de los archivos pero lo dejo asi x ahora

			t_list* tempBins = crearTempsParaBins(tabla);//le paso la tabla y cada archivo bin q tenga tiene varios bloques

			if(tempBins==NULL){
				log_info(LOGGERFS,"[Compactador]Error al crear temporal para los bins de la tabla [%s], finalizando compactador para esta tabla",tabla);
				eliminarDeTablas(tabla);
				pthread_exit(0);
			}

			//Empieza la comparacion
			t_list* particionesComparadas = compararBinsContraTmpcs(tempBins,tempTmpc);
			char** superString = malloc(sizeof(char*)*particionesComparadas->elements_count);
			for(int j=0;j<particionesComparadas->elements_count;j++)
				superString[j]=convertirTKVsAString(list_get(particionesComparadas,j));
			//@@ver q pasa si uno ta vacio
			//aca tengo una lista de lista con cada particion en orden con todos sus datos

			//@@primero chequear q la tabla siga existiendo

			//@@@@@@ahora ya tengo todo actualizado, queda eliminar lo viejo y cargar estos nuevos
			//@@liberar particionesComparadas y superString
			//@@usar mutexDeCompactacion
			//Al final borro los archivos temporales q tuve q crear
			remove(tempTmpc);
			for(int j=0;j<tempBins->elements_count;j++)remove((char*)list_get(tempBins,j));
			free(tempTmpc);
			list_destroy_and_destroy_elements(tempBins,free);
			list_destroy_and_destroy_elements(tmps,free);

			//podria esperar un cacho antes de borrar los bins originales y los de tmpc, por si otro thread los abrio antes q yo pueda bloquear la tabla

		}else list_destroy(tmps);

		usleep(tiempoCompactacion*1000);
	}
	//NO hacer free de tabla
}

char* convertirTKVsAString(t_list* particion){
	char* super=string_new();
	for(int i=0;i<particion->elements_count;i++){
		tp_tkv nodo =(tp_tkv)list_get(particion,i);
		char* auxitoa = string_itoa(nodo->timeStamp);
		string_append(&super, auxitoa);
		free(auxitoa);
		string_append(&super, ";");
		auxitoa=string_itoa(nodo->key);
		string_append(&super, auxitoa);
		free(auxitoa);
		string_append(&super, ";");
		string_append(&super, nodo->value);
		string_append(&super, "\n");
	}
}

t_list* compararBinsContraTmpcs(t_list* binsFiles,char* tmpcsFile){
	//FILE* archivotempBins=fopen(todos los tempBins @@@,"r");
	//FILE* archivotempTmpc=fopen(tempTmpc,"r");
	t_list* bins=list_create();//lista de listas, cada elemento es una particion desde la 0 en adelante
	for(int j=0;j<binsFiles->elements_count;j++){
		list_add(bins,cargarTimeStampKeyValue((char*)list_get(binsFiles,j)));
	}
	t_list* tmpcs= cargarTimeStampKeyValue(tmpcsFile);
	tp_tkv nuevoTKV;
	while((nuevoTKV=list_remove(tmpcs,0))!=NULL){

		bool buscarKey(tp_tkv tkv){
			return tkv->key==nuevoTKV->key;
		}
		tp_tkv originalTKV = (tp_tkv)list_find((t_list*)list_get(bins,nuevoTKV->key % bins->elements_count),buscarKey);
		if(originalTKV!=NULL){
			if(nuevoTKV->timeStamp>originalTKV->timeStamp){
				originalTKV->timeStamp=nuevoTKV->timeStamp;
				free(originalTKV->value);
				originalTKV->value=malloc(strlen(nuevoTKV->value)+1);
				strcpy(originalTKV->value,nuevoTKV->value);
				//modifico el tkv de la lista
				liberarTKV(nuevoTKV);
			}
		}
		else{
			list_add((t_list*)list_get(bins,nuevoTKV->key % bins->elements_count),nuevoTKV);
			//no libero el tkv xq es este mismo el q va a ir a la otra lista
			//agregar el nuevo
		}
	}
	list_destroy(tmpcs);
	return bins;
}

void liberarTKV(tp_tkv tkv){
	free(tkv->value);
	free(tkv);
}
/*
tp_viejoTimestamp buscarKey(uint16_t key,t_list* particion){
	tp_viejoTimestamp original;
	tp_tkv tkvoriginal;
	for(int i=0;i<particion->elements_count;i++){
		tkvoriginal = (tp_tkv)list_get(particion,i);
		if(tkvoriginal->key==key){
			original=malloc(sizeof(t_viejoTimestamp));
			original->index=i;
			original->timeStamp=tkvoriginal->timeStamp;
			return original;
		}
	}
	return NULL;
}
*/
t_list* cargarTimeStampKeyValue(char* path){
	t_list* listaResultante = list_create();
	tp_tkv nuevoNodo;
	FILE* archivo = fopen(path,"r");
	char** lineaParseada;
	char *linea = NULL;
	char *aux = NULL;
	size_t linea_buf_size = 0;
	ssize_t linea_size;

	linea_size = getline(&aux, &linea_buf_size, archivo);
	while (linea_size >= 0){
		linea=(string_split(aux,"\n"))[0]; //hago esto para sacarle el \n
		lineaParseada = string_split(linea, ";");
		nuevoNodo=malloc(sizeof(t_tkv));
		nuevoNodo->timeStamp=atoi(lineaParseada[0]);
		nuevoNodo->key=atoi(lineaParseada[1]);
		nuevoNodo->value=malloc(strlen(lineaParseada[2])+1);
		strcpy(nuevoNodo->value,lineaParseada[2]);
		list_add(listaResultante,nuevoNodo);
		for(int j=0;lineaParseada[j]!=NULL;j++) free(lineaParseada[j]);
		free(lineaParseada);
		linea_size = getline(&aux, &linea_buf_size, archivo);
	}
	fclose(archivo);
	return listaResultante;
}

char* crearTempParaTmpcs(t_list* tmpc){
	char* directorioDeTrabajo= string_new();//@recordar hacer free
	string_append(&directorioDeTrabajo,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeTrabajo,"/Blocks/");

	char* archivoTempUbicacion = string_new();//@recordar hacer free
	string_append(&archivoTempUbicacion,directorioDeTrabajo);
	char* tempBlock = getNextTemp();
	string_append(&archivoTempUbicacion,tempBlock);
	free(tempBlock);

	FILE* archivoTemp=fopen(archivoTempUbicacion,"w");//aca meto todo

	for(int j=0;j<tmpc->elements_count;j++){
		t_config* tmpc_conf = config_create((char*)list_get(tmpc,j));
		char** arrayDeBloques = config_get_array_value(tmpc_conf,"BLOCKS");
		config_destroy(tmpc_conf);

		for(int i=0;arrayDeBloques[i]!=NULL;i++){
			char ch;
			char* ubicacionDelBloque=string_new();
			string_append(&ubicacionDelBloque,directorioDeTrabajo);
			string_append(&ubicacionDelBloque,arrayDeBloques[i]);
			string_append(&ubicacionDelBloque,".bin");
			log_info(LOGGERFS,"[Compactador]Voy a unir las keys del bloque %s",ubicacionDelBloque);//borrar
			FILE* archivoDeBloque=fopen(ubicacionDelBloque,"r");
			while((ch =fgetc(archivoDeBloque))!=EOF)
				fputc(ch, archivoTemp);
			fclose(archivoDeBloque);
			free(ubicacionDelBloque);
			free(arrayDeBloques[i]);
		}
	}

	fclose(archivoTemp);//ya cargue todos los datos del tmpc aca
	free(directorioDeTrabajo);
	free(archivoTempUbicacion);
	return archivoTempUbicacion;
}
t_list* crearTempsParaBins(char* tabla){
	char* binsMeta = string_new();
	string_append(&binsMeta, configuracionDelFS.puntoDeMontaje);
	string_append(&binsMeta, "/Tables/");
	string_append(&binsMeta, tabla);
	string_append(&binsMeta,"/Metadata");
	t_config* bins_metadata = config_create(binsMeta);
	if(bins_metadata==NULL) return NULL;
	if(!config_has_property(binsMeta,"PARTITIONS"))return NULL;
	int cantBins = config_get_int_value(binsMeta,"PARTITIONS");
	config_destroy(bins_metadata);
	free(binsMeta);
	int length;

	t_list* tempBins=list_create();//la lista va a estar ordenada: ej el primer elem es un file q referencia a los contenidos de 0.bin, el 2do a 1.bin, etc

	char* directorioDeTrabajo= string_new();//@recordar hacer free
	string_append(&directorioDeTrabajo,configuracionDelFS.puntoDeMontaje);
	string_append(&directorioDeTrabajo,"/Blocks/");


	for(int i=0;i<cantBins;i++){
		//por cada .bin desde 0 hasta cantBins-1 tengo q obtener sus bloques y meterlos en el super bloque
		length= snprintf( NULL, 0, "%d", i );
		char* str = malloc( length + 1 );
		snprintf( str, length + 1, "%d", i );
		char* bin = string_new();
		string_append(&bin,tabla);
		string_append(&bin,str);
		string_append(&bin,".bin");
		free(str);
		t_config* bin_conf = config_create(bin);
		char** arrayDeBloques = config_get_array_value(bin_conf,"BLOCKS");
		config_destroy(bin_conf);
		free(bin);

		//@
		char* archivoTempUbicacion = string_new();
		string_append(&archivoTempUbicacion,directorioDeTrabajo);
		string_append(&archivoTempUbicacion,getNextTemp());
		FILE* archivoTemp=fopen(archivoTempUbicacion,"w");//aca meto todo

		for(int i=0;arrayDeBloques[i]!=NULL;i++){
			char ch;
			char* ubicacionDelBloque=string_new();
			string_append(&ubicacionDelBloque,directorioDeTrabajo);
			string_append(&ubicacionDelBloque,arrayDeBloques[i]);
			string_append(&ubicacionDelBloque,".bin");
			log_info(LOGGERFS,"[Compactador]Voy a unir las keys del bloque %s",ubicacionDelBloque);//borrar
			FILE* archivoDeBloque=fopen(ubicacionDelBloque,"r");
			while((ch =fgetc(archivoDeBloque))!=EOF)
				fputc(ch, archivoTemp);
			fclose(archivoDeBloque);
			free(ubicacionDelBloque);
			free(arrayDeBloques[i]);
		}
		//free(archivoTempUbicacion);
		list_add(tempBins,(void*)archivoTempUbicacion);
		fclose(archivoTemp);//ya cargue todos los datos de los bins aca
	}
	free(directorioDeTrabajo);
	return tempBins;
}

char* getNextTemp(){//creo q seria mejor meter todo en mem y listo
	static int tmp;
	int length;
	char* nextTemp=string_new();
	pthread_mutex_lock(&nextTmp);
	tmp++;
	length = snprintf( NULL, 0, "%d", tmp );
	char* str = malloc( length + 1 );
	snprintf( str, length + 1, "%d", tmp );
	pthread_mutex_unlock(&nextTmp);
	string_append(&nextTemp,str);
	string_append(&nextTemp,".atmp");
	free(str);
	return nextTemp;
}



