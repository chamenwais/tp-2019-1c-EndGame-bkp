/*
 * funcionesAuxiliares.h
 *
 *  Created on: 24 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESAUXILIARES_H_
#define FUNCIONESAUXILIARES_H_

#ifndef  XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE //tmb
#endif

#include "lissandra.h"
#include "variablesGlobales.h"
#include "../COM/lqvg/com.h"
#include "../COM/lqvg/protocolo.h"
#include <stdbool.h>
#include <ftw.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>

int crearDirectorioParaLaTabla(char* nombreDeLaTabla);
int crearMetadataParaLaTabla(char* nombreDeLaTabla, char* tipoDeConsistencia,
		int numeroDeParticiones, int tiempoDeCompactacion);
int crearArchivosBinariosYAsignarBloques(char* nombreDeLaTabla,
		int numeroDeParticiones);
int eliminarDirectorioYArchivosDeLaTabla(char* nombreDeLaTabla);
int crearArchivoDeBloque(int bloqueLibre);
int eliminarDeLaMemtable(char* nombreDeLaTabla);
int eliminarDirectorio(char* nombreDeLaTabla);
int setearEstadoDeFinalizacionDelSistema(bool estadoDeFinalizacion);
bool obtenerEstadoDeFinalizacionDelSistema();
int eliminarArchivoDeMetada(char* nombreDeLaTabla);
int eliminarTemporales(char* nombreDeLaTabla);
int liberarBloquesYParticiones(char* nombreDeLaTabla);
t_metadataDeLaTabla obtenerMetadataDeLaTabla(char* nombreDeLaTabla);
bool verSiExisteListaConDatosADumpear(char* nombreDeLaTabla);
int aLocarMemoriaParaLaTabla(char* nombreDeLaTabla);
tp_nodoDeLaMemTable obtenerNodoDeLaMemtable(char* nodeName);
int hacerElInsertEnLaMemoriaTemporal(char* nombreDeLaTabla, uint16_t key, char* value, unsigned timeStamp);
tp_nodoDeLaTabla obtenerKeyConTimeStampMasGrande(t_list* keysObtenidas);
t_list* escanearPorLaKeyDeseada(uint16_t key, char* nombreDeLaTabla, int numeroDeParticionQueContieneLaKey);
t_list* escanearPorLaKeyDeseadaMemTable(uint16_t key, char* nombreDeLaTabla);
t_list* obtenerListaDeDatosDeArchivo(char* nombreDelArchivo, char* nombreDeLaTabla, uint16_t key);
t_list* recuperarKeysDelArchivoFinal(char* nombreDelArchivo, uint16_t key);
bool existeElArchivo(char* nombreDelArchivo);
t_list* escanearPorLaKeyDeseadaArchivosTemporales(uint16_t key, char* nombreDeLaTabla);
t_list* escanearPorLaKeyDeseadaParticionCorrespondiente(uint16_t key,
		int numeroDeParticionQueContieneLaKey, char* nombreDeLaTabla);
int vaciarListaDeKeys(t_list* keysObtenidas);
char* recortarHastaUltimaBarra(char* path);
t_list* obtenerTodosLosDescriptores();
char* recortarDespuesUltimaBarra(char*);//ej: home/utnso/file -> file
void liberarYDestruirTablaDeMetadata(t_list*);
void liberarBloque(char* numeroBloque);
void liberarBloquesTmpc(char* pathCompletoTmpc);
void liberarBloquesDelBitmap(t_list* bloques);
t_list* insertarCadenaEnLosBloques(char* cadenaGigante);//retorna una lista de int 1,2, ..., o NULL si no quedan bloques(si ya tomo algun bloque lo libera)
int cargarParticionATabla(char* nombreTabla,int numParticion,int size,t_list* bloques);
t_list* insertarCadenaEnNuevosBloques(char* cadenaGigante);
#endif /* FUNCIONESAUXILIARES_H_ */
