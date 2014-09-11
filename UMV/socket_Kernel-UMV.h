/*
 * socket_Kernel-UMV.h
 *
 *  Created on: 03/05/2014
 *      Author:lemmau
 */

#ifndef SOCKET_KERNEL_UMV_H_
#define SOCKET_KERNEL_UMV_H_

	#define MAXDATASIZE 256

	#include <stdio.h>
	#include <stdlib.h>
	#include <errno.h>
	#include <unistd.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <signal.h>

	#include <algoritmosDeUbicacion.h>
	#include <sockets/Enviar_Recibir_Paquete.h>
	#include <PaquetesYOperaciones.h>

	// Variables para sockets
	extern char *ip_UMV;
	extern char *puerto_Kernel;

	void *inicializar_SocketKernelUMV();
	void cerrar_SocketKernelUMV();

#endif /* SOCKET_KERNEL_UMV_H_ */
