/*
 * socket_CPU-UMV.h
 *
 *  Created on: 17/05/2014
 *      Author:lemmau
 */

#ifndef SOCKET_CPU_UMV_H_
#define SOCKET_CPU_UMV_H_

	#include <PaquetesYOperaciones.h>
	#include <sockets/Enviar_Recibir_Paquete.h>

	void *inicializar_SocketCPUUMV();
	void cerrar_SocketCPUUMV();
	void *cpu_Handler(void *args);

#endif /* SOCKET_CPU_UMV_H_ */
