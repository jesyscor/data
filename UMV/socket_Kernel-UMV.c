/*
 * socket_Kernel-UMV.c
 *
 *  Created on: 03/05/2014
 *      Author:lemmau
 */

#include <socket_Kernel-UMV.h>

// Variables para sockets

fd_set master;	// master file descriptor list
fd_set read_fds;// temp file descriptor list for select()
int fdmax;		// maximum file descriptor number
int listener;	// listening socket descriptor
int newfd;		// newly accept()ed socket descriptor
struct sockaddr_storage remoteaddr;	// client address
socklen_t addrlen;
char buf[MAXDATASIZE];	// buffer for client data
int nbytes;
int yes = 1;
char remoteIP[INET6_ADDRSTRLEN];
int i, j, rv;
struct addrinfo hints, *ai, *p;

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}


stream_t *serialize_struct_handshake (paq_Handshake *hns, int *size)
{
	*size = 	strlen(hns->data)+1;
	stream_t *stream = malloc(*size);
	stream->data = hns->data;

	return stream;
}

stream_t *serialize_struct_SolicitudMemoria (paq_res_SolicitudMemoria *bytesAsignados, int *size)
{
	*size = sizeof(paq_res_SolicitudMemoria);
	stream_t *stream = malloc(sizeof(stream_t));
	stream->data = malloc(*size);

	memcpy(stream->data, bytesAsignados, *size);

	return stream;
}

int handShakeConKernel(int fdSock) {

	paq_Handshake hns;
	stream_t * serializado;
	int size;

	hns.data = "[UMV]: Handshake con Kernel, exitoso!";

	serializado = serialize_struct_handshake(&hns, &size);

	send_msg_to(fdSock, HANDSHAKE_SUCCESS, serializado->data, size);
	return 0;
}

int enviarRespuestaSolicitudMemoria(int fdSock, paq_res_SolicitudMemoria rsm)
{
	int size;
	stream_t * serializado;

	serializado = serialize_struct_SolicitudMemoria(&rsm, &size);
	send_msg_to(fdSock,SOLICITUD_MEMORIA_RESPUESTA, serializado->data, size);

	return 0;
}

paq_SolicitudMemoria deserialize_struct_SolicitudMemoria(char* paquete)
{
	int offset = 0;

	paq_SolicitudMemoria paq_Recibido;// = malloc(sizeof(paq_SolicitudMemoria));

	//Recibo id_proceso
	memcpy(&paq_Recibido.id_proceso, paquete, sizeof(u_int32_t));
	offset += sizeof(u_int32_t);

	//Recibo tamaño del segmento
	memcpy(&paq_Recibido.size, paquete + offset,
			sizeof(u_int32_t));
	offset += sizeof(u_int32_t);

	//Recibo los datos del segmento

	paq_Recibido.datos = malloc(paq_Recibido.size);

	memcpy(paq_Recibido.datos, paquete + offset,
			paq_Recibido.size);

	return paq_Recibido;
}


u_int32_t crearSegmentoPorSolicitud(paq_SolicitudMemoria datos)
{
	NodoDeSegmento nuevoSegmento;
	nuevoSegmento.size = datos.size;

	if (datos.size <= 0)
		return -2;

	nuevoSegmento = crearSegmento(*obtenerProcesoPorID(datos.id_proceso), nuevoSegmento);
//printf("datossss -> [%s] \n",datos.datos);

	guardarBloque(datos.id_proceso, nuevoSegmento.base, 0, nuevoSegmento.size, datos.datos);

	return nuevoSegmento.base;
}

paq_res_SolicitudMemoria guardarSolicitudDeMemoria(paq_SolicitudMemoria datosAAlmacenar)
{
	paq_res_SolicitudMemoria bytesAsignados;

	//Si no existe, creo el proceso
	if (!obtenerProcesoPorID(datosAAlmacenar.id_proceso))
		agregarProceso(datosAAlmacenar.id_proceso);

printf("proceso = [%d], size = [%d], datos = [%s]\n",datosAAlmacenar.id_proceso, datosAAlmacenar.size,datosAAlmacenar.datos);
	/* Para acceder se respeta el orden:
	 * 0-Code Segment
	 * 1-Indice Codigo
	 * 2-Indice Etiquetas
	 * 3-Stack Segment
	 * En total hay 4 segmentos por programa
	 */

	// La UMV le responde al PLP con la direccion virtual (ficticia)
	// Una vez recibida, el PLP la guarda en el PCB correspondiente
	if ((bytesAsignados.posicion = crearSegmentoPorSolicitud(datosAAlmacenar)) == -1)	{
		perror ("No hay memoria suficiente para el segmento"); //TODO intentar compactar...
	//	return bytesAsignados;
	}
	//TODO: MUY IMPORTANTE, LA UMV EXPLOTA CUANDO NO LE DA EL ESPACIO EN LUGAR DE DEVOLVER -1, HAY QUE HACER ESTA COMPROBACION AL CREAR EL SEGMENTO!
	// Y EL -1 lo tiene que devolver en la posición de memoria si no hay espacio
	return bytesAsignados;
}

