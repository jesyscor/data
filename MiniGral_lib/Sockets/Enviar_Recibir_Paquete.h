/*
 * Enviar_Recibir_Paquete.h
 *
 *  Created on: 14/05/2014
 *      Author:lemmau
 */

#ifndef ENVIAR_RECIBIR_PAQUETE_H_
#define ENVIAR_RECIBIR_PAQUETE_H_

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/socket.h>
	#include <errno.h>

	typedef struct Cabecera {
		u_int32_t identificador;/* ID de operacion */
		u_int32_t longitud; 	/* Longitud del mensaje, en bytes */
	} Cabecera;

	typedef struct Stream {
		char* data;
	} stream_t;

	extern int RETARDO;

	//Funciones de proposito Gral

	int endianess();
	int sendall(int socket, void *buf, int *len);
	void recv_msg_from(int socket, int *idMensaje, char **mensaje);			//analizar_paquete()
	void send_msg_to(int socket, int idMensaje, char *mensaje, int tamanho);//preparar_paquete()


#endif /* ENVIAR_RECIBIR_PAQUETE_H_ */
