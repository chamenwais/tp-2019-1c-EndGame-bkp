/*
 * TestGeneral.c
 *
 *  Created on: 17 may. 2019
 *      Author: utnso
 */

#include "TestGeneral.h"

int correr_tests(){
	CU_initialize_registry();

	CU_pSuite tests_gestion_seg_pag =
			CU_add_suite("Suite para probar gestión de la seg paginada", setup_gestion_seg_pag, clean_gestion_seg_pag);
	CU_add_test(tests_gestion_seg_pag, "test_calcular_cantidad_marcos_MP", test_calcular_cantidad_marcos_MP);
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

int setup_gestion_seg_pag(){
	TAMANIO_MEMORIA=256;
	TAMANIO_VALUE=3;
	g_logger= log_create("/home/utnso/TestsMemoria.log", "TestsMemoria", true, LOG_LEVEL_DEBUG);
	return 0;
}

int clean_gestion_seg_pag(){
	log_destroy(g_logger);
	return 0;
}

void test_calcular_cantidad_marcos_MP(){
	setup_gestion_seg_pag();
	int tamanio_marco = obtener_tamanio_marco();
	CU_ASSERT_EQUAL(obtener_cantidad_marcos_en_MP(tamanio_marco), 23);
}
