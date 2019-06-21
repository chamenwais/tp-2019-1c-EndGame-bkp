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
		sleep(5);//puedo chequear por cambios en la carpeta para arrancar pero no me convence

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
	t_list* tmps;//lista de los tmp q van a entrar en compactacion, y se van a convertir en tmpc
	int cantidadDeCompactaciones=0;
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
					list_destroy_and_destroy_elements(tmps,free);
					log_error(LOGGERFS,"[Compactador]No se pudo renombrar el archivo [%s] a [%s], finalizando compactador para la tabla [%s]",(char*)list_get(tmps,i),tmpc,tabla);
					eliminarDeTablas(tabla);
					pthread_exit(0);
				}
				list_replace_and_destroy_element(tmps,i,tmpc,free);
			}

			//creo temp para tmpc
			char* tempTmpc = crearTempParaTmpcs(tmps);//mi nuevo tmp
			if(tempTmpc==NULL){
				log_info(LOGGERFS,"[Compactador]Error al crear temporal para los tmpc de la tabla [%s], finalizando compactador para esta tabla",tabla);
				list_destroy_and_destroy_elements(tmps,free);
				eliminarDeTablas(tabla);
				pthread_exit(0);
			}

			t_list* tempBins = crearTempsParaBins(tabla);//le paso la tabla y cada archivo bin q tenga tiene varios bloques
			if(tempBins==NULL){
				log_info(LOGGERFS,"[Compactador]Error al crear temporal para los bins de la tabla [%s], finalizando compactador para esta tabla",tabla);
				list_destroy_and_destroy_elements(tmps,free);
				free(tempTmpc);
				eliminarDeTablas(tabla);
				pthread_exit(0);
			}

			//Comparacion
			t_list* particionesComparadas = compararBinsContraTmpcs(tempBins,tempTmpc);
			//
			remove(tempTmpc);
			for(int j=0;j<tempBins->elements_count;j++)remove((char*)list_get(tempBins,j));
			free(tempTmpc);
			list_destroy_and_destroy_elements(tempBins,free);

			char** superString = malloc(sizeof(char*)*particionesComparadas->elements_count);
			for(int j=0;j<particionesComparadas->elements_count;j++)
				superString[j]=convertirTKVsAString(list_get(particionesComparadas,j));
			int cantidadParticiones =particionesComparadas->elements_count;
			list_destroy_and_destroy_elements(particionesComparadas,liberarListaTKV);
			//aca tengo una lista de string con cada particion en orden con todos sus datos

			//@@ver q pasa si uno ta vacio

			//PASOS:
			//1.bloquear la tabla@@
			clock_t inicio = clock();

			//2.liberar bloques tmpc
			list_iterate(tmps,liberarBloquesTmpc);
			list_destroy_and_destroy_elements(tmps,free);
			//3.liberar bloques bin
			liberarBloquesYParticiones(tabla);
			//4.pedir bloques para el nuevo bin
			t_list* bloquesPorParticion = list_create();
			for(int i=0;i<cantidadParticiones;i++){
				t_list* bloques =insertarCadenaEnNuevosBloques(superString[i]);
				if(bloques==NULL){
					//no hay mas bloques en el fs :(
					list_iterate(bloquesPorParticion,liberarBloquesDelBitmap);//si ya habia agarrado algun bloque lo libero
					bajarADiscoBitmap();
					list_destroy_and_destroy_elements(bloquesPorParticion,list_destroy);
					log_info(LOGGERFS,"[Compactador]Deteniendo compactador de la tabla [%s], no quedan bloques en el fs",tabla);

					for(int j=0;j<cantidadParticiones;j++){
						log_info(LOGGERFS,"[Compactador]Tabla= %s , Particion= %d , Datos perdidos= %s",tabla,j,superString[j]);
						free(superString[j]);//@por ahora tiro toda la info de la tabla
					}
					eliminarDeTablas(tabla);
					pthread_exit(0);
				}
				else
					list_add(bloquesPorParticion,bloques);
			}
			//5.cargar datos del nuevo bin
			int result;
			for(int i=0;i<cantidadParticiones;i++){
				result=cargarParticionATabla(tabla,i,strlen(superString[i]),list_get(bloquesPorParticion,i));
				if(result==0){
					//la tabla ya no existe
					list_iterate(bloquesPorParticion,liberarBloquesDelBitmap);
					bajarADiscoBitmap();
					list_destroy_and_destroy_elements(bloquesPorParticion,list_destroy);
					for(int j=0;j<cantidadParticiones;j++)
						free(superString[j]);
					log_info(LOGGERFS,"[Compactador]La tabla [%s] dejo de existir durante su compactacion, deteniendo compactador para ella",tabla);
					eliminarDeTablas(tabla);
					pthread_exit(0);
				}
			}
			list_destroy_and_destroy_elements(bloquesPorParticion,list_destroy);
			for(int j=0;j<cantidadParticiones;j++)
				free(superString[j]);
			//6.desbloquear tabla y dejar registro tiempo q la tabla tuvo bloqueada@@
			clock_t tiempoBloqueada = clock() - inicio;
			int msec = tiempoBloqueada * 1000 / CLOCKS_PER_SEC;
			cantidadDeCompactaciones++;
			if(cantidadDeCompactaciones>10)cantidadDeCompactaciones=0;
			guardarMilisegundosBloqueada(tabla,msec,!cantidadDeCompactaciones);
			//@@usar mutexDeCompactacion

		}else list_destroy(tmps);

		usleep(tiempoCompactacion*1000);
	}
	//NO hacer free de tabla
}

