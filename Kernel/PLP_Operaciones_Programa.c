/*
 * PLP_Operaciones_Programa.c
 *
 *  Created on: 19/05/2014
 *      Author:lemmau
 */

#include "PLP_Operaciones_Programa.h"

pid_structura* deserialize_struct_pid(char* datos)
{
	pid_structura *self = malloc(sizeof(pid_structura));
	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	memcpy(&self->pid, datos + offset, tmp_size = sizeof(self->pid));

	return self;
}

pid_structura* undo_struct_pid(char* datos)
{
	pid_structura* deserializado = deserialize_struct_pid(datos);

	return deserializado;
}


int handlePID(char* datos, u_int32_t socket)
{
	pid_structura *pid_struct = undo_struct_pid(datos);
	u_int32_t *sk = (u_int32_t*) malloc(sizeof(u_int32_t));
	memcpy(sk, &socket, sizeof(u_int32_t));

	// Meter PID y Socket de Programa en diccionario de PLP.
	// key=PID -> value=SOCKET
	dictionary_put(dictionaryProgramas, (char*)&pid_struct->pid, sk);

	return pid_struct->pid;
}

stream_t *handlePIDResult(mensaje *datos, int *size)
{
	return do_struct_mensaje(datos, size);
}

int handleScriptLine(char* datos)
{
	mensaje *msg = undo_struct_mensaje(datos);
	char* script_sucio;

	script_sucio = msg->out;
	script = malloc(strlen(script_sucio));

	//Limpio el script ?
	int i,j = 0;
	for (i = 0; i <= strlen(script_sucio); i++){
		if (script_sucio[i] != '\t')
		{
			script[j] = script_sucio[i];
			//printf("El script va quedando...: %d -> [%c] \n",j, script[j]);	//only for debug purpose
			j++;
		}
	}

//	printf("El script quedo: %s \n", script);

	free(msg->out);
	free(msg);

	return 0;
}

int handleScriptEOF(char *datos)
{
	return 0;
}

stream_t *handleHandshake(mensaje* datos, int* size)
{
	return do_struct_mensaje(datos, size);
}

stream_t *handleScriptResult(mensaje *datos, int *size)
{
	return do_struct_mensaje(datos, size);
}

 stream_t *handlePrintMsg(mensaje *datos, int *size)
 {
	 return do_struct_mensaje(datos, size);
 }

 stream_t *handleEndProgramm(mensaje *datos, int *size)
 {
	 return do_struct_mensaje(datos, size);
 }
