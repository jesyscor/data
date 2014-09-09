/*
 * PLP_Operaciones_UMV.h
 *
 *  Created on: 19/05/2014
 *      Author:lemmau
 */

#ifndef PLP_OPERACIONES_UMV_H_
#define PLP_OPERACIONES_UMV_H_

	#include <stdlib.h>
	#include <stdio.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include "Enviar_Recibir_Paquete.h"
	#include "PLP_Estructuras.h"
	#include <parser/metadata_program.h>


	typedef struct arg_UMV {	// Estructura para pasar argumentos al hilo UMV
		//Falta definir
	}arg_UMV;


	t_PCB info_mem_UMV;
	char* bufferUMV = NULL;

	int crear_cliente_UMV(const char *ip, const char* puerto);
	void conectar_a_UMV(const char *ip, const char *puerto);
	stream_t *serialize_struct_solicitud_memoria(paq_SolicitudMemoria* datos, int *size);
	stream_t* do_struct_solicitud_memoria(paq_SolicitudMemoria* datos, int *size);
	int handleHandshakeUMV(char *datos);
	stream_t *handleSolicitudMemoria(paq_SolicitudMemoria* datos, int *size);
	int handleMemoriaSuccess(char* datos);
	int handleMemoriaFail(char* datos);
	int solicitarMemoria(t_medatada_program *metadata, u_int32_t pid, u_int32_t size, char* datos);

	//Funciones y variables en PLP.h
	extern int sockfd_cte;
	extern int analizar_paquete(u_int32_t socket, char *paquete, operaciones_t *id);
	extern int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura);

	//Funciones y variables en PLP_Operaciones_Programa.h
	extern char* script;

	u_int32_t calcular_peso(u_int32_t cant_de_etiq, u_int32_t cant_de_func, u_int32_t cant_tot_de_lineas_de_cod);

#endif /* PLP_OPERACIONES_UMV_H_ */
