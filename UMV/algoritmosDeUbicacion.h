/*
 * algoritmosDeUbicacion.h
 *
 *  Created on: 28/04/2014
 *      Author:lemmau
 */

#ifndef ALGORITMOSDEUBICACION_H_
#define ALGORITMOSDEUBICACION_H_

	#include <stdlib.h>
	#include <string.h>
	#include <stdio.h>
	#include <stdbool.h>
	#include <commons/collections/list.h>
	#include <estructurasDeSegmentos.h>

	typedef struct estructura_NodoDeSegmentoAuxiliar
	{
		int proceso;
		int base;
		int size;
		u_int32_t posicionMP;
	} NodoDeSegmentoAuxiliar;

	//Variables Externas
	extern t_list *listaAuxiliar;
	extern char *bloquePrincipal;

	//Funciones Principales

	char *solicitarBloque(int proceso, int base, int offset, int size);
	int guardarBloque(int proceso, int base, int offset, int size, char *bufferAEscribir);

	// de creacion de seg segun algoritmo elegido
	NodoDeSegmento crearSegmento_firstFit(NodoDeProceso datosDelProceso, NodoDeSegmento datosDelSegmento);
	NodoDeSegmento crearSegmento_worstFit(NodoDeProceso datosDelProceso, NodoDeSegmento datosDelSegmento);

	// de compactacion
	void compactar();

#endif
