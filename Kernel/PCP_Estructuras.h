/*
 * PCP_Estructuras.h
 *
 *  Created on: 25/05/2014
 *      Author:lemmau
 */

#ifndef PCP_ESTRUCTURAS_H_
#define PCP_ESTRUCTURAS_H_

	#include <stdbool.h>
	#include <commons/string.h>

	#include "Enviar_Recibir_Paquete.h"
	#include "Estructuras_Auxiliares.h"

	typedef enum Operaciones_PCP	{
		HANDSHAKE_CPU_SUCCESS = 23,
		HANDSHAKE_CPU_FAIL,
		PCB_SEND = 25,
		IO,
		END_PROGRAMM_PCP,
		QUANTUM_FINISHED,
		PID_CPU,
		PID_CPU_SUCCESS,
		PID_CPU_FAIL,
		QUANTUM_SEND = 32,
		IMPRIMIR_ENTERO = 100,
		IMPRIMIR_TEXTO = 101,
		ENVIAR_IMPRIMIR_TEXTO = 102,
		ENVIAR_IMPRIMIR_ENTERO = 103,
	} operaciones_PCP;

	typedef struct CPU_list_value {	//Estructura para linkear PID con socket de CPU y tambien su disponibilidad.
		u_int32_t socket;
		u_int32_t pid;
		bool available;
	} CPU_list_nodo;

#endif /* PCP_ESTRUCTURAS_H_ */
