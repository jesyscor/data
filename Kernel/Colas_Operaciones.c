/*
 * Colas_Operaciones.c
 *
 *  Created on: 24/05/2014
 *      Author:lemmau
 */

#include "Colas_Operaciones.h"

int encolar(t_listas_operaciones cola, t_PCB* pcb)
{
	int res;
	switch(cola) {
		case NEW:
			res = encolar_in_new(pcb);
			break;
		case READY:
			res = encolar_in_ready(pcb);
			break;
		case EXIT:
			res = encolar_in_exit(pcb);
			break;
		case EXEC:
			res = encolar_in_exec(pcb);
			break;
		case BLOCK:
			res = encolar_in_block(pcb);
			break;
	}
	return res;
}

int encolar_in_new(t_PCB* pcb)
{	// TODOS los programas pasan por aca !!
	// Agregamos al programa a la la lista y reordenamos la lista segun peso

//	pthread_mutex_lock(&mutex_cola_new);
	list_add(cola_new, pcb);
	list_sort(cola_new, sorter);
//	pthread_mutex_unlock(&mutex_cola_new);

	return 0;
}

// PLP
int encolar_in_ready(t_PCB* pcb)
{
	sem_wait(&sem_gradoMultiProg);	// decremento de sem contador
	queue_push(cola_ready, pcb);

	printf("encolar_in_ready ->");
	printf("id [%d],peso [%d],segcod [%d] cursordestack [%d],indicedecodigo [%d],indicedeetiq [%d],segmentodestack [%d],tam contexto actual [%d], tam ind etiq [%d]\n",
			pcb->identificador,pcb->peso,pcb->segmentoDeCodigo,
			pcb->cursorDeStack,pcb->indiceDeCodigo,pcb->indiceDeEtiquetas,pcb->segmentoDeStack,
			pcb->tamanioCotextoActual,pcb->tamanioIndiceDeEtiquetas);

//	pcb = desencolar_in_ready();
//	printf("encolar_in_ready->");
//	printf("id [%d],peso [%d],segcod [%d] cursordestack [%d],indicedecodigo [%d],indicedeetiq [%d],segmentodestack [%d],tam contexto actual [%d], tam ind etiq [%d]\n",
//			pcb->identificador,pcb->peso,pcb->segmentoDeCodigo,
//			pcb->cursorDeStack,pcb->indiceDeCodigo,pcb->indiceDeEtiquetas,pcb->segmentoDeStack,
//			pcb->tamanioCotextoActual,pcb->tamanioIndiceDeEtiquetas);

	return 0;
}

int encolar_in_exit(t_PCB* pcb)
{
//	pthread_mutex_lock(&mutex_cola_exit);
	sem_post(&sem_gradoMultiProg);	// incremento de sem contador
	queue_push(cola_exit, pcb);		// y retiro elem de la cola
//	pthread_mutex_unlock(&mutex_cola_exit);

	return 0;
}

int encolar_in_exec(t_PCB* pcb)
{
//	pthread_mutex_lock(&mutex_cola_exec);
	queue_push(cola_exec, pcb);
//	pthread_mutex_unlock(&mutex_cola_exec);
	return 0;
}

int encolar_in_block(t_PCB* pcb)
{
//	pthread_mutex_lock(&mutex_cola_block);
	queue_push(cola_block, pcb);
//	pthread_mutex_unlock(&mutex_cola_block);

	return 0;
}

t_PCB* desencolar(t_listas_operaciones cola)
{
	t_PCB* res;

	switch(cola) {
		case NEW:
			res = desencolar_in_new();
			break;
		case READY:
			res = desencolar_in_ready();
			break;
		case EXIT:
			res = desencolar_in_exit();
			break;
		case EXEC:
			res = desencolar_in_exec();
			break;
		case BLOCK:
			res = desencolar_in_block();
			break;
	}
	return res;
}

t_PCB* desencolar_in_new()
{
	//TODO: implementar metodo
	//TODO: implementar sincronizacion.

	t_PCB *elemento = malloc(sizeof(t_PCB));

	return elemento;
}

t_PCB* desencolar_in_ready()
{
	//t_PCB *elemento;
	t_PCB *elemento = malloc(sizeof(t_PCB));

//	pthread_mutex_lock(&mutex_cola_ready);
	elemento = (t_PCB*) queue_pop(cola_ready);
//	pthread_mutex_unlock(&mutex_cola_ready);

	return elemento;
}

t_PCB* desencolar_in_exit()
{
	//TODO: implementar metodo
	//TODO: implementar sincronizacion.

	t_PCB *elemento;

	elemento = malloc(sizeof(t_PCB));

	return elemento;
}

t_PCB* desencolar_in_exec()
{
	//TODO: implementar metodo
	//TODO: implementar sincronizacion.

	t_PCB *elemento;

	elemento = malloc(sizeof(t_PCB));

	return elemento;
}

t_PCB* desencolar_in_block()
{
	//TODO: implementar metodo
	//TODO: implementar sincronizacion.

	t_PCB *elemento;
	elemento = malloc(sizeof(t_PCB));
	//	pthread_mutex_lock(&mutex_cola_ready);
		elemento = (t_PCB*) queue_pop(cola_block);
	//	pthread_mutex_unlock(&mutex_cola_ready);
	return elemento;
}

