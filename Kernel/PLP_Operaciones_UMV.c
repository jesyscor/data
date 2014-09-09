/*
 * PLP_Operaciones_UMV.c
 *
 *  Created on: 19/05/2014
 *      Author:lemmau
 */

#include "PLP_Operaciones_UMV.h"

stream_t *serialize_struct_solicitud_memoria(paq_SolicitudMemoria* datos, int *size)
{
	*size = sizeof(u_int32_t)*2 + datos->size;

		stream_t *stream = malloc(sizeof(stream_t));
		stream->data = malloc(*size);

		int offset = 0, tmp_size = 0;

		memset(stream->data, 0, *size);

		offset += tmp_size;

		//PID
		memcpy(stream->data, &datos->id_proceso, tmp_size = sizeof(u_int32_t));
		offset += tmp_size;

		//Datos lenght
		memcpy(stream->data + offset, &datos->size, tmp_size = sizeof(u_int32_t));
		offset += tmp_size;

		//Datos
		memcpy(stream->data + offset, datos->datos, tmp_size = datos->size);
		offset += tmp_size;

		return stream;
}

stream_t* do_struct_solicitud_memoria(paq_SolicitudMemoria* datos, int *size)
{
	stream_t *serializado = serialize_struct_solicitud_memoria(datos,size);

	return serializado;
}

paq_res_SolicitudMemoria *deserialize_struct_res_SolicitudMemoria(char* datos)
{
	paq_res_SolicitudMemoria* self = malloc(sizeof(paq_res_SolicitudMemoria));

	memcpy(&self->posicion, datos,sizeof(self->posicion));

	return self;
}

int handleHandshakeUMV(char *datos)
{
	mensaje* deserializado = undo_struct_mensaje(datos);

	printf("%s\n", deserializado->out);
	free(deserializado->out);
	free(deserializado);

	return 0;
}

stream_t *handleSolicitudMemoria(paq_SolicitudMemoria* datos, int *size)
{
	return do_struct_solicitud_memoria(datos, size);
}

int handleMemoriaSuccess(char* datos)
{
	paq_res_SolicitudMemoria *deserializado = deserialize_struct_res_SolicitudMemoria(datos);

	return deserializado->posicion;
}

int handleMemoriaFail(char* datos)
{	//TODO: ver que hacer aca cuando la UMV no da memoria.

	return 0;
}

int solicitarMemoria(t_medatada_program *metadata, u_int32_t pid, u_int32_t size, char* datos)
{
	int res;
	paq_SolicitudMemoria solicitud;
	operaciones_t resultadoSolicitudMemoria;

	printf("\n================\n pid [%d], size [%d], datos[%s]\n",pid,size,datos);
	solicitud.id_proceso = pid;
	solicitud.size = size;
	//Falta el malloc?
	solicitud.datos = datos;

	if((preparar_paquete(sockfd_cte, SOLICITAR_MEMORIA_UMV, &solicitud)) != 0) {	// Envia los datos para solicitar memoria
		perror("[Kernel-PLP]: Error al enviar solicitud de memoria a la UMV.");
		return 1;
	}

	res = analizar_paquete(sockfd_cte, bufferUMV, &resultadoSolicitudMemoria);

	if(resultadoSolicitudMemoria == MEMORIA_UMV_FAIL) {
		perror("[Kernel-PLP]: la UMV nego espacio en memoria.");
		return 1;
	}

	return res;
}

u_int32_t calcular_peso(u_int32_t cant_de_etiq, u_int32_t cant_de_func, u_int32_t cant_tot_de_lineas_de_cod)
{
	// cant_tot_de_lineas_de_cod NO TIENE RELACION DIRECTA PERO A EFECTOS PRACTICOS ES LO MISMO QUE DECIR LA LONGITUD DE SU CODIGO!!!
	return (5 * cant_de_etiq) + (3 * cant_de_func) + cant_tot_de_lineas_de_cod;
}
