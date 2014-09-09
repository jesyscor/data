/*
 * main.h
 *
 *  Created on: 04/05/2014
 *      Author:lemmau
 */

#ifndef MAIN_H_
#define MAIN_H_

	#define MAX_COUNT_OF_CONFIG_KEYS 14

	#include <commons/config.h>
	#include <commons/string.h>	// probar quoteado !!
	#include <commons/collections/dictionary.h>
	#include <commons/collections/list.h>
	#include <parser/parser.h>
	#include <parser/sintax.h>

	#include <stdlib.h>
	#include <sys/stat.h>
	#include <stdlib.h>
	#include <string.h>
	#include <pthread.h>
	#include <semaphore.h>

	#include "PLP.h"
	#include "PCP.h"
//	#include "IO.h"
	#include "CMD.h"
	#include "Estructuras_Auxiliares.h"

	// Variables de configuracion
	char *config_path;
	t_config *config_file;

	char** hio;
	char** id_hio;

	t_dictionary * dictionaryCompartidas;
	t_dictionary * dictionarySemaforos;

	int retardo;

	char** vars;
	char** semaforos;
	char** valorSemaforos;

	// Estructuras de configuracion

	typedef struct estructura_variable_compartida {
		int valor;
		pthread_mutex_t mutex;
	} var_comp_t;

	typedef struct estructura_semaforo {
		char szSemaforo[20];
		int iSemaforo;
		pthread_mutex_t mutex;
		t_list * progEncolados;
	} semaforo_t;

	t_list *sem_list, *io_list;

	// Variables para hilos
	pthread_t thread_PLP, thread_PCP, thread_CMD; // Threads para cada hilo del proceso Kernel (tmb estan los thread_IO[i])
	int ret_PLP, ret_PCP, ret_IO, ret_CMD;		// Valores de retorno al crear hilo

	// Tipos de los argumentos pasados a los hilos
	arg_PLP param_PLP;
	arg_PCP param_PCP;

	// Funciones principales
	void leer_config();		// Lee configuracion y asigna a variables.
	void cargar_arg_PLP();	// Carga estructura para mandar a hilo PLP
	void cargar_arg_PCP();	// Carga estructura para mandar a hilo PCP
	void cargar_arg_IO();	// Carga estructura para mandar a hilo IO

#endif /* MAIN_H_ */
