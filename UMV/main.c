/*
 * main.c
 *TODO Implementar validaciones -segmentation fault- y -memory overload-(y mostrar estas en consola programa?)
 *TODO implementar LOGS solo por archivo, NO POR PANTALLA !!!
 *TODO VOLCAR contenido de dump en archivo externo (append)
 *TODO VER fragmentacion externa en segmentos (compactacion)
 *TODO CHEQUEAR con algun ayudante si RETARDO esta ok
 *
 *  Created on: 14/04/2014
 *      Author:lemmau
 */

#include <main.h>

void liberarBloquePrincipal() {
	free(bloquePrincipal);
}

void leerConfig() {

	config_file = config_create("config.cfg");
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!\n");
    	log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!");
		exit(1);
	}

	// SIZE_MEMORIA
	if(config_has_property(config_file, "SIZE_MEMORIA"))
	{
		sizeBloquePrincipal = config_get_int_value(config_file, "SIZE_MEMORIA");
		sprintf(bufferLog,"SIZE_MEMORIA = [%d]",sizeBloquePrincipal);
		log_info(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'SIZE_MEMORIA' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'SIZE_MEMORIA' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	bloquePrincipal = malloc(sizeBloquePrincipal);

	//Se puede inicializar la memoria, en este caso creo que conviene guardar un \0 en los bloques
	memset(bloquePrincipal,0,sizeBloquePrincipal);

	// IP_UMV
	if(config_has_property(config_file, "IP_UMV"))
	{
		ip_UMV = config_get_string_value(config_file, "IP_UMV");
		sprintf(bufferLog,"IP_UMV = [%s]", ip_UMV);
		log_info(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_UMV' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_UMV' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//PUERTO_KERNELUMV
	if(config_has_property(config_file, "PUERTO_KERNEL"))
	{
		puerto_Kernel = config_get_string_value(config_file, "PUERTO_KERNEL");
		sprintf(bufferLog,"PUERTO_KERNEL = [%s]", puerto_Kernel);
		log_info(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	// PUERTO_CPUUMV
	if(config_has_property(config_file, "PUERTO_CPU"))
	{
		puerto_CPU = config_get_string_value(config_file, "PUERTO_CPU");
		sprintf(bufferLog,"PUERTO_CPU = [%s]", puerto_CPU);
		log_info(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_CPU' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_CPU' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
}

void inicilizarEstructuras() {
	crear_ListaDeProcesos();
	crear_ListaAuxiliar();
}

void inicializarSockets(){
	int sError;
	log_trace(logger,"Esperando por conexiones entrantes.");

		sError = pthread_create(&socket_KernelUMV, NULL, inicializar_SocketKernelUMV, NULL );
		log_trace(logger,"Sockets a la escucha del Kernel.");

		if(sError) {
	    	 fprintf(stderr,"[UMV]: Error on 'pthread_create()' function -> %d\n",sError);
	    	 log_error(logger,"Error, se ha dejado de escuchar conexiones. Abortando programa.");
	         exit(EXIT_FAILURE);
		}

	    sError = pthread_create(&socket_CPUUMV, NULL, inicializar_SocketCPUUMV, NULL );
	    log_trace(logger,"Sockets a la escucha de CPUs.");

	     if(sError) {
	    	 fprintf(stderr,"[UMV]: Error on 'pthread_create()' function -> %d\n",sError);
	    	 log_error(logger,"Error, se ha dejado de escuchar conexiones. Abortando programa.");
	    	 exit(EXIT_FAILURE);
	     }
}

int main(void) {

	logger = log_create("UMV.log","UMV",false,LOG_LEVEL_TRACE);

	leerConfig();				// y reserva espacio de memoria (bloquePrincipal) de 1024 bytes
	inicilizarEstructuras();	// las listas
	inicializarSockets();

	printf("[UMV]: Consola CMD interactiva, activada.\n");
	log_trace(logger,"Consola CMD interactiva, activada.");

	while ( gestionarInput() == 0 ) {
		printf("[UMV-CMD]: A la espera de comandos...\n");
	}
	printf("[UMV]: Esperando cierre de hilos...");

	liberarBloquePrincipal();
	pthread_join(socket_KernelUMV,NULL);
	pthread_join(socket_CPUUMV,NULL);

	config_destroy(config_file);
	cerrar_SocketKernelUMV();
	log_destroy(logger);
	return 0;
}

