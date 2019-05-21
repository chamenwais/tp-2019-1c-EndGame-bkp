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

#endif /* TEST_TESTGENERAL_H_ */
