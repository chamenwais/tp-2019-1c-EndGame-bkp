/*
 * GestionSegPaginada.c
 *
 *  Created on: 15 may. 2019
 *      Author: utnso
 */

#include "GestionSegPaginada.h"

int obtener_tamanio_marco() {
	int tamanio_marco = sizeof(long) + sizeof(uint16_t) + sizeof(TAMANIO_VALUE)
			+ 1;
	return tamanio_marco;
}

int obtener_cantidad_marcos_en_MP(int tamanio_marco) {
	int cantidad_marcos = (int) TAMANIO_MEMORIA / tamanio_marco;
	return cantidad_marcos;
}
