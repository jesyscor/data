/*
 * cpuHandler.c
 *
 *  Created on: 25/05/2014
 *      Author:lemmau
 */

#include <cpuHandler.h>

//HANDSHAKE
stream_t *serialize_struct_handshake_CPU(paq_Handshake *hns, int *size) {
	*size = strlen(hns->data) + 1;
	stream_t *stream = malloc(sizeof(stream_t));
	stream->data = malloc(*size);

	memcpy(stream->data, hns->data, strlen(hns->data) + 1);

	return stream;
}

int handShakeConCPU(int fdSock) {

	paq_Handshake hns;
	stream_t * serializado = malloc(sizeof(stream_t));
	int size;

	hns.data = malloc(strlen("[UMV]: Handshake con UMV, exitoso! \n"));
	hns.data = "[UMV]: Handshake con UMV, exitoso! \n";

	serializado = serialize_struct_handshake_CPU(&hns, &size);

	send_msg_to(fdSock, HANDSHAKE_SUCCESS, serializado->data, size);

	return 0;
}

//FIN HANDSHAKE

//SERIALIZADOS

/*Serializo y envio la instruccion esto no va mas, se usa en CPU
int enviar_Instruccion_Res(int fdSock, char* instruccion) {

	stream_t * serializado = malloc(sizeof(stream_t));
	int size;

	serializado->data = malloc(strlen(instruccion));
	serializado->data = instruccion;

	size = strlen(instruccion);

	send_msg_to(fdSock, SOLICITUD_INSTRUCCION_RESPUESTA, serializado->data,
			size);

	return 0;
}*/

stream_t *serialize_struct_res_Lectura (char* buffer, u_int32_t size, u_int32_t *sizePaquete)
{
	stream_t *stream = malloc(sizeof(stream_t));

	*sizePaquete = sizeof(u_int32_t) + size;

	stream->data = (char*)malloc(*sizePaquete);

	memcpy(stream->data, &size, sizeof(u_int32_t));
	memcpy(stream->data + sizeof(u_int32_t), buffer, size);

	return stream;
}


int enviar_res_Lectura(int fdSock, char* buffer, u_int32_t size) {

	u_int32_t sizePaquete;

	stream_t * serializado = serialize_struct_res_Lectura(buffer,size,&sizePaquete);

	send_msg_to(fdSock, SOLICITUD_LECTURA_RESPUESTA, serializado->data,
			sizePaquete);

	free(serializado);

	return 1;
}

int enviar_res_Escritura(int fdSock, u_int32_t posicion) {

	stream_t * serializado = malloc(sizeof(stream_t));
	int size;
	size = sizeof(u_int32_t);

	serializado->data = malloc(size);

	memcpy(serializado->data, &posicion, size);

	send_msg_to(fdSock, SOLICITUD_ESCRITURA_RESPUESTA, serializado->data,
			size);

	free(serializado);

	return 1;
}

//DESERIALIZADOS

/*paq_sol_Instruccion deserialize_struct_Instruccion_Rec(char *mensaje) {
	paq_sol_Instruccion solicitud;

	memcpy(&solicitud, mensaje, sizeof(paq_sol_Instruccion));

	return solicitud;
}

paq_sol_Push deserialize_struct_Push_Rec(char *mensaje) {
	paq_sol_Push solicitud;

	memcpy(&solicitud, mensaje, sizeof(paq_sol_Push));

	return solicitud;
}*/

paq_sol_EscribirMemoria *deserialize_struct_Solicitud_Escritura(char *mensaje){
	paq_sol_EscribirMemoria *solicitud;

	solicitud = malloc(sizeof(paq_sol_EscribirMemoria));

	int offset = 0;

	memcpy(&solicitud->id_proceso, mensaje, sizeof(u_int32_t));
	offset+=sizeof(u_int32_t);

	memcpy(&solicitud->base,mensaje + offset,sizeof(u_int32_t));
	offset+=sizeof(u_int32_t);

	memcpy(&solicitud->offset,mensaje + offset,sizeof(u_int32_t));
	offset+=sizeof(u_int32_t);

	memcpy(&solicitud->size,mensaje + offset,sizeof(u_int32_t));
	offset+=sizeof(u_int32_t);

	solicitud->buffer = malloc(solicitud->size);
	memcpy(solicitud->buffer,mensaje + offset, solicitud->size);

	printf("Guardo el numero %d \n", (u_int32_t)*solicitud->buffer);

	return solicitud;
}

