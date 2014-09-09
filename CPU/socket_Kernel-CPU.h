/*
 * socket_Kernel-CPU.h
 *
 *  Created on: 25/05/2014
 *      Author:lemmau
 */

#ifndef SOCKET_KERNEL_CPU_H_
#define SOCKET_KERNEL_CPU_H_

	#include <stdlib.h>
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <unistd.h>
	#include <string.h>

	#include "Enviar_Recibir_Paquete.h"
	#include "PaquetesYOperaciones.h"

	// Funciones PCP

	void *get_in_addr_Kernel(struct sockaddr *sa);
	int crear_cliente_PCP(const char *ip, const char* puerto);
	int conectar_a_PCP(const char *ip, const char *puerto);
	int handleHandshakePCP(char* datos);
	int handlePCBRecv(char* datos);
	int handleQuantumRecv(char* datos);
	stream_t *handlePrintNum(paq_numero* datos);
	stream_t *handlePrintText(mensaje* datos);
	stream_t* do_struct_int(paq_numero* datos);
	stream_t* serialize_int(paq_numero* datos);
	stream_t* do_struct_mensaje(mensaje* datos);

	t_PCB pcbActual; //PCB con el que esta trabajando

#endif /* SOCKET_KERNEL_CPU_H_ */
