/*
 * funcionesComunes.c
 *
 *  Created on: 18 abr. 2019
 *      Author: utnso
 */

#include "funcionesComunes.h"

bool existeLaTabla(char* nombreDeLaTabla){
	// Verificar que la tabla no exista en el file system.
	// Por convención, una tabla existe si ya hay otra con el mismo nombre.
	//@@?? Para dichos nombres de las tablas siempre tomaremos sus valores en UPPERCASE (mayúsculas).
	//bool resultado;
	//char* directorioDeLaTabla=string_new();
	//string_append(&directorioDeLaTabla, configuracionDelFS.puntoDeMontaje);
	//string_append(&directorioDeLaTabla, "/Tables/");
	//string_append(&directorioDeLaTabla, nombreDeLaTabla);
	//struct stat st = {0};

	log_info(LOGGERFS,"Hay una solicitud de existencia de la tabla %s", nombreDeLaTabla);

	if(!esTablaNuevaFS(nombreDeLaTabla)){
		log_info(LOGGERFS,"Tabla %s existe", nombreDeLaTabla);
		return true;
	}
	else{
		log_info(LOGGERFS,"Tabla %s no existe", nombreDeLaTabla);
		return false;
	}

	/*if(stat(directorioDeLaTabla, &st) == -1){
		log_info(LOGGERFS,"La tabla %s no existe", directorioDeLaTabla);
		resultado=false;
	}else{
		log_info(LOGGERFS,"La tabla %s ya existe", directorioDeLaTabla);
		resultado=true;
		}
	*/

	//free(directorioDeLaTabla);
}

int create(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion){

	log_info(LOGGERFS,"Hay una solicitud de create de %s", nombreDeLaTabla);

	aplicarRetardo();

	//if(existeLaTabla(nombreDeLaTabla)==false){

	if(agregarAListaDeTablasFS(nombreDeLaTabla)){

		pthread_rwlock_t* mutexTabla = bloquearExclusiveTablaFS(nombreDeLaTabla);
		if(mutexTabla!=NULL)
			log_info(LOGGERFS,"Nueva tabla %s bloqueada(escritura), procedo a crear sus archivos", nombreDeLaTabla);
		else{
			log_error(LOGGERFS,"Nueva tabla %s no bloqueada(escritura), no se pudieron crear sus archivos!", nombreDeLaTabla);
			return EXIT_FAILURE;
		}

		crearDirectorioParaLaTabla(nombreDeLaTabla);
		crearMetadataParaLaTabla(nombreDeLaTabla,tipoDeConsistencia,
				numeroDeParticiones,tiempoDeCompactacion);
		if(crearArchivosBinariosYAsignarBloques(nombreDeLaTabla,numeroDeParticiones)==EXIT_SUCCESS){
			log_info(LOGGERFS,"La tabla %s se creo correctamente", nombreDeLaTabla);
		}else{
			log_error(LOGGERFS,"No se pudo crear la tabla %s", nombreDeLaTabla);
			//@@@@@@eliminar el directorio y la metadata!!!
			if(mutexTabla!=NULL){
				desbloquearExclusiveTablaFS(mutexTabla);
				log_info(LOGGERFS,"Tabla %s desbloqueada(escritura)", nombreDeLaTabla);
				}
			eliminarDeListaDeTablasFS(nombreDeLaTabla);
			return EXIT_FAILURE;
			}

		if(mutexTabla!=NULL){
			desbloquearExclusiveTablaFS(mutexTabla);
			log_info(LOGGERFS,"Tabla %s desbloqueada(escritura)", nombreDeLaTabla);
			}

		return TABLA_CREADA;
	}else{
		log_error(LOGGERFS,"Se esta intentando crear una tabla con un nombre que ya existia: %s", nombreDeLaTabla);
		printf("Se esta intentando crear una tabla con un nombre que ya existia: %s\n", nombreDeLaTabla);
		return TABLA_YA_EXISTIA;
		}
}


int drop(char* nombreDeLaTabla){
	// Pasos para hacerlo:
	// 1) Verificar que la tabla exista en el file system.
	// 2) Eliminar directorio y todos los archivos de dicha tabla.

	log_info(LOGGERFS,"Hay una solicitud de drop de %s", nombreDeLaTabla);

	aplicarRetardo();

	if(eliminarDeListaDeTablasFS(nombreDeLaTabla)){
		log_info(LOGGERFS,"Voy a borrar la tabla %s", nombreDeLaTabla);
		eliminarDirectorioYArchivosDeLaTabla(nombreDeLaTabla);
		log_info(LOGGERFS,"Se borro la tabla %s", nombreDeLaTabla);
		return TABLA_BORRADA;
	}
	else{
		log_error(LOGGERFS,"Se esta intentando borrar una tabla que no existe: %s", nombreDeLaTabla);
		return TABLA_NO_EXISTIA;
	}
}

t_metadataDeLaTabla describe(char* nombreDeLaTabla){
	/* La operación Describe permite obtener la Metadata de una tabla en particular.
	 *	1) Verificar que la tabla exista en el file system.
	 *	2) Leer el archivo Metadata de dicha tabla.
	 *	3) Retornar el contenido del archivo.
	 */

	log_info(LOGGERFS,"Hay una solicitud de describe de %s", nombreDeLaTabla);

	aplicarRetardo();

	pthread_rwlock_t* mutexTabla = bloquearSharedTablaFS(nombreDeLaTabla);
	if(mutexTabla!=NULL)
		log_info(LOGGERFS,"Tabla %s bloqueada(lectura)", nombreDeLaTabla);
	else
		log_error(LOGGERFS,"Tabla %s no existe", nombreDeLaTabla);

	t_metadataDeLaTabla metadata=obtenerMetadataDeLaTabla(nombreDeLaTabla);
	//obtener la metadata no deberia correr xq ya se q la tabla no existe@@!!
	if(mutexTabla!=NULL){
		desbloquearSharedTablaFS(mutexTabla);
		log_info(LOGGERFS,"Tabla %s desbloqueada(lectura)", nombreDeLaTabla);
		}

	return metadata;
}

