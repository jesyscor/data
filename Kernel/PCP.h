/*
 * PCP.h
 *
 *  Created on: 04/05/2014
 *      Author:lemmau
 */

#ifndef PCP_H_
#define PCP_H_

	#define MAXDATASIZE 256
	#define DELAY_CHECK_READY_SEC 1
	#define DELAY_CHECK_READY_USEC 0

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

	#include <commons/collections/queue.h>
	#include <commons/log.h>

	#include "Enviar_Recibir_Paquete.h"
	#include "Estructuras_Auxiliares.h"
	#include "PCP_Estructuras.h"
	#include "Colas_Operaciones.h"

	// Vars de configuracion
	char* puerto_cpu;
	int quantum;

	char* pcb;
	mensaje msg_PCP;
	t_log* logger;
//	char bufferLog[80];

	typedef struct arg_PCP	{	// Estructura para pasar argumentos al hilo PCP
		char* ipKernel;
		char* puertoCPU;
		int quantum;
		t_log* ptrLogger;
	} arg_PCP;

	typedef struct arg_CPU {// Estructura para trackear info de CPU
		int fd_CPU; 		// Socket file desciptor
		u_int32_t pid_CPU;	// Process ID
	} arg_CPU;

	// Vars globales PCP
	t_queue* cola_exec;
	t_queue* cola_block;
	t_list* listaDeCPUs;

	int analizar_paquete(u_int32_t socket, char *paquete, operaciones_t *id);
	// Funciones
	void *get_in_addr_PCP(struct sockaddr *sa);
//	int enviar_msg_PCP(const void *mensaje, int fd);
	int crear_listener_PCP(const char *, const char *); // Crea un socket de escucha.
	void* main_PCP(void* parametros);
	t_queue *queue_create();
	void queue_push(t_queue *self, void *element);
	void *queue_peek(t_queue *self);
	void handshake_CPU(void* parametros);
	void* set_track_CPU(int fd);
	stream_t *serialize_struct_mensaje_PCP (t_PCB pcb);
	int preparar_paquete_PCP(u_int32_t socket, operaciones_PCP op, void* estructura);
	int addCPUtoList(int newfd_PCP);
	int handlePrintText(char *paquete, u_int32_t socket);
	int handlePrintNum(char *paquete, u_int32_t socket);

	//Variables y funciones PCP_Operaciones_CPU.h
	extern char *bufferCPU;
	extern int handleIO(char *paquete);
	extern int handleEndProgrammPCP(char *paquete);
	extern int handleQuantumFinished(char *paquete);
	extern stream_t *handleQuantumSend(t_quantum* quantum , int* size);
	extern stream_t *handleHandshakePCP(mensaje* estructura, int *size);
	extern stream_t *handlePCBSend(t_PCB* datos, int* size);
	extern int handlePIDCPU(char *paquete, u_int32_t socket);

	extern int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura);

#endif /* PCP_H_ */

