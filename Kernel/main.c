/*
 * main.c
 *
 *  Created on: 24/04/2014
 *      Author:lemmau
 *
 * CICLO DEL PROCESO KERNEL
 *
 * 1- Leer archivo de configuracion y asignar valores
 * 2- Asignar esos valores (en una estructura) como argumentos de los hilos PLP,PCP y IO
 * 3- Levantar Hilos
 *	3.1- Hilo PLP
 *	3.2- Hilo PCP
 *	3.3- Hilo CMD
 *
 *TODO Tratar HIO
 */

#include "main.h"

void leerConfig(char *config_path)
{
	if(!config_path) {
		// en caso de NO haberlo ingresado como parametro desde la consola, lo BUSCARA en el MISMO DIRECTORIO donde se ejecute el proceso
		config_path = "config.cfg";
	}

	config_file = config_create(config_path);
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!\n");
		fprintf(stderr,"Usage:\n> ./Kernel [PATH/config.cfg]\n");
	    log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!");
		exit(1);
	}

	// VARIABLES COMPARTIDAS (DICCIONARIO)
	// Sus valores se inicializan en 0(cero) SEGUN enunciado del TP !!
	if(config_has_property(config_file, "COMPARTIDAS"))
	{
		int i;
		dictionaryCompartidas = dictionary_create();
		vars = config_get_array_value(config_file, "COMPARTIDAS");

		for(i = 0; vars[i] != '\0'; i++)
		 {
			var_comp_t* var_comp = malloc(sizeof(var_comp_t));
			var_comp->valor = 0;								// SE INICIALIZAN EN 0 (CERO) SEGUN ENUNCIADO DEL TP !!
			pthread_mutex_init(&(var_comp->mutex), NULL);
			dictionary_put(dictionaryCompartidas, vars[i], var_comp);

			sprintf(bufferLog, "COMPARTIDA [%s] = [%d]", vars[i], 0);
			log_debug(logger, bufferLog);
		}
	} else {
		fprintf(stderr, "Falta key 'COMPARTIDAS' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'COMPARTIDAS' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//SEMAFOROS y VALOR_SEMAFOROS
	if(config_has_property(config_file, "SEMAFOROS") && config_has_property(config_file, "VALOR_SEMAFORO"))
	{
		int i;
		dictionarySemaforos = dictionary_create();
		sem_list = list_create();

		semaforos = config_get_array_value(config_file, "SEMAFOROS");
		valorSemaforos = config_get_array_value(config_file, "VALOR_SEMAFORO");

		for (i = 0; semaforos[i] != '\0'; ++i)
		{
			semaforo_t * unSemaforo = (semaforo_t *) malloc(sizeof(semaforo_t));
			strcpy(unSemaforo->szSemaforo, semaforos[i]);
			unSemaforo->iSemaforo = atoi(valorSemaforos[i]);
			pthread_mutex_init(&unSemaforo->mutex, NULL );
			unSemaforo->progEncolados = list_create();

			list_add(sem_list, unSemaforo);

			int* valor = malloc(sizeof(int));
			*valor = atoi(valorSemaforos[i]);
			dictionary_put(dictionarySemaforos, semaforos[i], valor);

			sprintf(bufferLog, "SEMAFORO [%s] = [%s]", unSemaforo->szSemaforo, valorSemaforos[i]);
			log_debug(logger, bufferLog);
		}
	} else {
		fprintf(stderr, "Falta key 'SEMAFOROS' y/o 'VALOR_SEMAFORO' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'SEMAFOROS' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

//	//HIO y ID_HIO (Creacion de Hilo por c/u de estos)
//	if(config_has_property(config_file, "ID_HIO") && config_has_property(config_file, "HIO"))
//	{
//		//int cantIO;
//		int i;
//		id_hio = config_get_array_value(config_file, "ID_HIO");
//		hio = config_get_array_value(config_file, "HIO");
//		pthread_t thread_IO[i];
//
//		//for (cantIO = 0; id_hio[cantIO] != '\0'; cant++);
////		pthread_attr_t attr_thread_IO[cantIO];
//
//		io_list = list_create();
//
//		for (i = 0; id_hio[i] != '\0'; i++)
//		{
//			IO_t * unIO = malloc(sizeof(IO_t));
//
//			strcpy(unIO->device, id_hio[i]);
//			unIO->retardo = atoi(hio[i]);
//			sem_init(&(unIO->semaforo), 1, 0);
//			pthread_mutex_init(&unIO->mutex,NULL);
//			unIO->progEncolados = list_create();
//
//			list_add(io_list, unIO);
//
////			pthread_attr_init(&attrHilosIO[i]);
//
//			if((ret_IO = pthread_create(&thread_IO[i], NULL, (void*)&main_IO, unIO) < 0))
//			{
//				fprintf(stderr, "[Kernel]: Error on 'pthread_create()' function -  Hilo IO: [%d]\n", ret_IO);
//				exit(EXIT_FAILURE);
//			}
//
//			sprintf(bufferLog, "DEVICE = [%s] RETARDO = [%s]", unIO->device, hio[i]);
//			log_debug(logger, bufferLog);
//		}
//	} else {
//		fprintf(stderr, "Falta key 'ID_HIO' y/o 'HIO' en archivo de configuracion. Chequear.\n");
//		fprintf(stderr, "Programa abortado.\n");
//		log_error(logger,"Falta key 'ID_HIO' y/o 'HIO' en archivo de configuracion. Programa abortado.");
//		exit(1);
//	}

	//MULTIPROGRAMACION
	if(config_has_property(config_file, "MULTIPROGRAMACION"))
	{
		gradoMultiProg = config_get_int_value(config_file, "MULTIPROGRAMACION");

		sprintf(bufferLog, "MULTIPROGRAMACION = [%d]", gradoMultiProg);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr,"Falta key 'MULTIPROGRAMACION' en archivo de configuracion. Chequear.\n");
		fprintf(stderr,"Programa abortado.\n");
		log_error(logger,"Falta key 'MULTIPROGRAMACION' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//IP_KERNEL
	if(config_has_property(config_file, "IP_KERNEL"))
	{
		ip_kernel = config_get_string_value(config_file, "IP_KERNEL");

		sprintf(bufferLog, "IP_KERNEL = [%s]", ip_kernel);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//PUERTO_PROG
	if(config_has_property(config_file, "PUERTO_PROG"))
	{
		puerto_prog = config_get_string_value(config_file, "PUERTO_PROG");

		sprintf(bufferLog, "PUERTO_PROG = [%s]", puerto_prog);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_PROG' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_PROG' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//PUERTO_CPU
	if(config_has_property(config_file, "PUERTO_CPU"))
	{
		puerto_cpu = config_get_string_value(config_file, "PUERTO_CPU");

		sprintf(bufferLog, "PUERTO_CPU = [%s]", puerto_cpu);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_CPU' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_CPU' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//IP_UMV
	if(config_has_property(config_file, "IP_UMV"))
	{
		ip_umv = config_get_string_value(config_file, "IP_UMV");

		sprintf(bufferLog, "IP_UMV = [%s]", ip_umv);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_UMV' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_UMV' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//PUERTO_UMV
	if(config_has_property(config_file, "PUERTO_UMV"))
	{
		puerto_umv = config_get_string_value(config_file, "PUERTO_UMV");

		sprintf(bufferLog, "PUERTO_UMV = [%s]", puerto_umv);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_UMV' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_UMV' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//QUANTUM
	if(config_has_property(config_file, "QUANTUM"))
	{
		quantum = config_get_int_value(config_file, "QUANTUM");

		sprintf(bufferLog, "QUANTUM = [%d]", quantum);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'QUANTUM' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'QUANTUM' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//RETARDO
	if(config_has_property(config_file, "RETARDO"))
	{
		retardo = config_get_int_value(config_file, "RETARDO");

		sprintf(bufferLog, "RETARDO = [%d]", retardo);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'RETARDO' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'RETARDO' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//TAMANIO_STACK
	if(config_has_property(config_file, "TAMANIO_STACK"))
	{
		tamanio_stack = config_get_int_value(config_file, "TAMANIO_STACK");

		sprintf(bufferLog, "TAMANIO_STACK = [%d]", tamanio_stack);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'TAMANIO_STACK' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'TAMANIO_STACK' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

}

void cargar_arg_PLP(arg_PLP* arg)
{
	arg->multiprog = gradoMultiProg;
	arg->puertoProg = puerto_prog;
	arg->ipKernel = ip_kernel;
	arg->ipUMV = ip_umv;
	arg->puertoUMV = puerto_umv;
	arg->tamanioStack = tamanio_stack;
	arg->ptrLogger = logger;
}

void cargar_arg_PCP(arg_PCP* arg)
{
	arg->ipKernel = ip_kernel;
	arg-> puertoCPU = puerto_cpu;
	arg->quantum = quantum;
	arg->ptrLogger = logger;
}

int main(int argc, char **argv){

	config_path = argv[1];

	//1- Crear/Abrir file log para el registro de actividades
	logger = log_create("kernel.log","Kernel", false, LOG_LEVEL_TRACE);
	log_info(logger,"Inicio de registro de actividades.");

	//1- Leer archivo de configuracion y asignar valores
	leerConfig(config_path);

	//2- Asignar esos valores (en una estructura) como argumentos de los hilos PLP y PCP
	cargar_arg_PLP(&param_PLP);
	cargar_arg_PCP(&param_PCP);

	// Init de semaforos
	// lo UNICO que modifica al sem contador de multiprogramacion es:
	// 1) un PCB ingresando a ready -> DISMINUYE
	// 2) un PCB ingresando a exit -> INCREMENTA
	sem_init(&sem_gradoMultiProg,1,gradoMultiProg);
	//	sem_init(&productorConsumidor,1,0);
	pthread_mutex_init(&mutex_cola_new, NULL );
	pthread_mutex_init(&mutex_cola_ready, NULL );
	pthread_mutex_init(&mutex_cola_exec, NULL );
	pthread_mutex_init(&mutex_cola_block, NULL );
	pthread_mutex_init(&mutex_cola_exit, NULL );

	log_info(logger,"Se crean las estructuras necesarias del hilo.");

	//3- Levantar HILOS

		//3.1- Hilo PLP
	if((ret_PLP = pthread_create(&thread_PLP, NULL, (void*)&main_PLP, &param_PLP)))
	{
		fprintf(stderr, "[Kernel]: Error on 'pthread_create()' function - Hilo PLP: %d\n", ret_PLP);
		sprintf(bufferLog,"[Kernel]: Error on 'pthread_create()' function - Hilo PLP: %d", ret_PLP);
		log_error(logger,bufferLog);
		exit(EXIT_FAILURE);
	}

		//3.2- Hilo PCP
	if((ret_PCP = pthread_create(&thread_PCP, NULL, (void*)&main_PCP, &param_PCP)))
	{
		fprintf(stderr, "Error on 'pthread_create()' function - Hilo PCP: %d\n", ret_PCP);
		sprintf(bufferLog,"Error on 'pthread_create()' function - Hilo PCP: %d", ret_PLP);
		log_error(logger,bufferLog);
		exit(EXIT_FAILURE);
	}

		//3.3- Hilo CMD
	if((ret_CMD = pthread_create(&thread_CMD, NULL, (void*)&main_CMD, NULL)))	{
		fprintf(stderr, "Error on 'pthread_create()' function - Hilo CMD: %d\n", ret_CMD);
		sprintf(bufferLog,"Error on 'pthread_create()' function - Hilo CMD: %d", ret_CMD);
		log_error(logger,bufferLog);
		exit(EXIT_FAILURE);
	}

	//Revisar orden de espera para no entrar en espera infinta
	pthread_join(thread_PLP, NULL); //mantiene el hilo abierto para escuchar conexiones
	pthread_join(thread_PCP, NULL);
	pthread_join(thread_CMD, NULL);

	config_destroy(config_file);	//Destruimos la estructura config
	log_info(logger,"Fin de ejecucion.");
	return 0;
}