int handleSolicitudEscritura(int fdsock, char *mensaje) {

	paq_sol_EscribirMemoria *solicitud = deserialize_struct_Solicitud_Escritura(mensaje);
	u_int32_t posicion;

	posicion = guardarBloque(solicitud->id_proceso,solicitud->base,solicitud->offset,solicitud->size, solicitud->buffer);

	enviar_res_Escritura(fdsock, posicion);
	free(solicitud);

	return 0;
}

paq_sol_LeerMemoria *deserialize_struct_Solicitud_Lectura(char *mensaje) {
	paq_sol_LeerMemoria *solicitud;

	solicitud = malloc(sizeof(paq_sol_LeerMemoria));

	int offset = 0;

	memcpy(&solicitud->id_proceso, mensaje, sizeof(u_int32_t));
	offset+=sizeof(u_int32_t);

	memcpy(&solicitud->base,mensaje + offset,sizeof(u_int32_t));
	offset+=sizeof(u_int32_t);

	memcpy(&solicitud->offset,mensaje + offset,sizeof(u_int32_t));
	offset+=sizeof(u_int32_t);

	memcpy(&solicitud->size,mensaje + offset,sizeof(u_int32_t));

	return solicitud;
}

int handleSolicitudLectura(int fdsock, char *mensaje) {

	paq_sol_LeerMemoria *solicitud = deserialize_struct_Solicitud_Lectura(mensaje);
	char* buffer;

	buffer = solicitarBloque(solicitud->id_proceso, solicitud->base, solicitud->offset, solicitud->size);

printf ("buffer = [%s]\n",buffer);

	if (!buffer || buffer == NULL) {
		perror("[UMV] Segmentation Fault al Leer");
		return 0;
	}

	if (solicitud->size > 0){
		enviar_res_Lectura(fdsock, buffer, solicitud->size);
		free(solicitud);
		return 1;
	}

	return 0;
}

//FIN DESERIALIZADOS

//OPERACIONES SOBRE LA UMV ESTO VA EN CPU
/*
char* recuperar_Instruccion(paq_sol_Instruccion solicitud) {
	u_int32_t desde, hasta;

	printf(
			"Detalles de la solicitud: ID: %d   Base Indice: %d  NumeroInstruccion: %d \n",
			solicitud.id_proceso, solicitud.baseIndiceCodigo,
			solicitud.numeroDeInstruccion);
	char* indiceARecuperar = malloc(2 * sizeof(u_int32_t));
	//Obtengo los bytes a leer segun el indice de código
	indiceARecuperar = solicitarBloque(solicitud.id_proceso,
			solicitud.baseIndiceCodigo,
			solicitud.numeroDeInstruccion * sizeof(u_int32_t),
			2 * sizeof(u_int32_t));

	memcpy(&desde, indiceARecuperar, sizeof(u_int32_t));
	memcpy(&hasta, indiceARecuperar + sizeof(u_int32_t), sizeof(u_int32_t));

	char* instruccion = (char*) malloc(hasta + 1);

	instruccion = solicitarBloque(solicitud.id_proceso,
			solicitud.baseCodeSegment, desde, hasta);

	instruccion[hasta] = '\0';

	printf(
			"\n Desde:%d Hasta: %d Bloque Recuperado: %s    FIN BLOQUE RECUPERADO\n",
			desde, hasta, instruccion);

	return instruccion;

}

*/

//FIN OPERACIONES SOBRE LA UMV
int cpu_Listen(int fdCPU) {

	char *mensaje;
	int idMensaje;
	int res;

	recv_msg_from(fdCPU, &idMensaje, &mensaje);

	switch (idMensaje)
		{
		case SOLICITUD_LECTURA_RECIBIDA:
			res = handleSolicitudLectura(fdCPU,mensaje);
			break;
		case SOLICITUD_ESCRITURA_RECIBIDA:
			res = handleSolicitudEscritura(fdCPU,mensaje);
			break;
		}
	return res;
}

void *cpu_Handler(void *args) {
	int fdCPU = (int) args;

	handShakeConCPU(fdCPU);
	printf("[UMV]: Nueva CPU nro [%d] \n", fdCPU);

	while (cpu_Listen(fdCPU) >= 0)
	{}

	printf("[UMV]: Cerrada CPU nro: [%d] \n", fdCPU);

	return 0;
}
