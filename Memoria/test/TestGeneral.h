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

int setup_gestion_seg_pag();
int clean_gestion_seg_pag();
int correr_tests();
void test_calcular_cantidad_marcos_MP();

#endif /* TEST_TESTGENERAL_H_ */
