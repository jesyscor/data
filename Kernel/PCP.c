/*
 * PCP.c
 *
 *  Created on: 04/05/2014
 *      Author:lemmau
 */

#include "PCP.h"

	// Variables para sockets Servidor
	fd_set master_PCP; // master file descriptor list
	fd_set read_fds_PCP; // temp file descriptor list for select()
	int fdmax_PCP;// maximum file descriptor number
	int listener_PCP; // listening socket descriptor
	int newfd_PCP; // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr_PCP; // client address
	socklen_t addrlen_PCP;
	char buf_PCP[MAXDATASIZE]; // buffer for client data
	int nbytes_PCP;
	int yes_PCP = 1;
	char remoteIP_PCP[INET6_ADDRSTRLEN];
	int i_PCP, j_PCP, rv_PCP;
	struct addrinfo hints_PCP, *ai_PCP, *p_PCP;

void *get_in_addr_PCP(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int crear_listener_PCP(const char *ip, const char *puerto)
{
	FD_ZERO(&master_PCP); // clear the master and temp sets
	FD_ZERO(&read_fds_PCP);

	memset(&hints_PCP, 0, sizeof(hints_PCP));// get us a socket and bind it
	hints_PCP.ai_family = AF_INET;
	hints_PCP.ai_socktype = SOCK_STREAM;

	if ((rv_PCP = getaddrinfo(ip, puerto, &hints_PCP, &ai_PCP)) != 0) {
		fprintf(stderr, "[Kernel-PCP]: Error on 'getaddrinfo()' function : [%s]\n", gai_strerror(rv_PCP));
		exit(1);
	}

	for(p_PCP = ai_PCP; p_PCP != NULL; p_PCP = p_PCP->ai_next)
	{
		listener_PCP = socket(p_PCP->ai_family, p_PCP->ai_socktype, p_PCP->ai_protocol);

		// lose the pesky "address already in use" error message
		setsockopt(listener_PCP, SOL_SOCKET, SO_REUSEADDR, &yes_PCP, sizeof(int));
		if (bind(listener_PCP, p_PCP->ai_addr, p_PCP->ai_addrlen) < 0) {
			close(listener_PCP);
			continue;
		}
		break;
	}

	// if we got here, it means we didn't get bound
	if (p_PCP == NULL)	{
		fprintf(stderr, "[Kernel-PCP]: Error on 'selectserver': failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai_PCP); // all done with this

	if (listen(listener_PCP, 10) == -1) {
		perror("[Kernel-PCP]: Error on 'listen()' function.");
		exit(3);
	}
	// add the listener to the master set
	FD_SET(listener_PCP, &master_PCP);
	// keep track of the biggest file descriptor
	fdmax_PCP = listener_PCP; // so far, it's this one

	printf("[Kernel-PCP]: Hilo a la espera de CPUs...\n");
	return listener_PCP;
}

/*void *dict_CPU_element_destroyer (CPU_dict_value *data)
{
	free(data);
	return 0;
}*/

int analizar_paquete_PCP(u_int32_t socket, char *paquete, operaciones_PCP *id)
{
	int res; // Resultado de cada handler
	recv_msg_from(socket, (int*)id, &paquete);

	switch(*id)
	{
		case PID_CPU:
			res = handlePIDCPU(paquete, socket);
			break;
		case IO:
			res = handleIO(paquete);
			break;
		case IMPRIMIR_TEXTO:
			res = handlePrintText(paquete, socket);
			break;
		case IMPRIMIR_ENTERO:
			res = handlePrintNum(paquete, socket);
			break;
		case END_PROGRAMM_PCP:
			res = handleEndProgrammPCP(paquete);
			break;
		case QUANTUM_FINISHED:
			res = handleQuantumFinished(paquete);
			break;
		default:
			return -1;
			break;
	}

	// Se libera el mensaje  cuando ya no lo necesitamos
	if (paquete != NULL)	{
		free (paquete);
		paquete = NULL;
	}

	return res;
}

int preparar_paquete_PCP(u_int32_t socket, operaciones_PCP op, void* estructura)
{
	int tamanho = 0;
	stream_t *paquete = NULL;

	switch(op)
	{
		case HANDSHAKE_CPU_SUCCESS: case HANDSHAKE_CPU_FAIL:
			paquete = handleHandshakePCP((mensaje*)estructura, &tamanho);
			break;
		case PCB_SEND:
			paquete = handlePCBSend((t_PCB*)estructura, &tamanho);
			break;
		case QUANTUM_SEND:
			paquete = handleQuantumSend((t_quantum*)estructura, &tamanho);
			break;
		default:
			break;
	}

	send_msg_to(socket, op, paquete->data , tamanho);

	free(paquete->data);
	free(paquete);

	return 0;
}

int atenderCPU(u_int32_t socket)
{
	/*	PASOS de handlePID
	* 1- Recibir PID. (handle pid, agregar pid al diccionario junto con el fdsock (sacar del PLP handle pid))
	* 2- Enviar respuesta de recepcion del PID
	* 3- Enviar Quantum
	* 4- Recibir confirmacion de recepcion de quantum
	* 5-
	*/

//	msg_PCP.out = "[Kernel-PCP]: Handshake con CPU, exitoso!";	// sale por consola CPU
//	preparar_paquete_PCP(newfd_PCP, HANDSHAKE_CPU_SUCCESS, &msg_PCP);
//	preparar_paquete_PCP(newfd_PCP, QUANTUM_SEND, &((arg_PCP *)parametros)->quantum);
//
	operaciones_PCP operacion;
	analizar_paquete_PCP(socket, bufferCPU, &operacion);

	return 0;
}

void* main_PCP(void* parametros)
{
	char* ipKernel = ((arg_PCP *)parametros)->ipKernel;
	char* puertoCPU = ((arg_PCP *)parametros)->puertoCPU;
	logger = ((arg_PCP *)parametros)->ptrLogger;

	log_trace(logger,"INICIO de registro de actividades del Hilo PCP.");

	int select_res;
	struct timeval timeout; //Timeout del Select
	timeout.tv_sec = DELAY_CHECK_READY_SEC;
	timeout.tv_usec = DELAY_CHECK_READY_USEC;

	// Creacion de estructuras
	cola_exec = queue_create();
	cola_block = queue_create();
	listaDeCPUs = list_create();

	log_info(logger,"Se crean las estructuras necesarias del hilo.");

	crear_listener_PCP(ipKernel, puertoCPU);

//char buf[256];
//int nbytes,j;
	for(;;)
	{
//		printf("chequeando...\n");
		read_fds_PCP = master_PCP; // copy it (copiamos el master al read_fds)
		while((select_res = select(fdmax_PCP+1, &read_fds_PCP, NULL, NULL, &timeout)) <= 0)
		{
			if (select_res == -1) {
				perror("[Kernel-PCP]: Error on 'select()' function.");
				exit(4);
			} else if(select_res == 0) {	//Se fija si hay un PCB disponible en la cola de Ready y lo pasa a Exec

//				t_PCB* pcb = malloc(sizeof(t_PCB));
//				pcb->identificador = 2;
//				pcb->segmentoDeCodigo = 2;
//				pcb->segmentoDeStack = 2;
//				pcb->indiceDeCodigo = 2;
//				pcb->indiceDeEtiquetas = 2;
//				pcb->PC = 2;
//				pcb->cursorDeStack = 0;			// lo seteo en cero, desp sera modificado
//				pcb->tamanioCotextoActual = 0;	// se inicializa en cero segun ayudante
//				pcb->tamanioIndiceDeEtiquetas = 2;
//				pcb->peso = 2;
//
//				encolar_in_ready(pcb);
				check_ready_exec();
			}
			timeout.tv_sec = DELAY_CHECK_READY_SEC;
			timeout.tv_usec = DELAY_CHECK_READY_USEC;
			read_fds_PCP = master_PCP;
		}
		// run through the existing connections looking for data to read
		for(i_PCP = 0; i_PCP <= fdmax_PCP; i_PCP++)
		{
			if (FD_ISSET(i_PCP, &read_fds_PCP))
			{ // we got one!!
				if (i_PCP == listener_PCP)
				{// handle new connections
					addrlen_PCP = sizeof(remoteaddr_PCP);
					newfd_PCP = accept(listener_PCP, (struct sockaddr *)&remoteaddr_PCP, &addrlen_PCP);
					if (newfd_PCP == -1)
					{
						perror("[Kernel-PCP]: Error on 'accept()' function.");
					}
					else
					{
						FD_SET(newfd_PCP, &master_PCP); // add to master set
						if (newfd_PCP > fdmax_PCP)	{	// keep track of the max
							fdmax_PCP = newfd_PCP;
						}
						printf("[Kernel-PCP]: Conexion establecida con CPU por host [%s] y port [%s]\n",
								inet_ntop(remoteaddr_PCP.ss_family,
										get_in_addr_PCP((struct sockaddr*)&remoteaddr_PCP),
										remoteIP_PCP, INET6_ADDRSTRLEN),
										((arg_PCP *)parametros)->puertoCPU);	// sale por consola KERNEL
						msg_PCP.out = "[Kernel-PCP]: Handshake con CPU, exitoso!";	// sale por consola CPU
						preparar_paquete_PCP(newfd_PCP, HANDSHAKE_CPU_SUCCESS, &msg_PCP);
						preparar_paquete_PCP(newfd_PCP, QUANTUM_SEND, &((arg_PCP *)parametros)->quantum);
						addCPUtoList(newfd_PCP);
					}
				}
				else
				{
				//	printf("chequeando x3...\n");
					atenderCPU(i_PCP);
//					// gestionar datos de un cte
//					if((nbytes = recv(i_PCP,buf,sizeof(buf),0)) < 0) {
//						// error o conexion cerrada por el cte
//						printf("chequeando x2...\n");
//						if(nbytes == 0) {
//							// conexion cerrada
//							printf("selectserver: socket %d hung up\n",i_PCP);
//						} else	{
//							perror("recv");
//						}
//						close(i_PCP);	// hasta luego
//						FD_CLR(i_PCP, &master_PCP);
//					} else {
//						// tenemos datos de algun cte
//printf("chequeando x3...\n");
//atenderCPU(i_PCP);
//						for(j = 0; j <= fdmax_PCP; j++)
//						{	// enviar a todoo el mundo
//							if(FD_ISSET(j, &master_PCP)){
//								// excepto al listener y a nosotros mismos
//								if(send(j,buf,nbytes,0) == -1){
//									perror("send");
//								}
//							}
//
//						}
//					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

	log_trace(logger,"FIN de registro de actividades del Hilo PCP.");
	return(0);
}
