/*
 * PaquetesYOperaciones.h
 *
 *  Created on: 25/05/2014
 *      Author:lemmau
 */

#ifndef PAQUETESYOPERACIONES_H_
#define PAQUETESYOPERACIONES_H_

	#include <stdlib.h>
	#include <stdio.h>

	typedef struct Quantum {
		u_int32_t quantum;
	} t_quantum;

	typedef struct Mensaje {
		char *out;
	} mensaje;

	typedef struct Numero {
		u_int32_t numero;
	} paq_numero;

	typedef struct __attribute__((__packed__)) PaqueteEscribirMemoria {
		u_int32_t id_proceso;
		u_int32_t base;
		u_int32_t offset;
		u_int32_t size;
	} paq_sol_EscribirMemoria;

	typedef struct __attribute__((__packed__)) PaqueteLeerMemoria {
		u_int32_t id_proceso;
		u_int32_t base;
		u_int32_t offset;
		u_int32_t size;
	} paq_sol_LeerMemoria;

	typedef struct __attribute__((__packed__)) PaqueteRespuestaEscribirMemoria {
		u_int32_t posicion;
	} paq_res_EscribirMemoria;

	typedef struct __attribute__((__packed__)) PaqueteRespuestaLeerMemoria {
		u_int32_t size;
		char* datos;
	} paq_res_LeerMemoria;

	typedef struct estructura_PCB {
		u_int32_t identificador;
		u_int32_t segmentoDeCodigo;
		u_int32_t segmentoDeStack;
		u_int32_t indiceDeCodigo;
		u_int32_t indiceDeEtiquetas;
		u_int32_t PC;
		u_int32_t cursorDeStack;
		u_int32_t tamanioCotextoActual;
		u_int32_t tamanioIndiceDeEtiquetas;
		u_int32_t peso;
	} t_PCB;

	typedef struct __attribute__((__packed__)) TipoInstruccionSerializada {
		u_int32_t start;
		u_int32_t offset;
	} t_instruction;

	typedef enum Operaciones {
		//OPERACIONES UMV
		HANDSHAKE_UMV_SUCCESS = 12,

		SOLICITUD_ESCRITURA_ENVIADA = 16,
		SOLICITUD_ESCRITURA_RESPUESTA = 17,

		SOLICITUD_LECTURA_ENVIADA = 18,
		SOLICITUD_LECTURA_RESPUESTA = 19,

		END_PROGRAM,

		//OPERACIONES PCP
		HANDSHAKE_CPU_SUCCESS = 23,
		HANDSHAKE_CPU_FAIL,

		PCB_RECV = 25,
		IO,
		END_PROGRAMM_PCP,
		QUANTUM_FINISHED,

		PID_CPU,
		PID_CPU_SUCCESS,
		PID_CPU_FAIL,

		QUANTUM_RECV = 32,
		IMPRIMIR_ENTERO = 100,
		IMPRIMIR_TEXTO = 101,
	} operaciones_t;

#endif /* PAQUETESYOPERACIONES_H_ */
