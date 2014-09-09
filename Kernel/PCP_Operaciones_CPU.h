/*
 * PCP_Operaciones_CPU.h
 *
 *  Created on: 26/05/2014
 *      Author:lemmau
 */

#ifndef PCP_OPERACIONES_CPU_H_
#define PCP_OPERACIONES_CPU_H_

#include "PCP_Estructuras.h"
#include "Enviar_Recibir_Paquete.h"
#include "commons/collections/list.h"
#include "Colas_Operaciones.h"
#include "commons/collections/dictionary.h"


//Variables
char *bufferCPU = NULL;
//extern dictionary
//

//Funciones
int handleIO(char *paquete);
int handleEndProgrammPCP(char *paquete);
int handleQuantumFinished(char *paquete);
stream_t *handleQuantumSend(t_quantum* quantum , int* size);
stream_t *handleHandshakePCP(mensaje* estructura, int *size);
stream_t *handlePCBSend(t_PCB* datos, int* size);
u_int32_t *buscar_socket_programa(u_int32_t socket);
int handlePIDCPU(char *paquete, u_int32_t socket);
int handlePrintText(char *paquete, u_int32_t socket);
int handlePrintNum(char *paquete, u_int32_t socket);

//Funciones y variables externas
extern t_dictionary *dictionaryCPUs;
extern mensaje* undo_struct_mensaje(char* datos);
extern stream_t* do_struct_mensaje(mensaje* datos, int* size);
extern stream_t *serialize_struct_mensaje (mensaje *msg, int *tamanho);
extern mensaje *deserialize_struct_mensaje(char *datos);
extern t_queue* cola_exec;
extern pid_structura *deserialize_struct_pid(char* datos);
//

#endif /* PCP_OPERACIONES_CPU_H_ */
