/*
 * PLP.c
 *
 *  Created on: 04/05/2014
 *      Author:lemmau
 */

#include "PLP.h"

// Variables para hilos
pthread_t thread_UMV, thread_servidor_programa;
int ret_UMV, ret_servidor_programa;

// Variables para sockets Servidor
fd_set master;		// master file descriptor list
fd_set read_fds;	// temp file descriptor list for select()
int fdmax;			// maximum file descriptor number
int listener;		// listening socket descriptor
int newfd;			// newly accept()ed socket descriptor
struct sockaddr_storage remoteaddr;	// client address
socklen_t addrlen;
int nbytes;
int yes=1;
char remoteIP[INET6_ADDRSTRLEN];
int i, j, rv;
struct addrinfo hints, *ai, *p;

// Variables para socket Cliente UMV
int sockfd_cte, numbytes_clt;
struct addrinfo hints_clt, *servinfo_clt, *p_clt;
int rv_clt;
char s_clt[INET6_ADDRSTRLEN];


void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int crear_listener(const char *ip, const char *puerto)
{
	FD_ZERO(&master);	// clear the master and temp sets
	FD_ZERO(&read_fds);

	memset(&hints, 0, sizeof(hints));	// get us a socket and bind it
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, puerto, &hints, &ai)) != 0)	{
		fprintf(stderr, "[Kernel-PLP] Error on selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

	for(p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)	{
			close(listener);
			continue;
		}
		break;
	}

	if (p == NULL)	{	// if we got here, it means we didn't get bound
		fprintf(stderr, "[Kernel-PLP] Error on selectserver: Failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai);	// all done with this

	if (listen(listener, 10) == -1)	{
		perror("[Kernel-PLP]: Error on 'listen()' function.");
		exit(3);
	}
	// add the listener to the master set
	FD_SET(listener, &master);
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	printf("[Kernel-PLP]: Hilo a la espera de Programas...\n");
	return listener;
}

int crear_cliente_UMV(const char *ip, const char* puerto)
{
	printf("[Kernel-PLP]: Probando de conectar a UMV por host [%s] y port [%s]\n", ip, puerto);

	memset(&hints_clt, 0, sizeof hints_clt);
	hints_clt.ai_family = AF_INET;
	hints_clt.ai_socktype = SOCK_STREAM;

	if ((rv_clt = getaddrinfo(ip, puerto, &hints_clt, &servinfo_clt)) != 0)	{
		fprintf(stderr, "[Kernel-PLP]: Error on 'getaddrinfo_cliente' -> %s\n", gai_strerror(rv_clt));
		return 1;
	}

	for(p_clt = servinfo_clt; p_clt != NULL; p_clt = p_clt->ai_next)
	{
		if ((sockfd_cte = socket(p_clt->ai_family, p_clt->ai_socktype, p_clt->ai_protocol)) == -1)	{
			perror("[Kernel-PLP]: Error on 'socket()' function");
			continue;
		}
		if (connect(sockfd_cte, p_clt->ai_addr, p_clt->ai_addrlen) == -1)	{
			close(sockfd_cte);
			perror("[Kernel-PLP]: Error on 'connect()' function");
			continue;
		}
		break;
	}

	if (p_clt == NULL)	{
		fprintf(stderr, "[Kernel-PLP]: Falla en la conexion a la UMV\n");
		return 2;
	}
	inet_ntop(p_clt->ai_family, get_in_addr((struct sockaddr *)p_clt->ai_addr), s_clt, sizeof s_clt);
	freeaddrinfo(servinfo_clt); // all done with this structure

	return 0;
}

void conectar_a_UMV(const char *ip, const char *puerto)
{
	operaciones_t operacion;

	if(bufferUMV != NULL)	{
		free(bufferUMV);
		bufferUMV = NULL;
	}

	if(crear_cliente_UMV(ip, puerto) != 0)	{
		perror("[Kernel-PLP]: No se puede conectar a la UMV. Abortando.");
		exit(1);
	}

	analizar_paquete(sockfd_cte, bufferUMV, &operacion); //Espera respuesta Handshake

	if(operacion == HANDSHAKE_UMV_FAIL)	{
		perror("[Kernel-PLP]: Handshake con UMV, sin exito!");
		log_error(logger, "Handshake con UMV, sin exito!");
		close(sockfd_cte);
		exit(1);
	}

	printf("[Kernel-PLP]: Conexion a UMV establecida.\n");
	log_info(logger, "Conexion a UMV establecida.");
}

void *dict_Programa_element_destroyer (u_int32_t *data)
{
	free(data);
	return 0;
}

int recibir_PID(u_int32_t socket)
{
	mensaje msg;
	int pid_programa;
	operaciones_t operacion;

	pid_programa = analizar_paquete(socket, bufferPRG, &operacion);

	if(pid_programa < 0) {
		log_error(logger,"Fallo en la recepcion del PID. Se cierra la conexion.");
		return -1;
	}

//	printf("[Kernel-PLP] PID....[%d]\n",pid_programa);	// only for debug purpose
	msg.out = "[Kernel-PLP]: PID recibido correctamente.";
	log_info(logger,"PID recibido correctamente.");
	preparar_paquete(socket, PID_SUCCESS, &msg);

	return pid_programa;
}

int recibir_script(u_int32_t socket)
{
	mensaje msg;
	operaciones_t operacion;

	while (analizar_paquete(socket, bufferPRG, &operacion) == 0) // Ciclo para recibir script
	{
		if (operacion == SCRIPT_EOF) {
			break;
		}
	}

	msg.out = "[Kernel-PLP]: Script recibido correctamente.";
	log_info(logger,"Script recibido correctamente.");
	preparar_paquete(socket, SCRIPT_SUCCESS, &msg);

	return 0;
}

t_PCB *crear_PCB(t_medatada_program *mtd, u_int32_t pid, t_PCB *mem_result)
{
	/*
	 * Guardo en el PCB la dir logica (ficticia), asi lo abstraemos de los posibles movimientos de los segmentos durante la compactación.
	 *
	 * Entonces para el Kernel y el CPU el segmento de código siempre esta en la dirección 536 (dirección lógica que esta en el PCB),
	 * sin importar que la dirección real, que maneja la UMV, era cuando se creo la 348, luego de una compactación paso a estar en la
	 * posición real 215 y luego por otra compactación se movió al 128.
	 *
	 * NOTA: Al proceso UMV JAMAS le paso el PCB
	 */
	t_PCB *pcb = (t_PCB*)malloc(sizeof(t_PCB));

	pcb->identificador = pid;
	pcb->segmentoDeCodigo = mem_result->segmentoDeCodigo;
	pcb->segmentoDeStack = mem_result->segmentoDeStack;
	pcb->indiceDeCodigo = mem_result->indiceDeCodigo;
	pcb->indiceDeEtiquetas = mem_result->indiceDeEtiquetas;
	pcb->PC = mtd->instruccion_inicio;
	pcb->cursorDeStack = 0;			// lo seteo en cero, desp sera modificado
	pcb->tamanioCotextoActual = 0;	// se inicializa en cero segun ayudante
	pcb->tamanioIndiceDeEtiquetas = mtd->etiquetas_size;
	pcb->peso = mem_result->peso;
	return pcb;
}

int atender_Programa(u_int32_t socket,u_int32_t gradoMultiProg, u_int32_t tamanio_stack)
{
	/*	PASOS
	* 1- Recibir PID.
	* 2- Recibir script
	* 3- Pre-procesar script
	* 4- Solicitar memoria a la UMV
	* 5- Crear PCB
	* 6- Encolar proceso
	*
	*	Los free MSG.OUT estan de más, segun valgrind !!
	*/

	mensaje msg;
	char *scriptCpy; //Al preprocesar el script lo modifica, por eso hago una copia.
	u_int32_t pid_Programa;

	// 1- Recibir PID

	pid_Programa = recibir_PID(socket);
	if(pid_Programa < 0) {
		msg.out = "[Kernel-PLP]: Fallo en la recepcion del PID. Se cierra la conexion.";
		preparar_paquete(socket, PID_FAIL, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		if(dictionary_has_key(dictionaryProgramas, (char*)&pid_Programa))
			dictionary_remove_and_destroy(dictionaryProgramas, (char*)&pid_Programa, (void *)dict_Programa_element_destroyer);
		return 1;
	}

	// 2- Recibir script

	if(recibir_script(socket) != 0) {
		msg.out = "[Kernel-PLP]: Fallo en la recepcion del script. Se cierra la conexion.";
		preparar_paquete(socket, SCRIPT_FAIL, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		if(dictionary_has_key(dictionaryProgramas, (char*)&pid_Programa))
			dictionary_remove_and_destroy(dictionaryProgramas, (char*)&pid_Programa, (void *)dict_Programa_element_destroyer);
		return 1;
	}

	// 3- Pre-procesar script

	scriptCpy = string_duplicate(script);	//TODO:	Algo cambia en "script" al preprocesarlo, por eso hago una copia. REVISAR.
	u_int32_t longitud_script = strlen(scriptCpy);
	t_medatada_program *metadata = pre_procesar_script(scriptCpy);

	// 4- Solicitar memoria a la UMV para segmentos
	printf("==========solicitarMemoria=========\n");

		// CODE segment
	if((info_mem_UMV.segmentoDeCodigo = solicitarMemoria(metadata, pid_Programa, longitud_script, scriptCpy)) < 0)	{
		msg.out = "[Kernel-PLP]: Fallo en la solicitud de memoria a la UMV. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAMM, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		if(dictionary_has_key(dictionaryProgramas, (char*)&pid_Programa))
			dictionary_remove_and_destroy(dictionaryProgramas, (char*)&pid_Programa, (void *)dict_Programa_element_destroyer);
		return 2;
	}
		// CODE index
	if((info_mem_UMV.indiceDeCodigo = solicitarMemoria(metadata, pid_Programa,
			metadata->instrucciones_size * sizeof(t_intructions), (char*)metadata->instrucciones_serializado)) < 0)	{
		msg.out = "[Kernel-PLP]: Fallo en la solicitud de memoria a la UMV. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAMM, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		if(dictionary_has_key(dictionaryProgramas, (char*)&pid_Programa))
			dictionary_remove_and_destroy(dictionaryProgramas, (char*)&pid_Programa, (void *)dict_Programa_element_destroyer);
		return 2;
	}
		// LABEL index
	if((info_mem_UMV.indiceDeEtiquetas = solicitarMemoria (metadata, pid_Programa,
														metadata->etiquetas_size, metadata->etiquetas)) < 0) {
		msg.out = "[Kernel-PLP]: Fallo en la solicitud de memoria a la UMV. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAMM, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		if(dictionary_has_key(dictionaryProgramas, (char*)&pid_Programa))
			dictionary_remove_and_destroy(dictionaryProgramas, (char*)&pid_Programa, (void *)dict_Programa_element_destroyer);
		return 2;
	}
		// STACK segment
	if((info_mem_UMV.segmentoDeStack = solicitarMemoria(metadata, pid_Programa, tamanio_stack, "")) < 0) {
		msg.out = "[Kernel-PLP]: Fallo en la solicitud de memoria a la UMV. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAMM, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		if(dictionary_has_key(dictionaryProgramas, (char*)&pid_Programa))
			dictionary_remove_and_destroy(dictionaryProgramas, (char*)&pid_Programa, (void *)dict_Programa_element_destroyer);
		return 2;
	}

	//5- Calcular PESO del proceso y este campo al PCB
/*
 *	NOTA : El 3er argumento (cant_tot_de_lineas_de_cod)
 *	NO TIENE RELACION DIRECTA PERO A EFECTOS PRACTICOS ES LO MISMO QUE DECIR LA LONGITUD DE SU CODIGO!!!
*/
	info_mem_UMV.peso = calcular_peso(metadata->cantidad_de_etiquetas, metadata->cantidad_de_funciones, longitud_script);

	// 6- Crear PCB e Init del PC
	t_PCB *pcb = crear_PCB(metadata, pid_Programa, &info_mem_UMV);

	// only for DEBUG purpose
	printf("cantidad_total_de_lineas_de_codigo: %d\n",longitud_script);
	printf("peso_proc: %d \n", pcb->peso);

	// PCB
	// only for DEBUG purpose
	printf("==========PID=========\n");
	printf("PID -> %d \n",pcb->identificador);
	printf("[segmDeCodigo] -> %d \n",pcb->segmentoDeCodigo);
	printf("[indiceDeCod] -> %d \n",pcb->indiceDeCodigo);
	printf("[indiceDeEtiq] -> %d \n",pcb->indiceDeEtiquetas);
	printf("[segDeStack] -> %d \n",pcb->segmentoDeStack);
	printf("PC -> %d \n",pcb->PC);
	printf("tamCntxtoActual -> %d \n",pcb->tamanioCotextoActual);
	printf("tamIndiceEtiq -> %d \n",pcb->tamanioIndiceDeEtiquetas);

	// METADATA
	// only for DEBUG purpose
	printf("==========METADATA=========\n");
	printf("inst_inicio -> %d \n",metadata->instruccion_inicio);
	printf("tam_instrucciones -> %d \n",metadata->instrucciones_size);
	printf("instruc_serializado -> %s \n",(char*)metadata->instrucciones_serializado);
	printf("tam_etiquetas -> %d \n",metadata->etiquetas_size);
	printf("etiquetas -> %s \n",(char*)metadata->etiquetas);
	printf("cant de func -> %d \n",metadata->cantidad_de_funciones);
	printf("cant de etiq -> %d \n",metadata->cantidad_de_etiquetas);

	// 7- Encolar proceso (segun SJN)
	encolar(NEW, pcb);

	return 0;
}

stream_t *serialize_struct_mensaje (mensaje *msg, int *tamanho)
{
	*tamanho = 	strlen(msg->out) + 1;

	char *data = (char*)malloc(*tamanho);
	stream_t *stream = malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	memset(data, 0, *tamanho);

	offset += tmp_size;
	memcpy(data + offset, msg->out, tmp_size = strlen(msg->out) + 1);

	stream->data = data;

	return stream;
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

t_medatada_program *pre_procesar_script(const char * script)
{
	return metadata_desde_literal(script);
}

stream_t* do_struct_mensaje(mensaje* datos, int* size)
{
	stream_t *serializado = serialize_struct_mensaje(datos, size);

	return serializado;
}

mensaje* undo_struct_mensaje(char* datos)
{
	mensaje *deserializado = deserialize_struct_mensaje(datos);

	return deserializado;
}

int analizar_paquete(u_int32_t socket, char *paquete, operaciones_t *op)
{
	int res; // Resultado de cada handler

	recv_msg_from(socket, (int*)op, &paquete);

	switch(*op) {
		case PID:
			res = handlePID(paquete, socket);
			break;
		case SCRIPT_LINE:
			res = handleScriptLine(paquete);
			break;
		case SCRIPT_EOF:
			res = handleScriptEOF(paquete);
			break;
		case HANDSHAKE_UMV_SUCCESS: case HANDSHAKE_UMV_FAIL:
			res = handleHandshakeUMV(paquete);
			break;
		case MEMORIA_UMV_SUCCESS:
			res = handleMemoriaSuccess(paquete);
			return res;
			break;
		case MEMORIA_UMV_FAIL:
			res = handleMemoriaFail(paquete);
			break;
		default:
			return -1;
			break;
	}

	// Se libera el mensaje  cuando ya no lo necesitamos
	if (paquete != NULL) {
		free (paquete);
		paquete = NULL;
	}
	return res;
}

int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura)
{
	int tamanho = 0;
	stream_t *paquete = NULL;

	switch(op)
	{
		case HANDSHAKE_SUCCESS: case HANDSHAKE_FAIL:
			paquete = handleHandshake((mensaje*)estructura, &tamanho);
			break;
		case PID_SUCCESS: case PID_FAIL:
			paquete = handlePIDResult((mensaje*)estructura, &tamanho);
			break;
		case SCRIPT_SUCCESS: case SCRIPT_FAIL:
			paquete = handleScriptResult((mensaje*)estructura, &tamanho);
			break;
		case END_PROGRAMM:
			paquete = handleEndProgramm((mensaje*)estructura, &tamanho);
			break;
		case SOLICITAR_MEMORIA_UMV:
			paquete = handleSolicitudMemoria((paq_SolicitudMemoria*)estructura, &tamanho);
			break;
		default:
			break;
	}

	send_msg_to(socket, op, paquete->data , tamanho);

	free(paquete->data);
	free(paquete);

	return 0;
}

void* main_PLP(void* parametros)
{
/* TODO el grado de multiprogramacion no se maneja por contador, el PLP siempre acepta programas.
 * El PLP se encarga de tener en cuenta el grado de multiprogramacion, al tomar los PCB de New a Ready.
 * TODO los programas nuevos se rechazan SOLAMENTE cuando falla un intento de reservarle un segmento de memoria.
 * TODO ver asunto destruccion de segmentos
 */

	/*	CICLO
	*
	* 1- CONECTAR A UMV
	* 2- PONER SOCKETS A LA ESCUCHA DE PROGRAMAS
	* 3- ATENDER PROGRAMA
	* 3.1- Recibir PID
	* 3.2- Recibir script
	* 3.3- Pre-procesar script
	* 3.4- Solicitar memoria a la UMV
	* 3.5- Crear PCB e Init del PC
	* 3.6- Encolar proceso (SJN)
	*/

	// parametros recibidos del Kernel
	logger = ((arg_PLP *)parametros)->ptrLogger;
	ip_kernel = ((arg_PLP *)parametros)->ipKernel;
	puerto_prog = ((arg_PLP *)parametros)->puertoProg;
	ip_umv = ((arg_PLP *)parametros)->ipUMV;
	puerto_umv = ((arg_PLP *)parametros)->puertoUMV;
	gradoMultiProg = ((arg_PLP *)parametros)->multiprog;
	tamanio_stack = ((arg_PLP *)parametros)->tamanioStack;


	log_trace(logger,"INICIO de registro de actividades del Hilo PLP.");

	// creacion de estructuras
	cola_new = list_create();
	cola_ready = queue_create();
	cola_exit = queue_create();
	dictionaryProgramas = dictionary_create();

	log_info(logger,"Se crean las estructuras necesarias del hilo.");

	int select_result;
	struct timeval tv; //Timeout del Select
	tv.tv_sec = DELAY_CHECK_NEW_READY_SEC;
	tv.tv_usec = DELAY_CHECK_NEW_READY_USEC;

	//1- Conectarse a UMV
	conectar_a_UMV(ip_umv, puerto_umv);

	//2- Socket de escucha para Programas
	crear_listener(ip_kernel, puerto_prog);

//	t_PCB* elemento = queue_pop(cola_ready);
//					printf("plp->");
//					printf("id [%d],peso [%d],segcod [%d] cursordestack [%d],indicedecodigo [%d],indicedeetiq [%d],segmentodestack [%d],tam contexto actual [%d], tam ind etiq [%d]\n",
//							elemento->identificador,elemento->peso,elemento->segmentoDeCodigo,
//							elemento->cursorDeStack,elemento->indiceDeCodigo,elemento->indiceDeEtiquetas,elemento->segmentoDeStack,
//							elemento->tamanioCotextoActual,elemento->tamanioIndiceDeEtiquetas);

	for(;;)
	{
		read_fds = master; // copy it (copiamos el master al read_fds)
		while((select_result = select(fdmax+1, &read_fds, NULL, NULL, &tv)) <= 0)
		{
			if (select_result == -1) {
				perror("[Kernel-PLP]: Error on 'select()' function.");
				exit(4);
			} else
				if(select_result == 0) {
//printf("chequeando...\n");
				check_new_ready(gradoMultiProg);	//Se fija si puede pasar PCB de la cola New a Ready.
				check_exit();				//Se fija si hay algun programa para terminar en la cola de Exit.
			}
			tv.tv_sec = DELAY_CHECK_NEW_READY_SEC;
			tv.tv_usec = DELAY_CHECK_NEW_READY_USEC;
			read_fds = master;
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{	// we got one!!
				if (i == listener)
				{	// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
					if (newfd == -1) {
						perror("[Kernel-PLP]: Error on 'accept()' function (dentro de select)");
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax)
						{	// keep track of the max
							fdmax = newfd;
						}
						printf("[Kernel-PLP]: Nuevo programa en host [%s] y socket [%d]\n",
								inet_ntop(
										remoteaddr.ss_family,
										get_in_addr((struct sockaddr*)&remoteaddr),
										remoteIP, INET6_ADDRSTRLEN
										),
										newfd);
						msg.out = "[Kernel-PLP] Handshake con Programa, exitoso!";
						preparar_paquete(newfd, HANDSHAKE_SUCCESS, &msg);
					}
				}
				else
				{	// handle data from a client
					atender_Programa(i,gradoMultiProg,tamanio_stack);
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

	log_trace(logger,"FIN de registro de actividades del Hilo PLP.");
	return 0;
}
