/*
 * estructurasDeSegmentos.h
 *
 *  Created on: 18/04/2014
 *      Author:lemmau
 */

#ifndef ESTRUCTURASDESEGMENTOS_H_
#define ESTRUCTURASDESEGMENTOS_H_

	// CONSTANTES

	#define CANTIDAD_ARGUMENTOS_OPERACION_CREAR 4
	#define CANTIDAD_ARGUMENTOS_OPERACION_DESTRUIR 1
	#define CANTIDAD_ARGUMENTOS_OPERACION_LEER 4
	#define CANTIDAD_ARGUMENTOS_OPERACION_ESCRIBIR 5

	#include <commons/collections/list.h>
	#include <pthread.h>

	// ESTRUCTURAS

	typedef struct estructura_NodoDeProceso {
		int id_proceso;
		t_list *tablaDeSegmentos;
	} NodoDeProceso;

	typedef struct estructura_NodoDeSegmento {
		int base;
		int size;
		u_int32_t posicionMP;
	} NodoDeSegmento;

	//Estructura para las Operaciones (LEER, ESCRIBIR, CREAR, DESTRUIR)
	typedef struct estructura_Operacion {
		char tipo;
		int proceso;
		int base;
		int offset;
		int size;
		char* buff;
	} Operacion;

	//Variables Globales

	char *bloquePrincipal;
	FILE *output_dump;

	pthread_mutex_t mutex;

	// Funciones Principales

	void crear_ListaDeProcesos();
	void crear_ListaAuxiliar();

	void agregarProceso(int id_proceso);
	void destruirProceso(int id_proceso);
	void listarProcesos();
	void mostrarContenido();

	void agregarSegmento(int id_proceso, int base, int size, u_int32_t posicionMP);
	//void listarSegmentos(FILE *output_dump, int id_proceso);

	NodoDeSegmento crearSegmento(NodoDeProceso datosDelProceso, NodoDeSegmento datosDelSegmento);

	NodoDeProceso nuevo_NodoDeProceso(int id_proceso);
	NodoDeSegmento nuevo_NodoDeSegmento(int base, int size, u_int32_t posicionMP);
	NodoDeSegmento *obtenerSegmento(int id_proceso, int numeroDeSegmento);
	NodoDeSegmento *obtenerSegmentoPorBase(int id_proceso, int base);

	NodoDeProceso *obtenerProceso(int id_proceso);
	NodoDeProceso *obtenerProcesoPorID(int idAEncontrar);

#endif /* ESTRUCTURASDESEGMENTOS_H_ */