void guardarMilisegundosBloqueada(char* nombreTabla,int milisegundos,bool resetFile){
	char* archivoMSBloqueada=string_new();
	string_append(&archivoMSBloqueada,configuracionDelFS.puntoDeMontaje);
	string_append(&archivoMSBloqueada, "/Tables/");
	string_append(&archivoMSBloqueada, nombreTabla);
	string_append(&archivoMSBloqueada, "/");
	string_append(&archivoMSBloqueada, "MSBlocked.info");
	FILE* archivo;
	if(resetFile) archivo = fopen(archivoMSBloqueada,"w");
	else archivo = fopen(archivoMSBloqueada,"a");
	if(archivo==NULL){
		free(archivoMSBloqueada);
		return;
	}
	fprintf(archivo,"MILLISECONDS=%d\n",milisegundos);
	fclose (archivo);
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
	return super;
}

t_list* compararBinsContraTmpcs(t_list* binsFiles,char* tmpcsFile){//ya se q todo lo q recibo existe xq lo cree yo

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

void liberarListaTKV(t_list* listaTkv){
	list_destroy_and_destroy_elements(listaTkv,liberarTKV);
}

void liberarTKV(tp_tkv tkv){
	free(tkv->value);
	free(tkv);
}

t_list* cargarTimeStampKeyValue(char* path){//ya se q este path existe xq lo cree yo
	t_list* listaResultante = list_create();
	tp_tkv nuevoNodo;
	FILE* archivo = fopen(path,"r");
	if(archivo==NULL){
		log_error(LOGGERFS,"[Compactador]El archivo %s que deberia haber creado antes ya no existe",path);
	}
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
		if(tmpc_conf==NULL){
			free(directorioDeTrabajo);
			fclose(archivoTemp);
			remove(archivoTempUbicacion);
			free(archivoTempUbicacion);
			return NULL;
		}
		if(!config_has_property(tmpc_conf,"BLOCKS")){
			free(directorioDeTrabajo);
			fclose(archivoTemp);
			remove(archivoTempUbicacion);
			free(archivoTempUbicacion);
			config_destroy(tmpc_conf);
			return NULL;
		}
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
			if(archivoDeBloque==NULL){
				free(directorioDeTrabajo);
				fclose(archivoTemp);
				remove(archivoTempUbicacion);
				free(archivoTempUbicacion);
				free(ubicacionDelBloque);
				for(int j=i;arrayDeBloques[j]!=NULL;j++)
					free(arrayDeBloques[j]);
				return NULL;
			}

			while((ch =fgetc(archivoDeBloque))!=EOF)
				fputc(ch, archivoTemp);
			fclose(archivoDeBloque);
			free(ubicacionDelBloque);
			free(arrayDeBloques[i]);
		}
	}

	fclose(archivoTemp);//ya cargue todos los datos del tmpc aca
	free(directorioDeTrabajo);
	//free(archivoTempUbicacion);
	return archivoTempUbicacion;
}
t_list* crearTempsParaBins(char* tabla){
	char* binsMeta = string_new();
	string_append(&binsMeta, configuracionDelFS.puntoDeMontaje);
	string_append(&binsMeta, "/Tables/");
	string_append(&binsMeta, tabla);
	string_append(&binsMeta,"/Metadata");
	t_config* bins_metadata = config_create(binsMeta);
	free(binsMeta);
	if(bins_metadata==NULL) return NULL;
	if(!config_has_property(bins_metadata,"PARTITIONS")){
		config_destroy(bins_metadata);
		return NULL;
	}
	int cantBins = config_get_int_value(bins_metadata,"PARTITIONS");
	config_destroy(bins_metadata);
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
		free(bin);
		if(bin_conf==NULL){
			list_destroy_and_destroy_elements(tempBins,free);
			free(directorioDeTrabajo);
			return NULL;
		}
		if(!config_has_property(bin_conf,"BLOCKS")){
			list_destroy_and_destroy_elements(tempBins,free);
			free(directorioDeTrabajo);
			config_destroy(bin_conf);
			return NULL;
		}
		char** arrayDeBloques = config_get_array_value(bin_conf,"BLOCKS");
		config_destroy(bin_conf);

		//@
		char* archivoTempUbicacion = string_new();
		string_append(&archivoTempUbicacion,directorioDeTrabajo);
		char* nextTemp =getNextTemp();
		string_append(&archivoTempUbicacion,nextTemp);
		free(nextTemp);
		FILE* archivoTemp=fopen(archivoTempUbicacion,"w");//aca meto todo

		for(int i=0;arrayDeBloques[i]!=NULL;i++){
			char ch;
			char* ubicacionDelBloque=string_new();
			string_append(&ubicacionDelBloque,directorioDeTrabajo);
			string_append(&ubicacionDelBloque,arrayDeBloques[i]);
			string_append(&ubicacionDelBloque,".bin");
			log_info(LOGGERFS,"[Compactador]Voy a unir las keys del bloque %s",ubicacionDelBloque);//borrar
			FILE* archivoDeBloque=fopen(ubicacionDelBloque,"r");
			if(archivoDeBloque==NULL){
				list_destroy_and_destroy_elements(tempBins,free);
				free(directorioDeTrabajo);
				free(ubicacionDelBloque);
				for(int j=i;arrayDeBloques[j]!=NULL;j++)
					free(arrayDeBloques[j]);
				fclose(archivoTemp);
				remove(archivoTempUbicacion);
				free(archivoTempUbicacion);
				return NULL;
			}
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



