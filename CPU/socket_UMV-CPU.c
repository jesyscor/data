/*
 * socket_UMV-CPU.c
 *
 *  Created on: 25/05/2014
 *      Author:lemmau
 */

#include "socket_UMV-CPU.h"

//Socket UMV
int sockfd_cte_UMV, numbytes_clt_UMV;
char buf_clt_UMV[MAXDATASIZE];
struct addrinfo hints_clt_UMV, *servinfo_clt_UMV, *p_clt_UMV;
int rv_clt_UMV;
char s_clt_UMV[INET6_ADDRSTRLEN];


void *get_in_addr_UMV(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int crear_cliente_UMV(const char *ip, const char* puerto)
{
	printf("[CPU]: Probando de conectar a UMV por host [%s] y port [%s]\n", ip, puerto);

	memset(&hints_clt_UMV, 0, sizeof hints_clt_UMV);
	hints_clt_UMV.ai_family = AF_INET;
	hints_clt_UMV.ai_socktype = SOCK_STREAM;

	if ((rv_clt_UMV = getaddrinfo(ip, puerto, &hints_clt_UMV, &servinfo_clt_UMV)) != 0)
	{
		fprintf(stderr, "[CPU]: Error on 'getaddrinfo()' function : %s\n", gai_strerror(rv_clt_UMV));
		return 1;
	}

	for(p_clt_UMV = servinfo_clt_UMV; p_clt_UMV != NULL; p_clt_UMV = p_clt_UMV->ai_next)
	{
		if ((sockfd_cte_UMV = socket(p_clt_UMV->ai_family, p_clt_UMV->ai_socktype, p_clt_UMV->ai_protocol)) == -1)
		{
			perror("[CPU]: Error on 'socket()' function.");
			continue;
		}
		if (connect(sockfd_cte_UMV, p_clt_UMV->ai_addr, p_clt_UMV->ai_addrlen) == -1)
		{
			close(sockfd_cte_UMV);
			perror("[CPU]: Error on 'connect()' function.");
			continue;
		}
		break;
	}

	if (p_clt_UMV == NULL)
	{
		fprintf(stderr, "[CPU]: Falla en la conexion a la UMV\n");
		return 2;
	}
	inet_ntop(p_clt_UMV->ai_family, get_in_addr_UMV((struct sockaddr *)p_clt_UMV->ai_addr), s_clt_UMV, sizeof s_clt_UMV);

	freeaddrinfo(servinfo_clt_UMV); // all done with this structure

	return 0;
}

int conectar_a_UMV(const char *ip, const char *puerto)
{
	int res_conectar;
	res_conectar = crear_cliente_UMV(ip, puerto);

	if(res_conectar != 0)	{
		perror("[CPU]: Error. No se puede conectar a la UMV.");
		return 1;
	}
	memset(buf_clt_UMV, 0, MAXDATASIZE-1); //Limpia el buffer
	return 0;
}

stream_t *serialize_struct_sol_Lectura (u_int32_t base, u_int32_t offset, u_int32_t size)
{
	paq_sol_LeerMemoria solicitud;

	stream_t *stream = malloc(sizeof(stream_t));
	stream->data = malloc(sizeof(paq_sol_LeerMemoria));

	solicitud.id_proceso = pcbActual.identificador;
	solicitud.base = base;
	solicitud.offset = offset;
	solicitud.size = size;

	memcpy(stream->data, &solicitud, sizeof(paq_sol_LeerMemoria));

	return stream;

}

paq_res_LeerMemoria *deserialize_struct_res_SolicitudLectura(char* datos)
{
	paq_res_LeerMemoria* self = malloc(sizeof(paq_res_LeerMemoria));


	memcpy(&self->size,datos,sizeof(u_int32_t));

	self->datos = (char*)malloc(self->size);

	memcpy(self->datos, datos + sizeof(u_int32_t), self->size);

	return self;
}


stream_t *serialize_struct_sol_Escritura (u_int32_t base, u_int32_t offset, u_int32_t size, char* datos, u_int32_t *sizeTotal)
{
	paq_sol_EscribirMemoria solicitud;

	stream_t *stream = malloc(sizeof(stream_t));

	solicitud.id_proceso = pcbActual.identificador;
	solicitud.base = base;
	solicitud.offset = offset;
	solicitud.size = size;

	*sizeTotal = sizeof(paq_sol_EscribirMemoria) + size;

	stream->data = malloc(*sizeTotal);

	memcpy(stream->data, &solicitud, sizeof(paq_sol_EscribirMemoria));
	memcpy(stream->data + sizeof(paq_sol_EscribirMemoria), datos, solicitud.size);

	return stream;

}

paq_res_EscribirMemoria *deserialize_struct_res_SolicitudEscritura(char* datos)
{
	paq_res_EscribirMemoria* self = malloc(sizeof(paq_res_EscribirMemoria));

	memcpy(&self->posicion,datos,sizeof(u_int32_t));

	return self;
}

int ejecutarInstruccion (char* instruccion)
{
	analizadorLinea(instruccion,&functions,&kernel_functions);
	return 0;
}


t_instruction solicitar_IndiceCodigo(u_int32_t numeroInstruccion)
{
	int id;
	char * paquete;
	stream_t * serializado;
	t_instruction indiceCodigo;
	int size = sizeof(paq_sol_LeerMemoria);
printf("solicitar_indicecodigo() \n");
	serializado = serialize_struct_sol_Lectura(	pcbActual.indiceDeCodigo,
												numeroInstruccion * sizeof(t_instruction),
												sizeof(t_instruction));
	send_msg_to(sockfd_cte_UMV, SOLICITUD_LECTURA_ENVIADA, serializado->data, size);
	//Espera respuesta
	recv_msg_from(sockfd_cte_UMV, &id, &paquete);

	memcpy(&indiceCodigo, deserialize_struct_res_SolicitudLectura(paquete)->datos, sizeof(t_instruction));

	return indiceCodigo;
}


void solicitar_ProximaInstruccion()
{
	int id;
	char* paquete;
	char* instruccion;
	paq_res_LeerMemoria *instruccion_data;
	stream_t * serializado;
	int tamanho = sizeof(paq_sol_LeerMemoria);

	t_instruction indiceInstruccion = solicitar_IndiceCodigo(pcbActual.PC);
printf("solicitar_proximainstruccion() \n");
	serializado = serialize_struct_sol_Lectura(pcbActual.segmentoDeCodigo, indiceInstruccion.start, indiceInstruccion.offset);
	send_msg_to(sockfd_cte_UMV, SOLICITUD_LECTURA_ENVIADA, serializado->data, tamanho);
	//Espera respuesta
	recv_msg_from(sockfd_cte_UMV, &id, &paquete);
	instruccion_data = deserialize_struct_res_SolicitudLectura(paquete);

	//Agrego el final de linea
	instruccion = malloc(instruccion_data->size + 1);
	memcpy(instruccion,instruccion_data->datos,instruccion_data->size);
	instruccion[instruccion_data->size] = '\0';

	ejecutarInstruccion(instruccion);

	free(instruccion_data);
	free(instruccion);
}

int handshake_UMV(char *mensaje)
{
	printf("%s", mensaje);
	printf("[CPU]: Conexion a UMV establecida.\n");
	return 0;
}
