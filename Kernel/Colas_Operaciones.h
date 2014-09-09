/*
 * Colas_Operaciones.h
 *
 *  Created on: 24/05/2014
 *      Author:lemmau
 */

#ifndef COLAS_OPERACIONES_H_
#define COLAS_OPERACIONES_H_

	#include "commons/collections/list.h"
	#include "commons/collections/queue.h"
	#include "commons/collections/dictionary.h"
	#include <unistd.h>

	#include "Estructuras_Auxiliares.h"
	#include "PLP_Estructuras.h"
	#include "PCP_Estructuras.h"
	#include <pthread.h>	// para el uso de los semaforos y CMD del kernel

	typedef enum Colas	{
		NEW,
		READY,
		EXIT,
		EXEC,
		BLOCK
	} t_listas_operaciones;

 	//Declaradas en PLP
	extern t_list* cola_new;
	extern t_queue* cola_ready;
	extern t_queue* cola_exit;

	//Declaradas en PCP
	extern t_queue* cola_exec;
	extern t_queue* cola_block;

	//FUNCIONES PROPIAS
	int encolar(t_listas_operaciones cola, t_PCB* pcb);
	int encolar_in_new(t_PCB* pcb);
	int encolar_in_ready(t_PCB* pcb);
	int encolar_in_exit(t_PCB* pcb);
	int encolar_in_exec(t_PCB* pcb);
	int encolar_in_block(t_PCB* pcb);
	t_PCB* desencolar(t_listas_operaciones cola);
	t_PCB* desencolar_in_new();
	t_PCB* desencolar_in_ready();
	t_PCB* desencolar_in_exit();
	t_PCB* desencolar_in_exec();
	t_PCB* desencolar_in_block();
	void check_new_ready();
	void check_exit();
	void check_ready_exec();
	bool sorter(void* elem1, void* elem2);

	//FUNCIONES Y VARIABLES EXTERNAS EN PLP
	extern fd_set master;
	extern t_dictionary *dictionaryProgramas;
	extern int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura);

	//FUNCIONES Y VARIABLES EXTERNAS EN PCP
	extern t_list *listaDeCPUs;
	extern int preparar_paquete_PCP(u_int32_t socket, operaciones_PCP op, void* estructura);

#endif /* COLAS_OPERACIONES_H_ */
