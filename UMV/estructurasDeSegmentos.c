/*
 * estructurasDeSegmentos.c
 *  Created on: 18/04/2014
 *      Author:lemmau
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <algoritmosDeUbicacion.h>
#include <estructurasDeSegmentos.h>

//Variables Externas
extern char algoritmo;
extern char *bloquePrincipal;
extern int sizeBloquePrincipal;

t_list *listaDeProcesos;
t_list *listaAuxiliar;

//Constructores de Nodos. **PRIVADO**, Se usan internamente

NodoDeProceso nuevo_NodoDeProceso(int id_proceso) {
	NodoDeProceso nuevoProceso;
	nuevoProceso.id_proceso = id_proceso;
	return nuevoProceso;
}

NodoDeSegmento nuevo_NodoDeSegmento(int base, int size, u_int32_t posicionMP) {
	NodoDeSegmento nuevoSegmento;
	nuevoSegmento.base = base;
	nuevoSegmento.size = size;
	nuevoSegmento.posicionMP = posicionMP;
	return nuevoSegmento;
}

//Lista Auxiliar de segmentos, ordenada por posicionMP
void setearNodoMaxyMin(t_list *listaAuxiliar) {
	NodoDeSegmentoAuxiliar *nodoMinimo = malloc(sizeof(NodoDeSegmentoAuxiliar));
	NodoDeSegmentoAuxiliar *nodoMaximo = malloc(sizeof(NodoDeSegmentoAuxiliar));
	nodoMinimo->posicionMP = (u_int32_t) bloquePrincipal;
	nodoMinimo->size = 0;

	nodoMaximo->posicionMP = (u_int32_t) bloquePrincipal + sizeBloquePrincipal;
	nodoMaximo->size = 0;

	list_add(listaAuxiliar, nodoMinimo);
	list_add(listaAuxiliar, nodoMaximo);
}

void crear_ListaAuxiliar() {
	listaAuxiliar = list_create();

	//Ademas, agrego los nodos minimo y maximo de la memoria principal.
	setearNodoMaxyMin(listaAuxiliar);
}

// -----------------------------
// --- Funciones de PROCESOS ---
// -----------------------------

//Crea la lista de procesos se crea por única vez al inicializar la UMV
void crear_ListaDeProcesos() {
	listaDeProcesos = list_create();
}

//Dado un PID en la Lista de Procesos, devuelve el puntero del nodo en esa posicion. **PRIVADO**
NodoDeProceso *obtenerProceso(int numeroDeProceso) {
	return ((NodoDeProceso*) list_get(listaDeProcesos, numeroDeProceso));
}

//Dado el ID, devuelve el puntero del nodo solicitado
NodoDeProceso *obtenerProcesoPorID(int idAEncontrar) {
	int i;
	for (i = 0; i < list_size(listaDeProcesos); i++) {
		NodoDeProceso *nodo = obtenerProceso(i);
		if (nodo->id_proceso == idAEncontrar) {
			return ((NodoDeProceso*) list_get(listaDeProcesos, i));
		}
	}
	return 0;
}

//Crea la lista de segmentos **PRIVADO**, Se usa internamente
void crear_ListaDeSegmentos(int id_proceso) {
	obtenerProcesoPorID(id_proceso)->tablaDeSegmentos = list_create();
}

//Agrega un nuevo proceso con el nombre id_proceso a la Lista de Procesos
void agregarProceso(int id_proceso) {
	NodoDeProceso *NodoAAgregar = malloc(sizeof(NodoDeProceso));
	*NodoAAgregar = nuevo_NodoDeProceso(id_proceso);
	list_add(listaDeProcesos, NodoAAgregar);
	crear_ListaDeSegmentos(id_proceso);
}

//Dado el ID, devuelve la posicion del Proceso en la Lista de Procesos
int buscarProcesoPorID(int idAEncontrar) {
	int i;
	for (i = 0; i <= list_size(listaDeProcesos); i++) {
		NodoDeProceso *nodo = obtenerProceso(i);
		if (nodo->id_proceso == idAEncontrar) {
			return (i);
		}
	}
	return 0;
}

//Dado el ID, destruye todos los segmentos de un proceso
void destruirProceso(int id_procesoADestruir) {
	list_remove(listaDeProcesos, buscarProcesoPorID(id_procesoADestruir));
}

// -------------------------------
// --- Funciones de SEGMENTOS ---
// -------------------------------

//Agrega un segmento al proceso id_proceso. Necesita los datos del segmento.
void agregarSegmento(int id_proceso, int base, int size, u_int32_t posicionMP) {
	NodoDeSegmento *nodoAAgregar = malloc(sizeof(NodoDeSegmento));
	*nodoAAgregar = nuevo_NodoDeSegmento(base, size, posicionMP);

	list_add(obtenerProcesoPorID(id_proceso)->tablaDeSegmentos, nodoAAgregar);
}

//Dado un proceso y la posicion del segmento en la lista (coincide con el numero de segmento), devuelve un puntero al segmento solicitado
NodoDeSegmento *obtenerSegmento(int id_proceso, int numeroDeSegmento) {
	return ((NodoDeSegmento*) list_get(
			obtenerProcesoPorID(id_proceso)->tablaDeSegmentos, numeroDeSegmento));
}

//Dado un proceso y una base retorna el segmento que se encuentra en dicha posicion
NodoDeSegmento *obtenerSegmentoPorBase(int id_proceso, int base) {
	int i;
	NodoDeProceso *proceso;
	if ((proceso = obtenerProcesoPorID(id_proceso)) == 0)
		return 0;

	for (i = 0;
			i < list_size(proceso->tablaDeSegmentos);
			i++)
	{
		if (obtenerSegmento(id_proceso, i)->base == base)
			return obtenerSegmento(id_proceso, i);
	}
	return 0;
}

int obtenerPosicionSegmentoPorBase(int id_proceso, int base) {
	int i;
	for (i = 0;
			i < list_size(obtenerProcesoPorID(id_proceso)->tablaDeSegmentos);
			i++)
	{
		if (obtenerSegmento(id_proceso, i)->base == base)
			return i;
	}
	return 0;
}


//Dada una posición en la lista auxilar retorna el segmento que se encuentra en esa posicion
NodoDeSegmentoAuxiliar *obtenerSegmentoAuxiliar(int numeroDeSegmento) {
	return ((NodoDeSegmentoAuxiliar*) list_get(listaAuxiliar, numeroDeSegmento));
}


//Dada una base retorna el segmento correspondiente en la lista auxiliar
NodoDeSegmentoAuxiliar *obtenerSegmentoPorBaseAuxiliar(int base) {
	int i;
	for (i = 0;
			i < list_size(listaAuxiliar);
			i++)
	{
		if (obtenerSegmentoAuxiliar(i)->base == base)
			return obtenerSegmentoAuxiliar(i);
	}
	return 0;
}

int obtenerPosicionSegmentoPorBaseAuxiliar(int base){
	int i;
		for (i = 0;
				i < list_size(listaAuxiliar);
				i++) {

			if (obtenerSegmentoAuxiliar(i)->base == base)
				return i;

		}

		return 0; //TODO Este y el de posicion de segmento por base tienen que retornar -1 para atajar el error

}

//Crea el segmento en la memoria de la UMV
NodoDeSegmento crearSegmento(NodoDeProceso datosDelProceso,	NodoDeSegmento datosDelSegmento) {
	if (algoritmo == 'f')
		return crearSegmento_firstFit(datosDelProceso, datosDelSegmento);
	else
		return crearSegmento_worstFit(datosDelProceso, datosDelSegmento);
}

// --------------------------
// --- Funciones de DEBUG ---
// --------------------------

void mostrarContenido(int output_dump)
{
	char* buffer;
	int i;
	NodoDeProceso nodo;
	NodoDeSegmento subnodo;

	for (i = 0; i < list_size(listaDeProcesos); i++)
	{
		nodo = *(NodoDeProceso*) list_get(listaDeProcesos, i);

		for (i = 0;
				i < list_size((obtenerProcesoPorID(nodo.id_proceso))->tablaDeSegmentos);
				i++)
		{
			subnodo = *(NodoDeSegmento*) list_get(
					obtenerProcesoPorID(nodo.id_proceso)->tablaDeSegmentos, i);

			buffer = solicitarBloque(nodo.id_proceso, subnodo.base, 0, subnodo.size);

			if (!buffer || buffer == NULL)	{
				perror("[CMD]: Segmentation Fault al Leer");
				exit(1);
			}

			printf("contenido = [%s] \n", buffer);
		}
	}
}

//Dado el ID, lista todos los segmentos de un proceso
void listarSegmentos( output_dump, id_proceso) {
	int i;
	NodoDeSegmento nodo;

	if(output_dump) {	// segun parametro -f (se graba en output_dump.LOG)
		fprintf((FILE*)output_dump,
			"Segm nro | Base \t| Tamanio \t| PosicionMP    |\n-------------------------------------------------\n");
		}
	else {	// se imprime por pantalla de la consola
		printf("Segm nro | Base \t| Tamanio \t| PosicionMP    |\n---------------------------------------------------------\n");
	}

	for (i = 0;
			i < list_size((obtenerProcesoPorID(id_proceso))->tablaDeSegmentos);
			i++)
	{
		nodo = *(NodoDeSegmento*) list_get(
				obtenerProcesoPorID(id_proceso)->tablaDeSegmentos, i);
		if (output_dump) {	// segun parametro -f (se graba en output_dump.LOG)
			fprintf(
					(FILE*)output_dump,
					"[%d]      | [%d]  \t| [%d]    \t| [%d]   |\n",
					i, nodo.base, nodo.size, (int) nodo.posicionMP);
		}
		else {	// se imprime por pantalla de la consola
			printf(
					"[%d]      | [%d]  \t| [%d]    \t| [%d]   |\n",
					i, nodo.base, nodo.size, (int) nodo.posicionMP);
		}
	}
}

//Lista todos los procesos actuales y sus segmentos
void listarProcesos(int output_dump) {

	int i;
	NodoDeProceso nodo;

	if (output_dump) {	// segun parametro -f (se graba en output_dump.LOG)
		fprintf(
				(FILE*)output_dump,
				"Cant de procesos en el sistema: [%d] \n",
				list_size(listaDeProcesos));
	} else {	// se imprime por pantalla de la consola
		printf("Cant de procesos en el sistema: [%d] \n", list_size(listaDeProcesos));
	}

	for (i = 0; i < list_size(listaDeProcesos); i++) {

		nodo = *(NodoDeProceso*) list_get(listaDeProcesos, i);

		if (output_dump) {	// segun parametro -f (se graba en output_dump.LOG)
			fprintf(
					(FILE*)output_dump,
					"->  PID: [%d] \n\t****** TABLA DE SEGMENTOS ******\n",
										nodo.id_proceso);

			listarSegmentos(output_dump, nodo.id_proceso);

		} else {	// se imprime por pantalla de la consola
			printf("-> PID: [%d] \n\t******TABLA DE SEGMENTOS******\n",
					nodo.id_proceso);

			listarSegmentos(NULL, nodo.id_proceso);
		}
	}
}

void stackSegment_Push(u_int32_t pid, u_int32_t numeroDeSegmento){
/////////////////////////////////////////////////////////////////////////////////////////////////
}

void stackSegment_Pop(u_int32_t pid, u_int32_t numeroDeSegmento){
/////////////////////////////////////////////////////////////////////////////////////////////////
}

//Destruye un segmento especifico. SOLO PARA DEBUG. En el tp los segmentos se destruyen al destruir el proceso, nunca individualmente
void destruirSegmento(int id_proceso, int base) {
	NodoDeProceso *proceso = obtenerProcesoPorID(id_proceso);

	list_remove(proceso->tablaDeSegmentos, obtenerPosicionSegmentoPorBase(id_proceso,base));
	list_remove(listaAuxiliar, obtenerPosicionSegmentoPorBaseAuxiliar(16));
}
