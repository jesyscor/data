/*
 * IO.h
 *
 *  Created on: 04/05/2014
 *      Author:lemmau
 */

#ifndef IO_H_
#define IO_H_

	#include <stdio.h>
	#include <unistd.h>	// usleep() function
	#include <commons/collections/list.h>
	#include <semaphore.h>
	#include <pthread.h>

	typedef struct estructura_HIO {
		char device[20];
		int retardo;
		sem_t semaforo;	// sem contador
		t_list * progEncolados;
		pthread_mutex_t mutex;
	} IO_t;

	typedef struct estructura_prog_retardo
	{
//		t_nuevoPrograma* programa;
		int retardo;
	} prog_retardo_t;

	void * main_IO(void*);	//Punto de entrada del hilo IO

#endif /* IO_H_ */
