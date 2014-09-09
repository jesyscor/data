/*
 * main.c
 *
 *  Created on: 24/04/2014
 *      Author:lemmau
 */

#include "main.h"

int cuentaDigitos(int numero){
	if(numero < 10)
		return 1;
	else
		return 1 + cuentaDigitos(numero/10);
}

void leerConfig(arg_CPU* arg) {

	config_file = config_create("config.cfg");
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!\n");
    	log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!");
		exit(1);
	}

	//IP_KERNEL
	if (config_has_property(config_file, "IP_KERNEL")) {

		arg->ipKernel = config_get_string_value(config_file, "IP_KERNEL");

    	sprintf(bufferLog,"IP_KERNEL = [%s]", arg->ipKernel);
    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//IP_UMV
	if (config_has_property(config_file, "IP_UMV")) {

		arg->ipUMV = config_get_string_value(config_file, "IP_UMV");

		sprintf(bufferLog,"IP_UMV = [%s]",arg->ipUMV);
    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_UMV' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_UMV' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//PUERTO_UMV
	if (config_has_property(config_file, "PUERTO_UMV")) {

		arg->puertoUMV = config_get_string_value(config_file, "PUERTO_UMV");

		sprintf(bufferLog,"PUERTO_UMV = [%s]", arg->puertoUMV);
    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_UMV' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_UMV' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//PUERTO_CPU
	if (config_has_property(config_file, "PUERTO_KERNEL")) {

		arg->puertoKernel = config_get_string_value(config_file, "PUERTO_KERNEL");

    	sprintf(bufferLog,"PUERTO_KERNEL = [%s]", arg->puertoKernel);
    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
}

int analizar_paquete(u_int32_t socket, char *paquete, operaciones_t *id)
{
	int res;
	recv_msg_from(socket, (int*) id, &paquete);

	switch (*id) {
		case HANDSHAKE_CPU_SUCCESS: case HANDSHAKE_CPU_FAIL:
			res = handleHandshakePCP(paquete);
			break;
		case HANDSHAKE_UMV_SUCCESS:
			res = handshake_UMV(paquete);
			break;
		case PCB_RECV:
			res = handlePCBRecv(paquete);
			break;
		case QUANTUM_RECV:
			res = handleQuantumRecv(paquete);
			break;
		case SOLICITUD_LECTURA_RESPUESTA:
			//res = handleInstruccion(paquete);
			break;
		default:
			return -1;
			break;
	}

	if (paquete != NULL ) {	// Se libera el mensaje  cuando ya no lo necesitamos
		free(paquete);
		paquete = NULL;
	}
	return res;
}

int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura)
{
	int tamanho = 0;
	stream_t *paquete = NULL;

	switch(op)	{
	    case IMPRIMIR_TEXTO:
			paquete = handlePrintText((mensaje*)estructura);
			break;
		case IMPRIMIR_ENTERO:
			paquete = handlePrintNum((paq_numero*)estructura);
			break;
		default:
			break;
	}

	send_msg_to(socket, op, paquete->data , tamanho);

	free(paquete->data);
	free(paquete);

	return 0;
}

int conectar_todo(void* parametros) {

	conectar_a_PCP(
					((arg_CPU *) parametros)->ipKernel,
					((arg_CPU *) parametros)->puertoKernel
				);
	conectar_a_UMV(
					((arg_CPU *) parametros)->ipUMV,
					((arg_CPU *) parametros)->puertoUMV
				);
	return 0;
}

void inicializarDiccionario()	{
	dictionaryVariables = dictionary_create();
	log_info(logger,"Diccionario de vars, inicializado.");
}

int main(int argc, char **argv) {

	logger = log_create("CPU.log","CPU",false,LOG_LEVEL_TRACE);

	leerConfig(&param_CPU);
	inicializarDiccionario();

	if (conectar_todo(&param_CPU) != 0) {
		fprintf(stderr,"[CPU] Fallo la conexion a PCP y UMV. \n");
		log_error(logger,"Fallo la conexion a PCP y UMV.");
		exit(1);
	}

	analizar_paquete(sockfd_cte_Kernel, bufferCPU, &Enum);	// handshake PCP
	analizar_paquete(sockfd_cte_UMV, bufferCPU, &Enum);		// handshake UMV

	analizar_paquete(sockfd_cte_Kernel, bufferCPU, &Enum);	//quantum	(PCP)
	analizar_paquete(sockfd_cte_Kernel, bufferCPU, &Enum);	//PCB		(PCP)

	if(Enum == PCB_RECV)
	{
		printf("[CPU]: Se recibio PCB ok\n");
	}
	/* Hay que hacer algo como
	 * For Quantum
	 * instruccion = solicitar_ProximaInstruccio();
	 * ejecutar(instruccion); //Este ejecutar instrucción es abortivo, si ejecuta la instruccion END, entonces hace un break del for
	 * osea, no sigue con el quantum porque ya llego al final del programa
	 */

	// PCB
	// only for DEBUG purpose
	printf("PID -> %d \n",pcbActual.identificador);
	printf("segmDeCodigo -> %d \n",pcbActual.segmentoDeCodigo);
	printf("indiceDeCod -> %d \n",pcbActual.indiceDeCodigo);
	printf("indiceDeEtiq -> %d \n",pcbActual.indiceDeEtiquetas);
	printf("segDeStack -> %d \n",pcbActual.segmentoDeStack);
	printf("PC -> %d \n",pcbActual.PC);
	printf("tamCntxtoActual -> %d \n",pcbActual.tamanioCotextoActual);
	printf("tamIndiceEtiq -> %d \n",pcbActual.tamanioIndiceDeEtiquetas);
//	int i;
//	for (i = 0; i < quantum; i++)
//	{
//		solicitar_ProximaInstruccion();
//
//	}

	solicitar_ProximaInstruccion();
	pcbActual.PC++;
	solicitar_ProximaInstruccion();
	pcbActual.PC++;
	solicitar_ProximaInstruccion();
	pcbActual.PC++;
	solicitar_ProximaInstruccion();
	pcbActual.PC++;
	solicitar_ProximaInstruccion();
	//analizar_paquete(sockfd_cte_UMV, bufferCPU, &Enum); //recibo instruccion

	while (analizar_paquete(sockfd_cte_Kernel, bufferCPU, &Enum) == 0) {
		//TODO:evaluar condicion terminacion de while
	}

	//solicitar_ProximaInstruccion();
	log_destroy(logger);
	return 0;
}

