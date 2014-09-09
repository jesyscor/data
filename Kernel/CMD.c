/*
 * CMD.c
 *
 *  Created on: 25/07/2014
 *      Author: utnso
 */

#include "CMD.h"

/* TODO LLAMAR lista_new en lugar de cola_new y enlistar() en lugar de encolar() para no prestar confusion
 * TODO mostrar programas (PID) bloqueados por IO y por SEMAFOROS
 */

void* main_CMD(void* parametro)
{
	char input[iMAX_LEN];

	printf("[Kernel]: Consola CMD interactiva, activada\n");

	while (1)
	{
		gets(input);

		if (!string_equals_ignore_case(input,"\n"))
		{
			if ((string_equals_ignore_case(input, "colas")))
			{
				show_all();
			}
			else
			{
				puts("Comando invalido. Volver a intentar.\n");
			}
		}
	}
	pthread_exit(0);
	return 0 ;
}

void showNewProgramsPLP(void* elemento)
{
	t_PCB* elem = (t_PCB*) elemento;
	printf("|\t PID \t\t|\t PESO \t|");
	printf("\n---------------------------------------------\n");
	printf("|\t[ %d ]\t|\t[ %d ]\t| \n", elem->identificador, elem->peso);

}

void printList(void* prog)
{
	t_PCB* progActual = (t_PCB*) prog;
	printf("PID: [%d] \n", progActual->identificador);
}

int show_all()
{
	//	only for debug purpose
	//	t_PCB* nodo = ((t_PCB*)list_get(cola_new,0));
	//	printf("PID.identificador -> %d\n",nodo->identificador);

	if(list_is_empty(cola_new))
	{
		puts("COLA NEW = [EMPTY]");
	}
	else
	{
	puts("\n******** Cola NEW ******** \n");
	list_iterate(cola_new, showNewProgramsPLP);
	printf("/\n");
	}

	puts("\nEstados de las restantes colas: \n");

	if(queue_is_empty(cola_ready))
	{
		puts("COLA READY = [EMPTY]\n");
	}
	else
	{
		puts("COLA READY = [NOT EMPTY]\n");
		t_PCB* elemento = queue_pop(cola_ready);
printf("id [%d],peso [%d], segcod [%d]\n",elemento->identificador, elemento->peso, elemento->segmentoDeCodigo);
	}

	if(queue_is_empty(cola_exec))
	{
		puts("COLA EXEC = [EMPTY]\n");
	}
	else
	{
		puts("COLA EXEC = [NOT EMPTY]\n");
	}

	if(queue_is_empty(cola_block))
	{
		puts("COLA BLOCK = [EMPTY]\n");
	}
	else
	{
		puts("COLA BLOCK = [NOT EMPTY]\n");
	}

//	for (i = 0; dictionarySemaforos[i] != '\0'; i++)
//	{
//		bool bfnEncuentraSem(void* param)
//		{
//			semaforo_t *unSem = (semaforo_t*) param;
//			return strcmp(unSem->szSemaforo, dictionarySemaforos[i]) == 0 ? 1 : 0;
//		}
//
//		semaforo_t semaf = (semaforo_t*) list_find(sem_list, bfnEncuentraSem);
//
//		printf("====================================\n");
//		printf("Lista de programas bloqueados por el semaforo [%s]:\n", dictionarySemaforos[i]);
//		list_iterate(semaf->progEncolados, printList);
//		printf("/\n");
//	}
	return 0;
}
