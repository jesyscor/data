/*
 * PLP.h
 *
 *  Created on: 04/05/2014
 *      Author:lemmau
 */

#ifndef PLP_H_
#define PLP_H_

	#define DELAY_CHECK_NEW_READY_SEC 1
	#define DELAY_CHECK_NEW_READY_USEC 0

    #include <stdlib.h>
	#include <stdio.h>
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

	#include <parser/metadata_program.h>
	#include <commons/collections/dictionary.h>
	#include <commons/collections/list.h>
	#include <commons/collections/queue.h>
	#include <commons/log.h>

	#include <sockets/Enviar_Recibir_Paquete.h>
	#include "PLP_Estructuras.h"
	#include "Estructuras_Auxiliares.h"
	#include "Colas_Operaciones.h"

	// Vars de configuracion
	int gradoMultiProg;
	int tamanio_stack;
	char* ip_kernel;
	char* puerto_prog;
	char* ip_umv;
	char* puerto_umv;

	mensaje msg;

	// Vars globales PLP
	t_list* cola_new;
	t_queue* cola_ready;
	t_queue* cola_exit;
	t_dictionary *dictionaryProgramas;

	typedef struct arg_PLP { // Estructura para pasar argumentos al hilo PLP
		char* ipKernel;
		char* puertoProg;
		char* puertoUMV;
		char* ipUMV;
		int multiprog;
		int tamanioStack;
		t_log* ptrLogger;
	} arg_PLP;

	// FUNCIONES PPALES

		// Funciones Grales
	void *get_in_addr(struct sockaddr *);
	int crear_listener(const char *, const char *); // Crea un socket de escucha.
	t_medatada_program *pre_procesar_script(const char * script);
	int analizar_paquete(u_int32_t socket, char *paquete, operaciones_t *id);
	int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura);
	void* main_PLP(void*); // Punto de entrada del hilo PLP

		// Funciones y variables PLP_Operaciones_Programa.h
	extern char* script;
	extern char *bufferPRG;
	extern int handlePID(char* datos, u_int32_t socket);
	extern stream_t *handlePIDResult(mensaje *datos, int *size);
	extern int handleScriptLine(char* datos);
	extern int handleScriptEOF(char *datos);
	extern stream_t *handleHandshake(mensaje* datos, int* size);
	extern stream_t *handleScriptResult(mensaje *datos, int *size);
	extern stream_t *handlePrintMsg(mensaje *datos, int *size);
	extern stream_t *handleEndProgramm(mensaje *datos, int *size);

		// Funciones y variables PLP_Operaciones_UMV.h
	extern char* bufferUMV;
	extern t_PCB info_mem_UMV;
	extern int handleHandshakeUMV(char *datos);
	extern stream_t *handleSolicitudMemoria(paq_SolicitudMemoria* datos, int *size);
	extern int handleMemoriaSuccess(char* datos);
	extern int handleMemoriaFail(char* datos);
	extern int solicitarMemoria(t_medatada_program *metadata, u_int32_t pid, u_int32_t size, char* datos);
	extern u_int32_t calcular_peso(u_int32_t cant_de_etiq, u_int32_t cant_de_func, u_int32_t cant_tot_de_lineas_de_cod);

		// Funciones operaciones con cola
	extern int encolar(t_listas_operaciones cola, t_PCB* pcb);
	extern void check_new_ready();
	extern void check_exit();

#endif /* PLP_H_ */
