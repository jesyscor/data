/*
 * main.h
 *
 *  Created on: 13/05/2014
 *      Author:lemmau
 */

#ifndef MAIN_H_
#define MAIN_H_

	#define MAXDATASIZE 256
	#define MAX_COUNT_OF_CONFIG_KEYS 4

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <signal.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <errno.h>
	#include <unistd.h>
	#include <string.h>
	#include <pthread.h>

	#include <parser/parser.h>
	#include <parser/metadata_program.h>
	#include <commons/log.h>
	#include <commons/config.h>

	#include "Primitivas.h"
	#include "Enviar_Recibir_Paquete.h"
	//#include "socket_Kernel-CPU.h"
	//#include "socket_UMV-CPU.h"

	// Definimos variables de configuracion
	t_config *config_file;
	char* ip_kernel;
	char* puerto_kernel;
	char* ip_umv;
	char* puerto_umv;
	int quantum;
	int retardo;

	t_log* logger;
	char bufferLog[80];
	t_dictionary *dictionaryVariables;
	operaciones_t Enum;

	// Definimos variables para Socket Cliente
	int sockfd, numbytes;
	char *buf;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	typedef struct arg_CPU { // Estructura para pasar argumentos al hilo CPU
		char* ipKernel;
		char* puertoKernel;
		char* ipUMV;
		char* puertoUMV;
	} arg_CPU;

	int ret_PCP;						// Valores de retorno al crear hilo
	pthread_t thread_PCP, thread_UMV;	// pthreads para cada hilo del proceso CPU
	arg_CPU param_CPU;
	char *bufferCPU = NULL;

	//Funciones principales
	void leerConfig();
	void cargar_arg_CPU(arg_CPU* arg);
	void* main_CPU(void* parametros);
	int analizar_paquete(u_int32_t socket, char *paquete, operaciones_t *id);
	int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura);

	//Funciones locales del socket_kernetl-CPU
	extern int handleHandshakePCP(char* datos);
	extern int handleQuantumRcv(char* datos);
	extern int handlePCBRcv(char* datos);
	extern int sockfd_cte_Kernel;
	extern int sockfd_cte_UMV;

#endif /* MAIN_H_ */
