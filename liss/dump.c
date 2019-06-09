/*
 * dump.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "dump.h"

int dump(char* nombreDeLaTabla){
	char* nombreDelArchivoTemp;
	char* bloques = string_new(); //va a tener el formato: [2,3,7,10]
	string_append(&bloques, "[");
	int sizeDelTemporal = 0;
	int punteroDelBloque=0;
	int bloqueActual=-1;
	bool hayBloquesLibres=true;
	char* cadenaFinal;

	bool esMiNodo(void* nodo) {
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		}

	void agregarALaCadenaFinal(void* nodo){
		char* cadenaAInsertar = string_new();
		string_append(&cadenaAInsertar, string_itoa(((tp_nodoDeLaTabla)nodo)->timeStamp));
		string_append(&cadenaAInsertar, ";");
		string_append(&cadenaAInsertar, string_itoa(((tp_nodoDeLaTabla)nodo)->key));
		string_append(&cadenaAInsertar, ";");
		string_append(&cadenaAInsertar, ((tp_nodoDeLaTabla)nodo)->value);
		string_append(&cadenaAInsertar, "\n");
		string_append(&cadenaFinal, cadenaAInsertar);
	}

	int insertarCadenaEnLosBloques(){
		bool termine=false;
		bool esElPrimero=true;
		while(termine==false){
			bloqueActual=obtenerBloqueLibreDelBitMap();
			ocuparBloqueDelBitmap(bloqueActual);
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
				//Actulizo la cadena de bloques
				if(esElPrimero){
					esElPrimero=false;
				}else{
					string_append(&bloques, ",");
					}
				string_append(&bloques, string_itoa(bloqueActual));
				}
			}
		return EXIT_SUCCESS;
		}

	log_info(LOGGERFS,"Duempeando la tabla %s",nombreDeLaTabla);
	log_info(LOGGERFS,"Block size del FS %d",metadataDelFS.blockSize);
	tp_nodoDeLaMemTable nodoDeLaMem = list_remove_by_condition(memTable,esMiNodo);
	log_info(LOGGERFS,"Voy a dumpear la tabla", nodoDeLaMem->nombreDeLaTabla);
	t_metadataDeLaTabla metadataDeLaTabla = obtenerMetadataDeLaTabla(nodoDeLaMem->nombreDeLaTabla);
	nombreDelArchivoTemp=buscarNombreDelTempParaDumpear(nodoDeLaMem->nombreDeLaTabla);
	cadenaFinal = string_new();
	//meto todo en un gran bodoque (en cadena cadenaFinal)
	list_iterate(nodoDeLaMem->listaDeDatosDeLaTabla,agregarALaCadenaFinal);
	sizeDelTemporal=string_length(cadenaFinal);
	log_info(LOGGERFS,"Cadena final a insertar: \n%s",cadenaFinal);
	insertarCadenaEnLosBloques();
	string_append(&bloques, "]");
	crearElTemp(nombreDelArchivoTemp, bloques, sizeDelTemporal);
	log_info(LOGGERFS,"Tabla %s dumpeada",nombreDeLaTabla);
	liberarMemoriaDelNodo(nombreDeLaTabla);
	free(bloques);
	if(hayBloquesLibres){
		return DUMP_CORRECTO;
	}else{
		log_error(LOGGERFS,"Se hizo el dump pero en em medio del proceso se acabaron los bloques libres, no se puede asegurar la consistencia de los datos");
		return NO_HAY_MAS_BLOQUES_EN_EL_FS;
	}
}

int liberarMemoriaDelNodo(char* liberarMemoriaDelNodo){
	return EXIT_SUCCESS;
}

int lanzarDumps(){
	char* ubicacionDeLasCarpetasDeBloque=string_new();
	string_append(&ubicacionDeLasCarpetasDeBloque, configuracionDelFS.puntoDeMontaje);
	string_append(&ubicacionDeLasCarpetasDeBloque, "/Tables");
	DIR *directorio;
	struct dirent *carpeta;
	directorio = opendir(ubicacionDeLasCarpetasDeBloque);
	if(directorio){
		while ((carpeta = readdir(directorio)) != NULL){
            if((!string_equals_ignore_case(carpeta->d_name,".."))&&
            		(!string_equals_ignore_case(carpeta->d_name,"."))){
            	//veo q no sea .. ni .
            	log_info(LOGGERFS,"Agregado dump para la tabla %s", carpeta->d_name);
            	char*nombreDeLaTabla=string_new();
            	string_append(&nombreDeLaTabla, carpeta->d_name);
            	lanzarHiloParaLaTabla(nombreDeLaTabla);
            }else{
            	log_info(LOGGERFS,"%s No es una carpeta de tablas, no lanzo el hilo", carpeta->d_name);
            	}
	        }
        closedir(directorio);
	    }
	free(ubicacionDeLasCarpetasDeBloque);
	return EXIT_SUCCESS;
}

void hiloDeDumpeo(tp_hiloDeDumpeo hiloDeDumpeo){
	return;
}


int lanzarHiloParaLaTabla(nombreDeLaTabla){
	log_info(LOGGERFS,"Voy a crear un hilo detachabe de dump para la tabla: %s", nombreDeLaTabla);
	tp_hiloDeDumpeo dumpTable;

	dumpTable->nombreDeLaTabla=string_duplicate(nombreDeLaTabla);
	pthread_attr_init(&(dumpTable->attr));
	pthread_attr_setdetachstate(&(dumpTable->attr), PTHREAD_CREATE_DETACHED);
	pthread_create(&(dumpTable->thread), &(dumpTable->attr), &hiloDeDumpeo(), dumpTable);
	pthread_attr_destroy(&(dumpTable->attr));

		int resultadoDeCrearHilo = pthread_create( &threadConsola, NULL,
				funcionHiloConsola, "Hilo consola");
		if(resultadoDeCrearHilo){
			log_error(LOGGERFS,"Error al crear el hilo de la consola, return code: %d",
					resultadoDeCrearHilo);
			exit(EXIT_FAILURE);
		}else{
			log_info(LOGGERFS,"La consola se creo exitosamente");
			return EXIT_SUCCESS;
			}

	return EXIT_SUCCESS;
}

int insertarDatosEnElBloque(char* cadenaAInsertar,int bloqueActual){
	//le pasas un numero de bloque y una cadena con los datos a insertar y los manda
	//a continuacion de lo q haya
	char* nombreDelArchivoDeBloque=string_new();
	string_append(&nombreDelArchivoDeBloque, configuracionDelFS.puntoDeMontaje);
	string_append(&nombreDelArchivoDeBloque, "/Blocks/");
	string_append(&nombreDelArchivoDeBloque, string_itoa(bloqueActual));
	string_append(&nombreDelArchivoDeBloque, ".bin");
	FILE* archivo=fopen(nombreDelArchivoDeBloque,"a");
	log_info(LOGGERFS,"Guardando %s en el archivo %s", cadenaAInsertar, nombreDelArchivoDeBloque);
	fprintf(archivo,"%s",cadenaAInsertar);
	fclose(archivo);
	return EXIT_SUCCESS;
}

int crearElTemp(char* nombreDelArchivo,char* bloques,int size){
	 //bloques tiene q venir formateado de la forma: [2,3,7,10]
	 log_info(LOGGERFS,"Voy a crear el archivo %s", nombreDelArchivo);
	 FILE * archivoTemp = fopen(nombreDelArchivo,"w");
	 fclose(archivoTemp);
	 t_config* configuracion = config_create(nombreDelArchivo);
	 config_set_value(configuracion, "SIZE", string_itoa(size));
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
		string_append(&pathDelTemp, string_itoa(i));
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
	return pathDelTemp;
}
