/*
 * PLP_Operaciones_Programa.h
 *
 *  Created on: 19/05/2014
 *      Author:lemmau
 */

#ifndef PLP_OPERACIONES_PROGRAMA_H_
#define PLP_OPERACIONES_PROGRAMA_H_

	#include <stdlib.h>
	#include <stdio.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include "PLP_Estructuras.h"
	#include <sockets/Enviar_Recibir_Paquete.h>
	#include "commons/collections/dictionary.h"


	extern t_dictionary *dictionaryProgramas;

	char* script = "";
	char *bufferPRG = NULL;

	pid_structura *deserialize_struct_pid(char* datos);
	pid_structura* undo_struct_pid(char* datos);
	int handlePID(char* datos, u_int32_t socket);
	stream_t *handlePIDResult(mensaje *datos, int *size);
	int handleScriptLine(char* datos);
	int handleScriptEOF(char *datos);
	stream_t *handleHandshake(mensaje* datos, int* size);
	stream_t *handleScriptResult(mensaje *datos, int *size);
	stream_t *handlePrintMsg(mensaje *datos, int *size);
	stream_t *handleEndProgramm(mensaje *datos, int *size);
	int recibir_script(int socket);
//	int atender_Programa(void* parametros);
//	int addCPUtoDic(u_int32_t socket);


#endif /* PLP_OPERACIONES_PROGRAMA_H_ */
