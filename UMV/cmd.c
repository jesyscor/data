/*
 * cmd.c
 *
 *  Created on: 13/04/2014
 *      Author:lemmau

 * MODELO DE CONSOLA
 *
 * OPERACIONES DISPONIBLES
 *1) > saludar
 *2) > variables
 *3) > posMP
 *4) > retardo
 *5.1) > algoritmo w
 *5.2) > algoritmo f
 *6) > compactar
 *7) > exit
 *8.1) > dump -f
 *8.2) > dump -c
 *9.1) > operacion -c proceso base offset tamaño
 *9.2) > operacion -d proceso base offset tamaño (Aca hay que corroborar. El enunciado dice que no se puede eliminar un segmento ailado)
 *9.3) > operacion -r proceso base offset tamaño
 *9.4) > operacion -w proceso base offset tamaño buffer
 *10) > !!
 *
 *1) test CMD
 *2) Muestra contenido de las vars globales
 *3) Muestra inicio de la UMV
 *4) Cambia el valor del retardo
 *5.1) Permite cambiar el algoritmo a Worst Fit
 *5.1) Permite cambiar el algoritmo a First Fit
 *6) Compacta UMV
 *7) Sale de la CMD
 *8.1) Volcado en output_dump.log de los segmentos
 *8.2) Volcado en consola de los segmentos
 *9.1) Crear Segmento (crea a partir de la base e ignora el offset?)
 *9.2) Destruir Segmento:
 *9.3) Leer Memoria
 *9.4) Escribir buffer
 *10) Permite ejecutar ULTIMA instruccion
 *
 * TODO PREGUNTAR por si la consola debe correr sobre un hilo independiente
 * TODO TERMINAR de pulir las validaciones SEG FAULT y MEM OVERLOAD
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/string.h>

#include <estructurasDeSegmentos.h>
#include <algoritmosDeUbicacion.h>
#include <commons/collections/list.h>

//Variables Globales
int RETARDO = 0;		// por default
char algoritmo = 'f';	// por default
char lastCmd[80];

//Operaciones

int operacionCrear(char *pch) {
	int i;
	Operacion oper;

	for (i = 0; i < CANTIDAD_ARGUMENTOS_OPERACION_CREAR; i++) {

		pch = strtok(NULL, " ");
		if (pch == NULL )
			return 0;

		switch (i) {
		case 0:
			oper.proceso = atoi(pch);
			break;
		case 1:
			oper.base = atoi(pch);
			break;
		case 2:
			oper.offset = atoi(pch);
			break;
		case 3:
			oper.size = atoi(pch);
			break;

		default:
			return 0;
		}
	}

	if (!(obtenerProcesoPorID(oper.proceso))) {
		perror("El proceso no existe");
		return 0;
	}

	NodoDeProceso *nodoProceso = (obtenerProcesoPorID(oper.proceso));
	NodoDeSegmento nodoSegmento;

	nodoSegmento.base = oper.base + oper.offset;
	nodoSegmento.size = oper.size;

	crearSegmento(*nodoProceso, nodoSegmento);

	return 1;
}

int operacionDestruir(char *pch) {
	int i;
	Operacion oper;

	for (i = 0; i < CANTIDAD_ARGUMENTOS_OPERACION_DESTRUIR; i++)
	{
		pch = strtok(NULL, " ");
		if (pch == NULL )
			return 0;

		switch (i) {
			case 0:
				oper.proceso = atoi(pch);
				break;
			default:
				return 0;
		}

	}
	destruirProceso(oper.proceso);
	return 1;
}

int operacionLeer(char *pch) {
	int i;
	Operacion oper;
	char* buffer;

	for (i = 0; i < CANTIDAD_ARGUMENTOS_OPERACION_LEER; i++) {

		pch = strtok(NULL, " ");
		if (pch == NULL )
			return 0;

		switch (i) {
			case 0:
				oper.proceso = atoi(pch);
				break;
			case 1:
				oper.base = atoi(pch);
				break;
			case 2:
				oper.offset = atoi(pch);
				break;
			case 3:
				oper.size = atoi(pch);
				break;
			default:
				return 0;
		}
	}

	buffer = solicitarBloque(oper.proceso, oper.base, oper.offset, oper.size);

	if (!buffer || buffer == NULL)	{
		perror("[CMD]: Segmentation Fault al Leer");
		return 0;
	}

	printf("content = [%s] \n", buffer);
	return 1;
}

int operacionEscribir(char *pch) {
	int i;
	Operacion oper;
	for (i = 0; i < CANTIDAD_ARGUMENTOS_OPERACION_ESCRIBIR; i++) {

		if (i == 4) // El tercer argumento es un cacho de texto, lo separo con ""
			pch = strtok(NULL, "\"");
		else
			pch = strtok(NULL, " ");

		if (pch == NULL )
			return 0;

		switch (i) {
		case 0:
			oper.proceso = atoi(pch);
			break;
		case 1:
			oper.base = atoi(pch);
			break;
		case 2:
			oper.offset = atoi(pch);
			break;
		case 3:
			oper.size = atoi(pch);
			break;
		case 4:
			oper.buff = pch;
			break;
		default:
			return 0;
		}

	}

	guardarBloque(oper.proceso, oper.base, oper.offset, oper.size, oper.buff);

	return 1;
}

//Recibe los comandos y los interpreta
int checkearInput(char input[40])
{
	char *pch;

	pch = strtok(input, " ");

	// Operacion DEBUG

	if (string_equals_ignore_case(pch, "saludar")) {
		printf("Hola!\n");
		return 1;
	}
	// Operacion DEBUG

	if (string_equals_ignore_case(pch, "variables")) {
		printf("\n *******************\n Variables Globales \n *******************\n");
		printf("Algoritmo de Compresion: [%c]\n", algoritmo);
		printf("retardo: [%d]\n", RETARDO);
		return 1;
	}
	// Operacion DEBUG

	if (string_equals_ignore_case(pch, "posMP")) {

		printf("%d \n", (u_int32_t) bloquePrincipal);
		return 1;
	}

	// DUMP

	if (string_equals_ignore_case(pch, "dump")) {

		pch = strtok(NULL, " -");
		if (pch == NULL )
			return 0;

			switch (pch[0]) {
				case 'f':
					output_dump = fopen("output_dump.log","a");
					listarProcesos(output_dump);
					mostrarContenido(output_dump);
					fclose(output_dump);
					break;
				default:
					listarProcesos(NULL);
					mostrarContenido(output_dump);
					return 1;
			}
	return 1;
	}
	// COMPACTACION

	if (string_equals_ignore_case(pch, "compactar")) {
		compactar();
		return 1;
	}

	// ALGORTIMO
	if (string_equals_ignore_case(pch, "algoritmo")) {

		pch = strtok(NULL, " ");
		if (pch == NULL )
			return 0;

			switch (pch[0]) {
				case 'w':
					algoritmo = 'w';
					break;
				case 'f':
					algoritmo = 'f';
					break;

				default:
					return 0;
			}
		return 1;
	}
	// RETARDO

	if (string_equals_ignore_case(pch, "retardo")) {
		pch = strtok(NULL, " ");

		if (pch == NULL )
			return 0;

		RETARDO = atoi(pch);
		return 1;
	}
	// OPERACIONES

	if (string_equals_ignore_case(pch, "operacion")) {

		pch = strtok(NULL, " -");
		if (pch == NULL )
			return 0;

		switch (pch[0]) {
		case 'c':
			if (operacionCrear(pch))
				return 1;
			break;
		case 'd':
			if (operacionDestruir(pch))
				return 1;
			break;
		case 'r':
			if (operacionLeer(pch))
				return 1;
			break;
		case 'w':
			if (operacionEscribir(pch))
				return 1;
			break;
		}
		return 0;
	}
	return 0;
}

//Se encarga de recibir los comandos y mandarlos a checkear.
int gestionarInput() {
	char c;
	char input[80];
	int char_count;

	c = getchar();
	char_count = 0;

	while ((c != '\n') && (char_count < 80)) {
		input[char_count++] = c;
		c = getchar();
	}

	input[char_count] = '\0';

	if (string_equals_ignore_case(input,"!!"))
		strcpy(input,lastCmd);

	if (input[0] == '\0' || input[0] == ' ')
		return 1;

	if (!checkearInput(input))
		printf("[UMV-CMD]: Error, entrada invalida.\n");

	strcpy(lastCmd, input);

	// EXIT
	if(!string_equals_ignore_case(input,"exit") == 0)
	{
		printf("[UMV-CMD]: CMD interactiva, cerrada\n");
		return(1);
	}
	else
	{
		return (0);
	}
}

