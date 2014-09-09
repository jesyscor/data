/* main.c
 *
 *  Created on: 14/05/2014
 *      Author:lemmau
 *
 *	CICLO DEL PROCESO PROGRAMA
 *
 * 1- Crear/Abrir file log del programa para el registro de actividades
 * 2- Obtener PID
 * 3- Leer archivo de configuracion y asignar valores
 * 4- Conectar a Kernel
 *	4.1- Analiza el primer mensaje (handshake con Kernel)
 * 	4.2 Envio del PID al Proceso Kernel
 * 5- Abrir script (pasado implicitamente como param)
 * 6- Enviar script al Kernel
 * 7- Quedar a la espera de mensajes para imprimir
 *
 * TODO Set ANSISOP_CONFIG via .sh
 * TODO Set soft link /Debug/Programa /usr/bin/interprete_ansisop via .sh
 * TODO Set soft link MiniGral_lib.so /usr/lib/MiniGral_lib.so via .sh
 * TODO Mostrar por consola de PROGRAMA excepciones de la UMV en caso de no contar con memoria suficiente
 * TODO CHECKEAR "LINEA EN BLANCO" de los programas ANSISOP (puede que deba de hacerlo en el kernel con ayuda del parser)
 * TODO REVISAR path del archivo de config porque DEPENDE desde DONDE se lo ejecute al script anisop
 * TODO REGISTRAR actividades con LOG FILE !!!
 * TODO VER necesidad de pasar por param LOG_LEVELss (LOG_LEVEL_INFO, LOG_LEVEL_ERROR, etc)
 */

#include "main.h"

void *get_in_addr_cliente(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int crear_socket_cliente (const char *ip, const char *puerto)
{
	printf("[Programa]: Probando de conectar a Kernel por host [%s] y port [%s]\n", ip, puerto);
	sprintf(bufferLog,"Probando de conectar a Kernel por host [%s] y port [%s]", ip, puerto);
	log_debug(logger,bufferLog);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, puerto, &hints, &servinfo)) != 0)	{
		fprintf(stderr, "[Programa]: Error on 'getaddrinfo' function -> %s\n", gai_strerror(rv));
		sprintf(bufferLog, "Error on 'getaddrinfo' function -> %s\n", gai_strerror(rv));
		log_debug(logger,bufferLog);

		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)	{
			perror("[Programa]: Error on 'socket()' function.");
			log_error(logger,"Error on 'socket()' function.");

			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)	{
			close(sockfd);
			perror("[Programa]: Error on 'connect()' function.");
			log_error(logger,"Error on 'connect()' function.");
			continue;
		}
		break;
	}

	if (p == NULL)	{
		fprintf(stderr, "[Programa]: Falla en la conexion al Kernel\n");
		return 1;
	}
	inet_ntop(p->ai_family, get_in_addr_cliente((struct sockaddr *)p->ai_addr), s, sizeof s);
	freeaddrinfo(servinfo);

	return 0;
}

void cerrar_socket_cliente() {
	close(sockfd);
}

// to RECIEVE

mensaje *deserialize_struct_mensaje(char *datos) {
	mensaje * self= malloc(sizeof(mensaje));
	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	for(tmp_size = 1; (datos + offset)[tmp_size-1] != '\0'; tmp_size++);
	self->out = malloc(tmp_size);
	memcpy(self->out, datos + offset, tmp_size);

	return self;
}

paq_numero *deserialize_struct_int(char *datos) {
	paq_numero *self = malloc(sizeof(paq_numero));

	memcpy(&self->numero, datos, sizeof(self->numero));

	return self;
}


void undo_struct_mensaje(char* datos) {
	mensaje *deserializado = deserialize_struct_mensaje(datos);

	printf("%s\n", deserializado->out);
	free(deserializado->out);
	free(deserializado);
}

void undo_struct_int(char* datos) {
    paq_numero *deserializado = deserialize_struct_int(datos);

    printf("[%d] /n", deserializado->numero);
    //free(deserializado->numero);
    free(deserializado);
}

int handleHandshake(char *datos) {
	undo_struct_mensaje(datos);
	return 0;
}

int handlePIDResult(char* datos) {
	undo_struct_mensaje(datos);
	return 0;
}

int handlePrintMsg(char *datos) {
	undo_struct_mensaje(datos);
	return 0;
}

int handlePrintInt(char *datos) {
	undo_struct_int(datos);
	return 0;
}

int handleEndProgramm(char *datos) {
	undo_struct_mensaje(datos);
	return 0;
}
// --------------------------------------

// to SEND

stream_t *serialize_struct_mensaje (mensaje *msg, int *tamanho) {
	*tamanho = 	strlen(msg->out) + 1;

	char *data = (char*)malloc(*tamanho);
	stream_t *stream = malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	memcpy(data + offset, msg->out, tmp_size = strlen(msg->out) + 1);

	stream->data = data;

	return stream;
}

stream_t *serialize_struct_pid(pid_structura *datos, int* size) {
	*size = sizeof(datos->pid);

	char *data = (char*)malloc(*size);
	stream_t *stream = malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	memcpy(data + offset, &datos->pid, tmp_size = sizeof(datos->pid));

	stream->data = data;

	return stream;
}

