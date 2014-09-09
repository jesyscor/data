/*
 * PCP_Operaciones_CPU.c
 *
 *  Created on: 26/05/2014
 *      Author:lemmau
 */


#include "PCP_Operaciones_CPU.h"

t_PCB *deserialize_struct_PCB(char* data)
	{
		t_PCB *pcb = malloc(sizeof(t_PCB));

		int offset = 0, tmp_size = 0;

		offset += tmp_size;
		//Identificador
		memcpy(&pcb->identificador, data, tmp_size = sizeof(pcb->identificador));
		offset += tmp_size;

		//Segmento de codigo
		memcpy(&pcb->segmentoDeCodigo, data + offset, tmp_size = sizeof(pcb->segmentoDeCodigo));
		offset += tmp_size;

		//Segmento de stack
		memcpy(&pcb->segmentoDeStack, data + offset, tmp_size = sizeof(pcb->segmentoDeStack));
		offset += tmp_size;

		//Cursor de stack
		memcpy(&pcb->cursorDeStack, data + offset, tmp_size = sizeof(pcb->cursorDeStack));
		offset += tmp_size;

		//Indice de codigo
		memcpy(&pcb->indiceDeCodigo, data + offset, tmp_size = sizeof(pcb->indiceDeCodigo));
		offset += tmp_size;

		//Indice de etiquetas
		memcpy(&pcb->indiceDeEtiquetas, data + offset, tmp_size = sizeof(pcb->indiceDeEtiquetas));
		offset += tmp_size;

		//Program counter
		memcpy(&pcb->PC, data + offset, tmp_size = sizeof(pcb->PC));
		offset += tmp_size;

		//Tamaño contexto actual
		memcpy(&pcb->tamanioCotextoActual, data + offset, tmp_size = sizeof(pcb->tamanioCotextoActual));
		offset += tmp_size;

		//Tamaño indice de etiquetas
		memcpy(&pcb->tamanioIndiceDeEtiquetas, data + offset, tmp_size = sizeof(pcb->tamanioIndiceDeEtiquetas));
		offset += tmp_size;

		return pcb;


	}


int seek_n_destroy(t_PCB* pcb_nuevo )
{
	int i;
	t_PCB *pcb_viejo;
	t_list *colaExec_list = cola_exec->elements;
	int cantidadElems = list_size(colaExec_list);

	for(i=0; i<cantidadElems; i++ ){
    pcb_viejo = list_get(colaExec_list, i);
     if(pcb_viejo->identificador == pcb_nuevo->identificador)
     {
    	 list_remove(colaExec_list, i);
    	 break;
     }

     }
return(0);
}

int handleIO(char *paquete)
{
	t_PCB *pcb;
	pcb = deserialize_struct_PCB(paquete);
	seek_n_destroy(pcb);
	encolar(BLOCK, pcb);
	//TODO:Implementar acciones para IO

	/*
	 * 1.- deserializar pcb
	 * 2.- recorrer cola exec, encontrar pcb id del recibido y eliminarlo de exec (auxiliar)
	 * 3.- encolar en la que corresponda
	 */

	return 0;
}

int handleEndProgrammPCP(char *paquete)
{
	t_PCB *pcb;
	pcb = deserialize_struct_PCB(paquete);
	seek_n_destroy(pcb);
	encolar(EXIT, pcb);
	//TODO:Implementar acciones para EndProgram
	return 0;
}

int handleQuantumFinished(char *paquete)
{
	t_PCB *pcb;
	pcb = deserialize_struct_PCB(paquete);
	seek_n_destroy(pcb);
	encolar(READY, pcb);
	//TODO:Implementar acciones para Finished
	return 0;
}

stream_t *handleHandshakePCP(mensaje* datos, int *size)
{
	return do_struct_mensaje(datos, size);
}

stream_t *serialize_struct_pcb(t_PCB* datos, int *size)
{
	*size = sizeof(t_PCB);

	char *data = (char*)malloc(*size);
	stream_t *stream = malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	memset(data, 0, *size);

	offset += tmp_size;
	//Identificador
	memcpy(data, &datos->identificador, tmp_size = sizeof(datos->identificador));
	offset += tmp_size;

	//Segmento de codigo
	memcpy(data + offset, &datos->segmentoDeCodigo, tmp_size = sizeof(datos->segmentoDeCodigo));
	offset += tmp_size;

	//Segmento de stack
	memcpy(data + offset, &datos->segmentoDeStack, tmp_size = sizeof(datos->segmentoDeStack));
	offset += tmp_size;

	//Cursor de stack
	memcpy(data + offset, &datos->cursorDeStack, tmp_size = sizeof(datos->cursorDeStack));
	offset += tmp_size;

	//Indice de codigo
	memcpy(data + offset, &datos->indiceDeCodigo, tmp_size = sizeof(datos->indiceDeCodigo));
	offset += tmp_size;

	//Indice de etiquetas
	memcpy(data + offset, &datos->indiceDeEtiquetas, tmp_size = sizeof(datos->indiceDeEtiquetas));
	offset += tmp_size;

	//Program counter
	memcpy(data + offset, &datos->PC, tmp_size = sizeof(datos->PC));
	offset += tmp_size;

	//Tamaño contexto actual
	memcpy(data + offset, &datos->tamanioCotextoActual, tmp_size = sizeof(datos->tamanioCotextoActual));
	offset += tmp_size;

	//Tamaño indice de etiquetas
	memcpy(data + offset, &datos->tamanioIndiceDeEtiquetas, tmp_size = sizeof(datos->tamanioIndiceDeEtiquetas));
	offset += tmp_size;

	stream->data = data;

	return stream;
}

