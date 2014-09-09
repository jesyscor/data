/*
* Estructuras_Auxiliares.h
*
*  Created on: 04/05/2014
*      Author:lemmau
*/

#ifndef ESTRUCTURAS_AUXILIARES_H_
#define ESTRUCTURAS_AUXILIARES_H_

	#include <stdint.h>
 	#include <stdio.h>
 	#include <stdlib.h>
	#include <commons/log.h>
	#include <semaphore.h>

	t_log* logger;
	char bufferLog[80];

	pthread_mutex_t mutex_cola_new;
	pthread_mutex_t mutex_cola_ready;
	pthread_mutex_t mutex_cola_exec;
	pthread_mutex_t mutex_cola_block;
	pthread_mutex_t mutex_cola_exit;

//	sem_t productorConsumidor;
	sem_t sem_gradoMultiProg;

	typedef struct __attribute__((__packed__)) estructura_PCB {
		u_int32_t segmentoDeStack;
		u_int32_t indiceDeCodigo;
		u_int32_t indiceDeEtiquetas;
		u_int32_t PC;
		u_int32_t cursorDeStack;
		u_int32_t tamanioCotextoActual;
		u_int32_t tamanioIndiceDeEtiquetas;
		u_int32_t peso;	// agrego esta para la planificacion SJN del PLP
		u_int32_t identificador;
		u_int32_t segmentoDeCodigo;
 	} t_PCB;

 	typedef struct Mensaje	{
 		char *out;
 	} mensaje;

	typedef struct PID	{
		u_int32_t pid;
	} pid_structura;

	typedef struct Quantum	{
		u_int32_t quantum;
	} t_quantum;

#endif /* ESTRUCTURAS_AUXILIARES_H_ */
