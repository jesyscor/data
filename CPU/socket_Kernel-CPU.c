/*
 * socket_Kernel-CPU.c
 *
 *  Created on: 25/05/2014
 *      Author:lemmau
 */

#include "socket_Kernel-CPU.h"

// Variables para socket Cliente
#define MAXDATASIZE 256


//Socket PCP
int sockfd_cte_Kernel, numbytes_clt_Kernel;
char buf_clt_Kernel[MAXDATASIZE];
struct addrinfo hints_clt_Kernel, *servinfo_clt_Kernel, *p_clt_Kernel;
int rv_clt_Kernel;
char s_clt_Kernel[INET6_ADDRSTRLEN];


void *get_in_addr_Kernel(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int crear_cliente_PCP(const char *ip, const char* puerto)
{
	printf("[CPU]: Probando de conectar a Kernel por host [%s] y port [%s]\n", ip, puerto);

	memset(&hints_clt_Kernel, 0, sizeof hints_clt_Kernel);
	hints_clt_Kernel.ai_family = AF_INET;
	hints_clt_Kernel.ai_socktype = SOCK_STREAM;

	if ((rv_clt_Kernel = getaddrinfo(ip, puerto, &hints_clt_Kernel, &servinfo_clt_Kernel)) != 0) {
		fprintf(stderr, "[CPU] Error on 'getaddrinfo()' function: %s\n", gai_strerror(rv_clt_Kernel));
		return 1;
	}

	for(p_clt_Kernel = servinfo_clt_Kernel; p_clt_Kernel != NULL; p_clt_Kernel = p_clt_Kernel->ai_next)
	{
		if ((sockfd_cte_Kernel = socket(p_clt_Kernel->ai_family, p_clt_Kernel->ai_socktype, p_clt_Kernel->ai_protocol)) == -1)
		{
			perror("[CPU]: Error on 'socket()' function.");
			continue;
		}
		if (connect(sockfd_cte_Kernel, p_clt_Kernel->ai_addr, p_clt_Kernel->ai_addrlen) == -1)
		{
			close(sockfd_cte_Kernel);
			perror("[CPU]: Error on 'connect()' function.");
			continue;
		}
		break;
	}

	if (p_clt_Kernel == NULL)	{
		fprintf(stderr, "[CPU]: Falla en la conexion al Kernel\n");
		return 2;
	}
	inet_ntop(p_clt_Kernel->ai_family, get_in_addr_Kernel((struct sockaddr *)p_clt_Kernel->ai_addr), s_clt_Kernel, sizeof s_clt_Kernel);
	freeaddrinfo(servinfo_clt_Kernel); // all done with this structure

	return 0;
}


mensaje *deserialize_struct_mensaje(char *datos)
{
	mensaje * self= malloc(sizeof(mensaje));
	int offset = 0, tmp_size = 0;


	offset += tmp_size;
	for(tmp_size = 1; (datos + offset)[tmp_size-1] != '\0'; tmp_size++);
	self->out = malloc(tmp_size);
	memcpy(self->out, datos + offset, tmp_size);

	return self;
}

t_quantum* deserialize_struct_quantum(char* datos)
{
	t_quantum* self = malloc(sizeof(t_quantum));

	memcpy(&self->quantum, datos, sizeof(self->quantum));

	return self;
}

t_quantum* undo_struct_quantum(char* datos)
{
	t_quantum* deserializado = deserialize_struct_quantum(datos);

	printf("quantum -> [%d] \n",deserializado->quantum);
	return deserializado;
}

mensaje* undo_struct_mensaje(char* datos)
{
	mensaje* deserializado = deserialize_struct_mensaje(datos);

	return deserializado;
}

int handleQuantumRecv(char* datos)
{
	//t_quantum *quantum = undo_struct_quantum(datos);
	undo_struct_quantum(datos);

    return 0;
}

stream_t *serialize_struct_mensaje (mensaje *msg)
{
	int tamanho = strlen(msg->out) + 1;

	char *data = (char*)malloc(tamanho);
	stream_t *stream = malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	memset(data, 0, tamanho);

	offset += tmp_size;
	memcpy(data + offset, msg->out, tmp_size = strlen(msg->out) + 1);

	stream->data = data;

	return stream;
}

stream_t* do_struct_mensaje(mensaje* datos)
{
	stream_t *serializado = serialize_struct_mensaje(datos);

	return serializado;
}

stream_t* serialize_int(paq_numero* datos)
{
	int size = sizeof(paq_numero);

	stream_t *stream = malloc(sizeof(stream_t));

	stream->data = malloc(size);

	memcpy(stream->data, &datos->numero, sizeof(paq_numero));

	return stream;
}

stream_t* do_struct_int(paq_numero* datos)
{
	stream_t *serializado = serialize_int(datos);
	return serializado;
}

t_PCB* deserialize_struct_pcb(char* datos)
{
	t_PCB *pcb = malloc(sizeof(t_PCB));

	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	//Identificador
	memcpy(&pcb->identificador, datos, tmp_size = sizeof(pcb->identificador));
	offset += tmp_size;

	//Segmento de codigo
	memcpy(&pcb->segmentoDeCodigo, datos + offset, tmp_size = sizeof(pcb->segmentoDeCodigo));
	offset += tmp_size;

	//Segmento de stack
	memcpy(&pcb->segmentoDeStack, datos + offset, tmp_size = sizeof(pcb->segmentoDeStack));
	offset += tmp_size;

	//Cursor de stack
	memcpy(&pcb->cursorDeStack, datos + offset, tmp_size = sizeof(pcb->cursorDeStack));
	offset += tmp_size;

	//Indice de codigo
	memcpy(&pcb->indiceDeCodigo, datos + offset, tmp_size = sizeof(pcb->indiceDeCodigo));
	offset += tmp_size;

	//Indice de etiquetas
	memcpy(&pcb->indiceDeEtiquetas, datos + offset, tmp_size = sizeof(pcb->indiceDeEtiquetas));
	offset += tmp_size;

	//Program counter
	memcpy(&pcb->PC, datos + offset, tmp_size = sizeof(pcb->PC));
	offset += tmp_size;

	//Tamaño contexto actual
	memcpy(&pcb->tamanioCotextoActual, datos + offset, tmp_size = sizeof(pcb->tamanioCotextoActual));
	offset += tmp_size;

	//Tamaño indice de etiquetas
	memcpy(&pcb->tamanioIndiceDeEtiquetas, datos + offset, tmp_size = sizeof(pcb->tamanioIndiceDeEtiquetas));
	offset += tmp_size;

	return pcb;
}

t_PCB* undo_struct_pcb(char* datos)
{
     t_PCB* deserializado = deserialize_struct_pcb(datos);
     return deserializado;
}

int handlePCBRecv(char* datos)
{
	pcbActual = *undo_struct_pcb(datos);

	return(0);
}

int handleHandshakePCP(char* datos)
{
	mensaje* deserializado = undo_struct_mensaje(datos);

	printf("%s\n", deserializado->out);
	free(deserializado->out);
	free(deserializado);

	return 0;
}


stream_t *handlePrintNum(paq_numero* datos)
{
	return do_struct_int(datos);
}

stream_t *handlePrintText(mensaje* datos)
{
    return do_struct_mensaje(datos);
}

int conectar_a_PCP(const char *ip, const char *puerto)
{
	int res_conectar;

	res_conectar = crear_cliente_PCP(ip, puerto);

	if(res_conectar != 0) {
		perror("[CPU]: No se puede conectar al PCP.");
		return 1;
	}
	memset(buf_clt_Kernel, 0, MAXDATASIZE-1); //Limpia el buffer
	return 0;
}
