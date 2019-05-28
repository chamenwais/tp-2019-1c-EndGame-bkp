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
	int ocupadoPorElBloque=0;
	int bloqueActual=-1;
	bool esMiNodo(void* nodo) {
		return !strcmp(((tp_nodoDeLaMemTable) nodo)->nombreDeLaTabla,nombreDeLaTabla);
		}

	void dumpearTabla(void* nodo) {
		int sizeDeLaDataADumpear;
		int ocupadoHastaElMomento=metadataDelFS.blockSize-ocupadoHastaElMomento;
		if((bloqueActual==-1)||(sizeDeLaDataADumpear<ocupadoHastaElMomento)){
			//necesito un bloque nuevo para llenar
			bloqueActual=obtenerBloqueLibreDelBitMap();
			if(bloqueActual!=-1){
				log_error(LOGGERFS,"Alerta, no hay mas bloques libres!!!!!");
			}else{
				string_append(&bloques, ",");
				string_append(&bloques, string_itoa(bloqueActual));
				}
			}
		char* cadenaAInsertar = string_new();
		string_append(&cadenaAInsertar, string_itoa(((tp_nodoDeLaTabla)nodo)->timeStamp));
		string_append(&cadenaAInsertar, ";");
		string_append(&cadenaAInsertar, string_itoa(((tp_nodoDeLaTabla)nodo)->key));
		string_append(&cadenaAInsertar, ";");
		string_append(&cadenaAInsertar, ((tp_nodoDeLaTabla)nodo)->value);
		string_append(&cadenaAInsertar, "\n");

		return;
	}


	tp_nodoDeLaMemTable nodoDeLaMem = list_remove_by_condition(memTable,esMiNodo);
	log_info(LOGGERFS,"Voy a dumpear la tabla", nodoDeLaMem->nombreDeLaTabla);
	t_metadataDeLaTabla metadataDeLaTabla = obtenerMetadataDeLaTabla(nodoDeLaMem->nombreDeLaTabla);
	nombreDelArchivoTemp=buscarNombreDelTempParaDumpear(nodoDeLaMem->nombreDeLaTabla);
	list_iterate(nodoDeLaMem->listaDeDatosDeLaTabla,dumpearTabla);
	string_append(&bloques, "]");
	crearElTemp(nombreDelArchivoTemp, bloques, sizeDelTemporal);
	log_info(LOGGERFS,"Tabla %s dumpeada",nombreDeLaTabla);
	liberarMemoriaDelNodo(nombreDeLaTabla);
	return EXIT_SUCCESS;
}

int liberarMemoriaDelNodo(char* liberarMemoriaDelNodo){
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
