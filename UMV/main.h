/*
 * main.h
 *
 *  Created on: 13/06/2014
 *      Author:lemmau
 */

#ifndef MAIN_H_
#define MAIN_H_

	#define MAX_COUNT_OF_CONFIG_KEYS 4

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <pthread.h>

	#include <commons/string.h>
	#include <commons/config.h>
	#include <commons/log.h>
	#include <commons/collections/list.h>

	#include <estructurasDeSegmentos.h>
	#include <socket_Kernel-UMV.h>
	#include <socket_CPU-UMV.h>

//	extern char *bloquePrincipal;

	int sizeBloquePrincipal;

	pthread_t socket_KernelUMV;
	pthread_t socket_CPUUMV;

	// Variables de configuracion
	t_config *config_file;
	char *ip_UMV;
	char *puerto_Kernel;
	char *puerto_CPU;

	t_log* logger;
	char bufferLog[80];
	// Main functions
	int gestionarInput();


#endif /* MAIN_H_ */
