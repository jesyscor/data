/*
 * socket_UMV-CPU.h
 *
 *  Created on: 25/05/2014
 *      Author:lemmau
 */

#ifndef SOCKET_UMV_CPU_H_
#define SOCKET_UMV_CPU_H_


	#define MAXDATASIZE 256

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <unistd.h>
	#include <string.h>

	#include <parser/parser.h>

	#include "Enviar_Recibir_Paquete.h"
	#include "PaquetesYOperaciones.h"
	#include "Primitivas.h"

	// Funciones UMV

	void *get_in_addr_UMV(struct sockaddr *sa);
	int crear_cliente__UMV(const char *ip, const char* puerto);
	int recibir_msg_UMV();
	int handshake_UMV(char* mensaje);
	int conectar_a_UMV(const char *ip, const char *puerto);
	void solicitar_ProximaInstruccion();
	int handleInstruccion(char* datos);
	int analizar_paquete(u_int32_t socket, char *paquete, operaciones_t *id);
	stream_t *serialize_struct_sol_Escritura (u_int32_t base, u_int32_t offset, u_int32_t size, char* datos, u_int32_t *sizeTotal);
	stream_t *serialize_struct_sol_Lectura (u_int32_t base, u_int32_t offset, u_int32_t size);

	paq_res_LeerMemoria *deserialize_struct_res_SolicitudLectura(char* datos);
	paq_res_EscribirMemoria *deserialize_struct_res_SolicitudEscritura(char* datos);

	extern t_PCB pcbActual; //PCB con el que esta trabajando

#endif /* SOCKET_UMV_CPU_H_ */
