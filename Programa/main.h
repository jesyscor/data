/*
 * main.h
 *
 *  Created on: 23/04/2014
 *      Author:lemmau
 */

#ifndef MAIN_H_
#define MAIN_H_

	#define MAXLINESIZE 1024
	#define MAX_COUNT_OF_CONFIG_KEYS 2

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <string.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>

	#include <commons/txt.h>
	#include <commons/string.h>
	#include <commons/config.h>
	#include <commons/log.h>
	#include <commons/process.h>

	#include "Enviar_Recibir_Paquete.h"

	// Variables de scripts a interpretar
	FILE *script;
	char *script_name;
	bool esAnsisop;

	// Variables de configuracion
	t_config *config_file;
	t_log *logger;

	char *ip_kernel;
	char *puerto_kernel;

	// Estructuras

	typedef struct Mensaje { // Operacion 1,2,7,8,9,10
		char *out;
	} mensaje;

	typedef struct Numero {
		int numero;
	} paq_numero;

	typedef struct PID {
		u_int32_t pid;
	} pid_structura;

	typedef enum Operaciones {
		HANDSHAKE_SUCCESS = 1,
		HANDSHAKE_FAIL,
		PID,
		PID_FAIL,
		PID_SUCCESS,
		SCRIPT_LINE,
		SCRIPT_EOF,
		SCRIPT_SUCCESS,
		SCRIPT_FAIL,
		ENVIAR_IMPRIMIR_TEXTO = 102,
		ENVIAR_IMPRIMIR_ENTERO = 103,
		END_PROGRAMM
	} operaciones_t;

	char *buffer;
	char bufferLog[80];
	char* scriptBody;
	int sizeOfFile;
	int bytesReadScript;
	operaciones_t idOperacion;
	mensaje msg;
	pid_structura pid;

	// Variables para Socket Cliente
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	// FUNCIONES PRINCIPALES

		//Funciones para Sockets
	void *get_in_addr(struct sockaddr *); 					// Devuelve estructura sockaddr.
	int crear_socket_cliente (const char *, const char *);	// Crea un socket cliente para conectar al PLP.
	void cerrar_socket_cliente();							// Cierra el socket cliente.

		//Funciones para Handshake
	int handleHandshake(char *datos);
	int handlePIDResult(char* datos);
	int analizar_paquete(char *paquete, operaciones_t *id);
	void conectar_a_Kernel(char *, char *);

	    //Funciones Grales
	void leer_config(); // Lee configuracion y asigna a variables.
	paq_numero *deserialize_struct_int(char *datos);

#endif /* MAIN_H_ */