// PLP
void check_new_ready(int multiprog)
{
	t_PCB* elemento;

	if(!list_is_empty(cola_new))
	{
		if (multiprog > 0)
		{
//			pthread_mutex_lock(&mutex_cola_new);
//			pthread_mutex_lock(&mutex_cola_ready);
			elemento = list_remove(cola_new, 0);
			encolar_in_ready(elemento);
//			pthread_mutex_unlock(&mutex_cola_ready);
//			pthread_mutex_unlock(&mutex_cola_new);

//			sem_post(&productorConsumidor);

			free(elemento);

			sprintf(bufferLog, "Se agrego a cola_ready el programa [%d] con peso [%d]", elemento->identificador, elemento->peso);
			log_debug(logger, bufferLog);
		}
		else
		{
			// No hay lugar para proceso alguno
		}

	}
}

void check_exit()
{
	//Terminar Programa (mensaje a traves del file descriptor)
	//Sacarlo del diccionario
	//Cierra conexion socket
	//TODO Borrar todos los segmentos del PCB
	//Eliminar file descriptor
	//Remove and destroy

	mensaje msg;
	t_PCB* elemento;
	u_int32_t *socket;

	if(!queue_is_empty(cola_exit))
	{
//		pthread_mutex_lock(&mutex_cola_exit);
		elemento = queue_pop(cola_exit);
		msg.out = "[Kernel-PLP]: Fin de instruccion alcanzado. Hasta la vista Programa!";
		socket = (u_int32_t*) dictionary_remove(dictionaryProgramas, (char*)elemento->identificador);
		preparar_paquete(*socket, END_PROGRAMM, &msg);
		close(*socket);
		FD_CLR(*socket, &master);
//		pthread_mutex_unlock(&mutex_cola_exit);

		free(elemento);
	}
}

/* PCP Chequea si hay elementos para pasar de ready a exec,
 * y en caso de pasarlos, manda el PCB a la CPU, en caso de haber alguno disponible.*/
void check_ready_exec()
{
	int i;
	t_PCB* elemento;
//	t_PCB* elemento = malloc(sizeof(t_PCB));
	int size_cola_exec = queue_size(cola_exec);
	int size_list = list_size(listaDeCPUs);
	CPU_list_nodo* nodo = malloc(sizeof(CPU_list_nodo));


	if(queue_is_empty(cola_ready)){
			printf("cola vacia \n");
		}
		else
		{
			printf("cola con contenido.\n");
			elemento = desencolar_in_ready();
//			memcpy(&variable,&elemento,(sizeof(t_PCB)));
			printf("check_ready_exec->");
			printf("id [%d],peso [%d],segcod [%d] cursordestack [%d],indicedecodigo [%d],indicedeetiq [%d],segmentodestack [%d],tam contexto actual [%d], tam ind etiq [%d]\n",
					elemento->identificador,elemento->peso,elemento->segmentoDeCodigo,
					elemento->cursorDeStack,elemento->indiceDeCodigo,elemento->indiceDeEtiquetas,elemento->segmentoDeStack,
					elemento->tamanioCotextoActual,elemento->tamanioIndiceDeEtiquetas);
		}

	if (!list_is_empty(listaDeCPUs))	// mientas exista alguna CPU arriba
	{
		if(size_list > size_cola_exec) // Si es cierto => hay CPUs disponibles
		{
			//Me fijo si hay algo en Ready
			if (queue_is_empty(cola_ready))
				return;

//			pthread_mutex_lock(&mutex_cola_ready);
			elemento = desencolar(READY);
//			pthread_mutex_unlock(&mutex_cola_ready);

//printf("check_ready_exec->");
//printf("id [%d],peso [%d],segcod [%d] cursordestack [%d],indicedecodigo [%d],indicedeetiq [%d],segmentodestack [%d],tam contexto actual [%d], tam ind etiq [%d]\n",
//		elemento->identificador,elemento->peso,elemento->segmentoDeCodigo,
//		elemento->cursorDeStack,elemento->indiceDeCodigo,elemento->indiceDeEtiquetas,elemento->segmentoDeStack,
//		elemento->tamanioCotextoActual,elemento->tamanioIndiceDeEtiquetas);

//			pthread_mutex_lock(&mutex_cola_exec);
			encolar(EXEC, elemento);
//			pthread_mutex_unlock(&mutex_cola_exec);

//			CPU_list_nodo* cpu = (CPU_list_nodo*) list_get(listaDeCPUs, 0);
//			bool estado_cpu = cpu->available;
//			u_int32_t socket_cpu = cpu->socket;

//			printf("[Kernel] Estado del CPU nro [%d] = [%d] \n", socket_cpu, estado_cpu);
//			printf("[Kernel] El tamanio del diccionario es: [%d] \n", size_list);

			for(i = 0; i < size_list; i++)	//Recorre lista de CPUs para ver cual esta disponible
			{
				nodo = ((CPU_list_nodo *)list_get(listaDeCPUs,i));
				if(nodo->available)
				{
					memcpy(&nodo->pid, &elemento->identificador, sizeof(u_int32_t));
printf("valor del pid [%d] \n", elemento->identificador);
					break;
				}
			}
			preparar_paquete_PCP(nodo->socket, PCB_SEND, elemento);
		}
	}
}

bool sorter(void *elemento1, void * elemento2)
{
	t_PCB * program_A = (t_PCB*) elemento1;
	t_PCB * program_B = (t_PCB*) elemento2;

	if (program_A->peso <= program_B->peso)	{
		return true;
	} else	{
		return false;
	}
}