stream_t *serialize_struct_quantum(t_quantum* datos, int* size)
{
	*size = sizeof(t_quantum);

	char *data = (char*)malloc(*size);
	stream_t *stream = malloc(sizeof(stream_t));
	memset(data, 0, *size);

	memcpy(data, &datos->quantum, sizeof(datos->quantum));

	stream->data = data;

	return stream;

}

stream_t *handlePCBSend(t_PCB* datos, int* size)
{
	return serialize_struct_pcb(datos, size);
}

stream_t *handleQuantumSend(t_quantum* quantum , int* size)
{

	return serialize_struct_quantum(quantum, size);

}

u_int32_t* buscar_socket_programa(u_int32_t socket){

	int i;
	u_int32_t* sk;
	u_int32_t PID_programa;
	int size_list = list_size(listaDeCPUs);
	CPU_list_nodo* nodo = malloc(sizeof(CPU_list_nodo));

	for(i = 0; i < size_list; i++)
	{	//recorrer la lista, encontrar el nodo del cpu (por el socket)
		nodo = ((CPU_list_nodo *)list_get(listaDeCPUs,i));

		if(nodo->socket == socket)
		{
			memcpy(&PID_programa, &nodo->pid, sizeof(u_int32_t));  //recuperar el PID del programa que se ejecuta
			break;
		}
	}

	sk = (u_int32_t*)dictionary_get(dictionaryProgramas,(char*) PID_programa); //buscar en el diccionario(PLP) el socket del programa (por el PID)

	return (sk);
}

int handlePrintText(char *paquete, u_int32_t socket_CPU){

	u_int32_t *socket_programa;

	socket_programa = buscar_socket_programa(socket_CPU);

	preparar_paquete(*socket_programa, ENVIAR_IMPRIMIR_TEXTO, paquete);

	return 0;
}

int handlePrintNum(char *paquete, u_int32_t socket_CPU) {

	u_int32_t *socket_programa;

	socket_programa = buscar_socket_programa(socket_CPU);

	preparar_paquete(*socket_programa, ENVIAR_IMPRIMIR_ENTERO, paquete);

	return 0;
}


int handlePIDCPU(char *datos, u_int32_t socket) //Anulo esto, lo hago de otra forma
{
	/*pid_structura *pid_struct = deserialize_struct_pid(datos);
	CPU_list_nodo *value = (CPU_list_nodo *)malloc(sizeof (CPU_list_nodo));
	char *sk = (char *)malloc(sizeof(socket));

	memcpy(&value->pid, &pid_struct->pid, sizeof(value->pid));
	value->available = true;

	memcpy(sk, &socket, sizeof(socket));

	//Meter PID y Socket de CPU en diccionario de PCP. Socket es la key y la estructura con el PID y disponibilidad el value.
	dictionary_put(dictionaryCPUs, sk, value);

	free(pid_struct);

	//return pid_struct->pid;
*/
	return 0;
}

//Creo una forma nueva.

int addCPUtoList(u_int32_t socket){

	CPU_list_nodo *value = (CPU_list_nodo *)malloc(sizeof (CPU_list_nodo));

	value->socket = socket;
	value->available = true;
	value->pid = 1;

	list_add(listaDeCPUs, value);

	printf("[Kernel-PCP]: socket [%d] agregado a la lista. \n", socket);

	return 0;
}

/*int setPIDinDiC(u_int32_t socket, u_int32_t pid){

	CPU_list_nodo *value = malloc(sizeof(CPU_list_nodo));

	//Buscar y destruir el socket available
	dictionary_remove(dictionaryCPUs, (char*)socket);

	//Meter PID Con el socket nuevo y available a false
	value->available = false;
	value->pid = pid;
	dictionary_put(dictionaryCPUs, (char*)socket, value);

	return 0;
}*/
