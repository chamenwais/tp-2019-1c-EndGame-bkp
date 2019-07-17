/*
 * funcionesKernel.h
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESKERNEL_H_
#define FUNCIONESKERNEL_H_

#include <stdlib.h>
#include "variablesGlobales.h"
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )
#define BUF_LEN     ( 1024 * EVENT_SIZE )


void inicializarLogKernel();
void iniciar_config(int, char **);
char* reconstruir_path_archivo(char* directorio, char* nombre_archivo);
void separar_path_pasado_por_parametro(char ** nombre_archivo, char ** directorio, char ** parametros);
void validar_apertura_archivo_configuracion();
void iniciar_escucha_cambios_conf(char * directorio, char * nombre_archivo);
void *escuchar_cambios_conf(void * estructura_path);
int inicializarVariablesGlobales();
int levantarConfiguracionInicialDelKernel();
int conectarse_con_primera_memoria(char* ip, char * puerto);
int conectarse_con_memoria(char* ip, char * puerto);
int inicializarListas();
t_operacion parsear(char * linea);
void enviar_handshake(int socket);
void* funcionHiloRequest(void* pcb);
int lanzarPlanificador();
int inicializarSemaforos();
void realizar_operacion(t_operacion resultado_del_parseado, tp_lql_pcb pcb, int socket_memoria);
void operacion_select(char* nombre_tabla, uint16_t key, tp_lql_pcb pcb, int socket_memoria);
void operacion_insert(char* nombre_tabla, int key, char* value, tp_lql_pcb pcb, int socket_memoria);
void operacion_create(char* nombre_tabla, char* tipo_consistencia, int num_particiones, int compaction_time, tp_lql_pcb pcb, int socket_memoria);
void operacion_describe(char* nombre_tabla, tp_lql_pcb pcb, int socket_memoria);
void operacion_drop(char* nombre_tabla, tp_lql_pcb pcb, int socket_memoria);
void operacion_journal(int socket);
tp_memo_del_pool_kernel decidir_memoria_a_utilizar(t_operacion operacion);
int lanzarPCP();
void* funcionHiloPCP();
void* funcionHiloPLP();
int lanzarHiloRequest(tp_lql_pcb pcb);
bool existeTabla(char* tabla);
char* obtenerTabla(t_operacion resultado_del_parseado);
void remover_pcb_de_lista(t_list* lista, tp_lql_pcb pcb);
bool pcbEstaEnLista(t_list* lista, tp_lql_pcb pcb);
tp_memo_del_pool_kernel buscar_memorias_segun_numero(t_list* lista, int numero);
void describeAll(int socket_memoria);
int conectar_con_memoria(char* ip, char * puerto);
tp_entrada_tabla_creada buscarTablaEnMetadata(char* tabla);
void iniciar_proceso_describe_all();
void* hacer_describe();
int obtener_pos_tabla(char* tabla);
void mostrar_lista_tablas();
int calcularHash(int key);
int actualizarQuantum(int nuevoQuantum);
int actualizarRefresh(int refresh);
int actualizarRetardo(int retardo);
int actualizarGossip(int gossip);
void* pedir_gossip();
void iniciar_pedido_gossip();
void conectarse_a_memorias_gossip(t_list* lista_gossip);
bool tablaFueBorrada(char* tabla, t_list* lista);
int abrir_socket_memoria(char*, char*);
double obtenerTimestamp();



#endif /* FUNCIONESKERNEL_H_ */