int handleSolicitudMemoria(char *paquete, operaciones_t *id, int sockfd)
{
	paq_SolicitudMemoria datosAAlmacenar;
	paq_res_SolicitudMemoria bytesAsignados;

	datosAAlmacenar = deserialize_struct_SolicitudMemoria(paquete);

	bytesAsignados = guardarSolicitudDeMemoria(datosAAlmacenar);

	enviarRespuestaSolicitudMemoria(sockfd, bytesAsignados);

	//Enviar mensaje ID OPERACION

	return 0;
}

int handleEndProgramm(char *datos, operaciones_t *id)
{
	return 0;
}

int handleDefault(char *datos, operaciones_t *id)
{
	return 0;
}


int analizar_paquete(int sockfd)
{
	int res; 		// Resultado de cada handler
	char* paquete;
	operaciones_t *id = malloc(sizeof(operaciones_t));

	recv_msg_from(sockfd, (int*)id, &paquete);

	switch(*id)
	{
		case SOLICITUD_MEMORIA_RECIBIDA:
			res = handleSolicitudMemoria(paquete, id, sockfd);
			break;
		case END_PROGRAM:
			res = handleEndProgramm(paquete, id);
			break;
		default:
			res = handleDefault(paquete, id);
			break;
	}

	if (paquete != NULL) {	// Se libera el mensaje  cuando ya no lo necesitamos
		free (paquete);
		paquete = NULL;
	}
	return res;
}


int crear_listener(const char *ip, const char *puerto) {

	FD_ZERO(&master); // clear the master and temp sets
	FD_ZERO(&read_fds);

	memset(&hints, 0, sizeof(hints)); // get us a socket and bind it
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, puerto, &hints, &ai)) != 0) {
		fprintf(stderr, "[UMV]: Error on selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

	for (p = ai; p != NULL ; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}
		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL ) {
		fprintf(stderr, "[UMV]: Error on selectserver: Failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, 10) == -1) {
		perror("[UMV]: Error on 'listen()' function.");
		exit(3);
	}
	// add the listener to the master set
	FD_SET(listener, &master);
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	printf("[UMV]: Hilo a la espera del Kernel...\n");
	return listener;
}

void *inicializar_SocketKernelUMV() {

	crear_listener(ip_UMV, puerto_Kernel);

	for (;;) {
		read_fds = master; // copy it (copiamos el master al read_fds)
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL ) == -1) //Ver los otros fd_set, tal vez los necesite.
		{
			perror("[UMV]: Error on 'select()' function.");
			exit(4);
		}
		// run through the existing connections looking for data to read
		for (i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{ // we got one!!
				if (i == listener)
				{ // handle new connections
					addrlen = sizeof(remoteaddr);
					newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen );
					if (newfd == -1)
					{
						perror("[UMV]: Error on 'accept()' function dentro de select.");
					}
					else
					{
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) { // keep track of the max
							fdmax = newfd;
						}
						printf("[UMV]: Conexion establecida con Kernel por host [%s] y port [%s]\n",
								inet_ntop(
										remoteaddr.ss_family,
										get_in_addr((struct sockaddr*) &remoteaddr),
										remoteIP, INET6_ADDRSTRLEN
										),
										puerto_Kernel);
						handShakeConKernel(newfd);
					}
				} else {
					/* Proximas conexiones (desp del handshake) provenientes del Kernel
					 * seran analizadas (para su deserializacion) PERO siempre teniendo
					 * en cuenta un RETARDO en milisegundos (puede cambiarse desde la CMD).
					 */
					usleep(RETARDO * 1000);
					analizar_paquete(i);
					//handle data from a client
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

	return 0;

}

void cerrar_SocketKernelUMV() {
	close(listener);
}
