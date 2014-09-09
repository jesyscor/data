/*
 * IO.c
 *
 *  Created on: 04/05/2014
 *      Author:lemmau
 */

#include "IO.h"

//void* main_IO(void* parametro)
//{
//	IO_t *unIO = (IO_t*) parametro;
//	prog_retardo_t* programa;

//	while (1)
//	{
//		sem_wait(&(unIO->semaforo));	// decremento sem contador
//		pthread_mutex_lock(&(unIO->mutex));		// lock cerrojo
//		programa = (prog_retardo_t*) list_remove(unIO->progEncolados, 0);
//		pthread_mutex_unlock(&(unIO->mutex));	// unlock cerrojo
//
//		usleep(unIO->retardo * programa->retardo * 1000);
//
//		printf("El programa [%d] estuvo bloqueado [%d] milisegundos.\n",
//				programa->programa->pcb->pid, unIO->retardo * programa->retardo);
//
//		pthread_mutex_lock(&mutexReady);	// lock cerrojo
//		list_add_in_index(listaReady, listaReady->elements_count, programa->programa);
//		pthread_mutex_unlock(&mutexReady);	// unlock cerrojo
//
//		sem_post(&productorConsumidor);	// incremento sem contador
//
//		free(programa);
//	}
//	return 0 ;
//}
