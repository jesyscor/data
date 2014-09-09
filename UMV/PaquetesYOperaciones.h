/*
 * PaquetesYOperaciones.h
 *
 *  Created on: 17/05/2014
 *      Author:lemmau
 */

#ifndef PAQUETESYOPERACIONES_H_
#define PAQUETESYOPERACIONES_H_

typedef struct PaqueteHandshake {
    char* data;
} paq_Handshake;

typedef struct __attribute__((__packed__)) PaqueteSolicitudMemoria {
	u_int32_t id_proceso;
	u_int32_t size;
	char* datos;
} paq_SolicitudMemoria;

typedef struct __attribute__((__packed__)) PaqueteRespuestaSolicitudMemoria {
    u_int32_t posicion;
} paq_res_SolicitudMemoria;

typedef struct __attribute__((__packed__)) PaqueteSolicitudInstruccion {
	u_int32_t id_proceso;
	u_int32_t baseCodeSegment;
	u_int32_t baseIndiceCodigo;
	u_int32_t numeroDeInstruccion;
} paq_sol_Instruccion;

typedef struct __attribute__((__packed__)) PaqueteEscribirMemoria {
	u_int32_t id_proceso;
	u_int32_t base;
	u_int32_t offset;
	u_int32_t size;
	char* buffer;
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

typedef enum Operaciones {
	// PLP o CPU
	HANDSHAKE_SUCCESS = 12,
	HANDSHAKE_FAIL = 13,

	SOLICITUD_MEMORIA_RECIBIDA = 14,
	SOLICITUD_MEMORIA_RESPUESTA = 15,

	SOLICITUD_ESCRITURA_RECIBIDA = 16,
	SOLICITUD_ESCRITURA_RESPUESTA = 17,

	SOLICITUD_LECTURA_RECIBIDA = 18,
	SOLICITUD_LECTURA_RESPUESTA = 19,

	END_PROGRAM
} operaciones_t;

#endif /* PAQUETESYOPERACIONES_H_ */
