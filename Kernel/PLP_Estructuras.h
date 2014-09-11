/*
 * PLP_Estructuras.h
 *
 *  Created on: 19/05/2014
 *      Author:lemmau
 */

#ifndef PLP_ESTRUCTURAS_H_
#define PLP_ESTRUCTURAS_H_

#include <commons/string.h>

#include <sockets/Enviar_Recibir_Paquete.h>
#include "Estructuras_Auxiliares.h"

typedef struct __attribute__((__packed__)) PaqueteSolicitudMemoria {
	u_int32_t id_proceso;
	u_int32_t size;
	char* datos;
} paq_SolicitudMemoria;

typedef struct __attribute__((__packed__)) PaqueteRespuestaSolicitudMemoria {
	u_int32_t posicion;
} paq_res_SolicitudMemoria;

typedef enum Operaciones {  //TODO: ordenar y reorganizar numeros!!
	//PROGRAMA
	HANDSHAKE_SUCCESS = 1,
	HANDSHAKE_FAIL,

	PID,
	PID_FAIL,
	PID_SUCCESS,

	SCRIPT_LINE,
	SCRIPT_EOF,
	SCRIPT_SUCCESS,
	SCRIPT_FAIL,

	END_PROGRAMM,
	//UMV
	HANDSHAKE_UMV_SUCCESS = 12,
	HANDSHAKE_UMV_FAIL = 13,

	SOLICITAR_MEMORIA_UMV = 14,
	MEMORIA_UMV_SUCCESS = 15,
	MEMORIA_UMV_FAIL = 16,
} operaciones_t;

mensaje* undo_struct_mensaje(char* datos);
mensaje* deserialize_struct_mensaje(char *datos);

stream_t *do_struct_mensaje(mensaje* datos, int* size);
stream_t *serialize_struct_mensaje(mensaje *msg, int *tamanho);

#endif /* PLP_ESTRUCTURAS_H_ */