t_describeAll_rta describeAll(){
	t_describeAll_rta descriptores;
	aplicarRetardo();
	descriptores.lista = obtenerTodosLosDescriptores();
	return descriptores;
}

int insert(char* nombreDeLaTabla, uint16_t key, char* value, double timeStamp){
	/* Ejemplo: INSERT TABLA1 3 “Mi nombre es Lissandra” 1548421507
	 * Pasos:
	 * 1) Verificar que la tabla exista en el file system. En caso que no exista,
	 * informa el error y continúa su ejecución.
	 * 2) Obtener la metadata asociada a dicha tabla.
	 * 3) Verificar si existe en memoria una lista de datos a dumpear.
	 * De no existir, alocar dicha memoria.
	 * 4) El parámetro Timestamp es opcional. En caso que un request no lo
	 * provea (por ejemplo insertando un valor desde la consola), se usará
	 * el valor actual del Epoch UNIX.
	 * 5)Insertar en la memoria temporal del punto anterior una nueva entrada que
	 * contenga los datos enviados en la request.
	 */
	log_info(LOGGERFS,"Hay una solicitud insert de la tabla %s", nombreDeLaTabla);

	aplicarRetardo();

	pthread_rwlock_t* mutexTabla = bloquearSharedTablaFS(nombreDeLaTabla);
	printf("hizo el bloqueo para insert\n\n\n\n");
	if(mutexTabla!=NULL){
		log_info(LOGGERFS,"Tabla %s bloqueada(lectura)", nombreDeLaTabla);
		if(verSiExisteListaConDatosADumpear(nombreDeLaTabla)==false){
			aLocarMemoriaParaLaTabla(nombreDeLaTabla);
			//lanzarHiloParaLaTablaDeDumpeo(nombreDeLaTabla);
		}
		int resultadoDelInsert = hacerElInsertEnLaMemoriaTemporal(nombreDeLaTabla, key, value, timeStamp);

		desbloquearSharedTablaFS(mutexTabla);
		log_info(LOGGERFS,"Tabla %s desbloqueada(lectura)", nombreDeLaTabla);

		//pthread_mutex_unlock(&mutexDeDump);
		return resultadoDelInsert;
	}
	else{
		log_error(LOGGERFS,"Se esta intentando hace un insert de una tabla que no existe %s", nombreDeLaTabla);
		printf("Se esta intentando insertar una tabla que no existe %s\n", nombreDeLaTabla);
		return TABLA_NO_EXISTIA;
	}
}


double getCurrentTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

double obtenerTimestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec)*1000+((unsigned long long)tv.tv_usec)/1000);
	double a = result;
	log_info(LOGGERFS,"Timestamo obtenido: %lf", a);
	return a;
}

int insertSinTime(char* nombreDeLaTabla, uint16_t key, char* value){
	double timeStamp = obtenerTimestamp();
	//printf("\n\nTimeStamp obtenido: %lf\n\n\n", obtenerTimestamp());
	insert(nombreDeLaTabla, key, value, timeStamp);
	return EXIT_SUCCESS;
}

tp_nodoDeLaTabla selectf(char* nombreDeLaTabla, uint16_t key){
	/*
	 * Ej:
	 * SELECT [NOMBRE_TABLA] [KEY]
	 * SELECT TABLA1 3
	 * Esta operación incluye los siguientes pasos:
	 *	1)Verificar que la tabla exista en el file system.
	 *  2)Obtener la metadata asociada a dicha tabla.
	 *	3)Calcular cual es la partición que contiene dicho KEY.
	 *	4)Escanear la partición objetivo, todos los archivos temporales y
	 *		la memoria temporal de dicha tabla (si existe) buscando la key deseada.
	 *	5)Encontradas las entradas para dicha Key, se retorna el valor con el Timestamp
	 *		más grande.
	 */
	log_info(LOGGERFS,"Hay una solicitud de select de %s", nombreDeLaTabla);

	aplicarRetardo();

	tp_nodoDeLaTabla resultado = NULL;
	if(existeLaTabla(nombreDeLaTabla)==false){
		log_error(LOGGERFS,"Se esta intentando hace un select de una tabla que no existe %s", nombreDeLaTabla);
		printf("Se esta intentando seleccionar de una tabla que no existe %s\n", nombreDeLaTabla);
		resultado=malloc(sizeof(t_nodoDeLaTabla));
		resultado->resultado=TABLA_NO_EXISTIA;
	}else{
		t_metadataDeLaTabla metadataDeLaTabla=obtenerMetadataDeLaTabla(nombreDeLaTabla);
		int numeroDeParticionQueContieneLaKey = key%(metadataDeLaTabla.particiones);
		log_info(LOGGERFS,"Numero de particion que contiene a la key es %d, ya que las particiones son %d, y la key vale %d",
				numeroDeParticionQueContieneLaKey, metadataDeLaTabla.particiones, key);
		t_list* keysObtenidas = escanearPorLaKeyDeseada(key, nombreDeLaTabla, numeroDeParticionQueContieneLaKey);
		resultado = obtenerKeyConTimeStampMasGrande(keysObtenidas);
		vaciarListaDeKeys(keysObtenidas);
		free(metadataDeLaTabla.consistencia);
	}
	return resultado;
}