stream_t* do_struct_mensaje(mensaje* datos, int* size) {
	stream_t *serializado = serialize_struct_mensaje(datos, size);

	return serializado;
}

stream_t *do_struct_pid(pid_structura *datos, int* size) {
	stream_t *serializado = serialize_struct_pid(datos, size);

	return serializado;
}

stream_t *handleScriptLine(mensaje* datos, int* size) {
	return do_struct_mensaje(datos, size);
}

stream_t *handlePID(pid_structura *datos, int* size) {
	return do_struct_pid(datos,size);
}

int handleScriptResult(char *datos) {
	undo_struct_mensaje(datos);

	return 0;
}
// --------------------------------------


int analizar_paquete(char *paquete, operaciones_t *op)
{
	int res; // Resultado de cada handler

	recv_msg_from(sockfd, (int*)op, &paquete);
//printf("id op [%d]\n",*op);	// for debug purpose
	switch(*op)
	{
		case HANDSHAKE_SUCCESS: case HANDSHAKE_FAIL:
//printf("HANDSHAKE_SUCCESS o HANDSHAKE_FAIL\n");
			log_trace(logger, "Respuesta Handshake, recibida.");
			res = handleHandshake(paquete);
			break;
		case SCRIPT_SUCCESS: case SCRIPT_FAIL:
//printf("SCRIPT_SUCCESS o SCRIPT_FAIL\n");
			log_trace(logger, "Kernel recibio envio de SCRIPT.");
			res = handleScriptResult(paquete);
			break;
		case PID_SUCCESS: case PID_FAIL:
//printf("PID_SUCCESS o PID_FAIL\n");
			log_trace(logger, "Kernel recibio envio de PID.");
			res = handlePIDResult(paquete);
			break;
		case ENVIAR_IMPRIMIR_TEXTO:
printf("ENVIAR_IMPRIMIR_TEXTO\n");
			log_trace(logger, "Solicitud de Impresion, recibida.");
			res = handlePrintMsg(paquete);;
			break;
		case ENVIAR_IMPRIMIR_ENTERO:
printf("ENVIAR_IMPRIMIR_ENTERO\n");
			log_trace(logger, "Solicitud de Impresion, recibida.");
			res = handlePrintInt(paquete);
			break;
		case END_PROGRAMM:
			log_trace(logger, "Respuesta de Fin de programa, recibida.");
			res = handleEndProgramm(paquete);
printf("END_PROGRAMM\n");
			break;
		default:
//printf("default\n");
			log_error(logger, "Kernel se cerro inesperadamente. Abortando ejecucion de script");
			exit(EXIT_FAILURE);
			break;
	}

	// Se libera el mensaje  cuando ya no lo necesitamos
	if (paquete != NULL)	{
		free (paquete);
		paquete = NULL;
	}
	return res;
}

int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura)
{
	int tamanho = 0;
	stream_t *paquete = NULL;
//printf("id op [%d]\n",op);	// for debug purpose
	switch(op)
	{
		case SCRIPT_LINE: case SCRIPT_EOF:
//printf("SCRIPT_LINE o SCRIPT_EOF\n");
			paquete = handleScriptLine((mensaje*)estructura, &tamanho);
			break;
		case PID:
			log_trace(logger, "Enviando PID al Kernel.");
//printf("PID\n");
			paquete = handlePID((pid_structura*)estructura, &tamanho);
			break;
		default:
//printf("default\n");
			return -1;
			break;
	}

	send_msg_to(socket, op, paquete->data , tamanho);

	free(paquete->data);
	free(paquete);

	return 0;
}

void conectar_a_Kernel(char *ip, char *puerto)
{
	if(crear_socket_cliente(ip,puerto))
	{
		fprintf(stderr,"[Programa]: No se pudo contactar al Kernel. Programa abortado.\n");
		log_error(logger,"No se pudo conectar al Kernel. Programa abortado.");
		exit(1);
	}
}

