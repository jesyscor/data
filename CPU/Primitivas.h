/*
 * Primitivas.h
 *
 *  Created on: 24/04/2014
 *      Author:lemmau
 */

#ifndef PRIMITIVAS_H_
#define PRIMITIVAS_H_

	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <signal.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <errno.h>
	#include <unistd.h>
	#include <string.h>
	#include <pthread.h>

	#include <commons/config.h>
	#include <commons/txt.h>
	#include <parser/parser.h>
	#include <parser/metadata_program.h>

	#include "Enviar_Recibir_Paquete.h"
	#include "socket_Kernel-CPU.h"
	#include "socket_UMV-CPU.h"

	extern int sockfd_cte_UMV;
	extern int sockfd_cte_Kernel;
	extern t_dictionary *dictionaryVariables;

	typedef struct t_Diccionario_Variables {	// Estructura para pasar argumentos al hilo CPU
		char* variable;
		u_int32_t posicion;
	} t_dic_variables;

	int ejecutarInstruccion (char* instruccion);
	void escribeMensaje (int socket, int idMensaje, char *mensaje, int tamanho);
	int cuentaDigitos(int numero);

	extern int preparar_paquete(u_int32_t socket, operaciones_t op, void* estructura);

	AnSISOP_funciones functions;
	AnSISOP_kernel kernel_functions;

	t_puntero definirVariable(t_nombre_variable identificador_variable);
	t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable);
	t_valor_variable dereferenciar(t_puntero direccion_variable);
	void asignar(t_puntero direccion_variable, t_valor_variable valor);
	t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);
	t_valor_variable asignarValorCompartida(t_nombre_compartida t_valor_variable);

//	t_puntero_instruccion irAlLabel(t_nombre_etiqueta etiqueta);
//	t_puntero_instruccion llamarSinRetorno(t_nombre_etiqueta etiqueta, t_puntero_instruccion linea_en_ejecucion);
//	t_puntero_instruccion llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion);
//	t_puntero_instruccion finalizar(void);
//	t_puntero_instruccion retornar(t_valor_variable retorno);
//	int imprimir(t_valor_variable valor_mostrar);
//	int imprimirTexto(char* texto);
//	int entradaSalida(t_nombre_dispositivo dispositivo, int tiempo);
//
//	int wait(t_nombre_semaforo identificador_semaforo);
//	int signal(t_nombre_semaforo identificador_semaforo);

//	AnSISOP_funciones functions = {
//			.AnSISOP_definirVariable = definirVariable,
//			.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,
//			.AnSISOP_dereferenciar = dereferenciar,
//			.AnSISOP_asignar = asignar,
//	//		.AnSISOP_irAlLabel              = irAlLabel,
//	//		.AnSISOP_llamarSinRetorno       = llamarSinRetorno,
//	//		.AnSISOP_llamarConRetorno       = llamarConRetorno,
//	//		.AnSISOP_finalizar              = finalizar,
//	//		.AnSISOP_retornar               = retornar,
//	//		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
//	//		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
//	//		.AnSISOP_imprimir 				= imprimir,
//	//		.AnSISOP_imprimirTexto			= imprimirTexto
//	//		.AnSISOP_entradaSalida          = entrarSalida,
//
//	};
//
//	AnSISOP_kernel kernel_functions = {
//	//		.AnSISOP_wait                   = waitAnsisop,
//	//		.AnSISOP_signal                 = signalAnsisop,
//	};


#endif /* PRIMITIVAS_H_ */
