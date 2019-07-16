/*
 * TestGeneral.h
 *
 *  Created on: 17 may. 2019
 *      Author: utnso
 */

#ifndef TEST_TESTGENERAL_H_
#define TEST_TESTGENERAL_H_

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"
#include "../src/Memoria.h"
#include "../src/Consola.h"

int correr_tests();

int setup_timestamp();
int clean_timestamp();
void test_timestamp();

int setup_gestion_seg_pag();
int clean_gestion_seg_pag();
void test_calcular_cantidad_marcos_MP();
void test_colocar_value_en_MP();
void test_modificar_key_en_MP();

int setup_comandos();
int clean_comandos();
void test_tomar_value_insert();
void test_tomar_value_insert_una_comilla();
void test_tomar_value_insert_sin_comillas();

int setup_lru();
int clean_lru();
void test_lru_completo();

int setup_todas_pag_modificadas();
int clean_todas_pag_modificadas();
void test_todas_paginas_modificadas();
void test_recopilar_paginas_modificadas();

#endif /* TEST_TESTGENERAL_H_ */