void leerConfig()
{
    config_file = config_create("config.cfg");
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!\n");
    	log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!");
		exit(1);
	}

    //IP_KERNEL
    if(config_has_property(config_file, "IP_KERNEL"))
    {
    	ip_kernel = config_get_string_value(config_file, "IP_KERNEL");
    	sprintf(bufferLog,"IP_KERNEL = [%s]",ip_kernel);
    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
    //PUERTO_KERNEL
    if(config_has_property(config_file, "PUERTO_KERNEL"))
    {
    	puerto_kernel = config_get_string_value(config_file, "PUERTO_KERNEL");
    	sprintf(bufferLog,"PUERTO_KERNEL = [%s]",puerto_kernel);
    	log_debug(logger,bufferLog);
    } else {
		fprintf(stderr, "Falta key 'PUERTO_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
}

void liberarMemoria()
{
	config_destroy(config_file);
	log_destroy(logger);
	cerrar_socket_cliente();
	free(buffer);
	exit(1);
}

int main(int argc, char	**argv){
	/*TODO Crear PID para Programa y mandarselo tambien al PLP. Despues del Handshake.
	 * El PLP lo recibe, y lo agrega en una lista auxiliar junto con el Socket por el cual se envian los mensajes.
	 */
	buffer = NULL;
	sizeOfFile = 0;
	script_name = argv[1];

	// Control de argumentos
	if(argc == 1 || argc > 2)
	{
		printf("[Error] Usage:\n> ./Programa script.ansisop\n > ./script.ansisop\n");
		exit(1);
	} else {
		esAnsisop = string_ends_with(script_name,".ansisop");
		if(!esAnsisop)
		{
			printf("[Error]: Tu script no es un autentico ansisop\n");
			exit(1);
		}
	}

	//1- Crear/Abrir FILE LOG del programa para el registro de actividades
	logger = log_create("programa.log", "Programa", false, LOG_LEVEL_TRACE);
	log_info(logger,"Inicio de registro de actividades del Proceso PROGRAMA.");

	//2- Obtener PID
	pid.pid = process_getpid();	// returns the process ID of the calling process. These functions are always successful.
	sprintf(bufferLog,"Nuevo Proceso con PID = [%d]",pid.pid);
	log_debug(logger,bufferLog);

	//3- Leer archivo de configuracion
	leerConfig();

	//4- Conectar al Kernel
	conectar_a_Kernel(ip_kernel, puerto_kernel);

	//4.1- Analiza el primer mensaje (handshake con Kernel)
	analizar_paquete(buffer, &idOperacion);	//Espera respuesta Handshake
	if(idOperacion == HANDSHAKE_FAIL) {
		printf("[Program]: Ocurrio un problema en el proceso Handshake. Programa abortado.\n");
		log_error(logger," Ocurrio un problema en el proceso Handshake. Programa abortado.");
		liberarMemoria();
		exit(1);
	}
	printf("[Programa]: Conexion a Kernel establecida.\n");
	log_info(logger,"Conexion a Kernel establecida.");

	//-4.2 Envio de PID al Proceso Kernel
	preparar_paquete(sockfd, PID, &pid);

	analizar_paquete(buffer, &idOperacion);
	if(idOperacion == PID_FAIL) {
		printf("[Programa]: Ocurrio un problema en el envio del PID. Programa abortado.\n");
		log_error(logger,"Ocurrio un problema en el envio del PID. Programa abortado.");
		liberarMemoria();
		exit(1);
	}
	//5- Abrir script
	script = fopen(script_name,"r");
	log_trace(logger,"Abriendo script para enviarlo al Kernel.");

	if (!script || script == NULL)	{
		printf("[Programa]: No se pudo abrir el script. Programa abortado.");
		log_error(logger,"No se pudo abrir el script. Programa abortado.");
		liberarMemoria();
		exit(1);
	}

	fseek(script, 0L, SEEK_END);
	sizeOfFile = ftell(script) + 1;
	fseek(script, 0L, SEEK_SET);

	scriptBody = (char*) malloc(sizeOfFile);

	bytesReadScript = fread(scriptBody,1,sizeOfFile,script);
	scriptBody[bytesReadScript] = 0;	// Add terminating zero.

	//6- Enviar script al Kernel
	if((bytesReadScript != sizeOfFile) && !feof(script)) {
		printf("[Programa]: Error al leer el script. Programa abortado.");
	    log_error(logger,"Error al leer el script. Programa abortado.");
		liberarMemoria();
		exit(1);
	}

	msg.out = scriptBody;
	preparar_paquete(sockfd, SCRIPT_LINE, &msg);
	log_trace(logger,"Se envio script al Kernel.");
	free(scriptBody);

	msg.out = "";
	preparar_paquete(sockfd, SCRIPT_EOF, &msg);
	fclose(script);

	analizar_paquete(buffer, &idOperacion); //Se queda a la espera de confirmacion de recepcion del Script completo.
											//Y la posible negativa a la solicitud de memoria para el programa
											//por parte del PLP.
	if(idOperacion == SCRIPT_FAIL)	{
		perror("[Programa]: Ocurrio un problema en la recepcion del Script. Programa abortado.");
		log_error(logger,"Ocurrio un problema en la recepcion del Script. Programa abortado.");
		liberarMemoria();
		exit(1);
	} else
	if(idOperacion == END_PROGRAMM)	{
		perror("[Programa]: Ocurrio un problema en la solicitud de memoria a la UMV. Programa abortado.");
		log_error(logger,"Ocurrio un problema en la recepcion del Script. Programa abortado.");
		liberarMemoria();
		exit(1);
	}
	log_trace(logger,"El script fue recibido por el Kernel con exito.");

	//7- Quedar a la espera de mensajes para imprimir
	while(analizar_paquete(buffer, &idOperacion) == 0)
	{
		if(idOperacion == END_PROGRAMM) {
			printf("[Programa]: Cerrando Programa...\n");
			sprintf(bufferLog,"Cerrando Programa con PID = [%d]",pid.pid);
			log_trace(logger,"Cerrando Programa");
			break;
		}
	}

	// Destruimos la estructura config, cerramos socket y liberamos memoria
	liberarMemoria();

	return 0;
}
