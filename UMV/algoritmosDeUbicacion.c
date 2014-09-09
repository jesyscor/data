/*
 * algoritmosDeUbicacion.c
 *
 *  Created on: 28/04/2014
 *      Author:lemmau
 *
 *  ALGORITMOS DE UBICACION EN SEGMENTOS Y COMPACTACION DE MEMORIA.
 *
 *  WORST FIT : busca el bloque mas grande disponible y guarda el segmento ahi.
 *  FIRST FIT : guarda el segmento en el primer bloque que encuentra (y que tiene tamaño suficiente para albergarlo)
 *
 *  Ambos algoritmos retornan el NODO DEL SEGMENTO con todos los datos cargados
 *
 */

#include <algoritmosDeUbicacion.h>

//Solicitar y Guardar buffers

char *solicitarBloque(int proceso, int base, int offset, int size) {

	NodoDeSegmento *segmentoSolicitado = obtenerSegmentoPorBase(proceso, base);
	char *buffer = malloc(size);
printf("proceso = [%d], base = [%d], offset = [%d], size = [%d], ",proceso,base,offset,size);
	memset(buffer, 0, size);

	if (segmentoSolicitado == 0)
		return 0;

	if (size + offset <= segmentoSolicitado->size) {
		memcpy(buffer,(char*)(segmentoSolicitado->posicionMP + offset), size);
		return buffer;
	}

	free(buffer);
	return 0;
}

//Devuelve el lugar donde empezo a guardar los datos

int guardarBloque(int proceso, int base, int offset, int size, char *bufferAEscribir) {

	NodoDeSegmento *segmentoSolicitado = obtenerSegmentoPorBase(proceso, base);

	if (size + offset <= segmentoSolicitado->size) {
		char *bufferDestino = (char*) (segmentoSolicitado->posicionMP + offset);
		memset(bufferDestino, 0, size);
		memcpy(bufferDestino, bufferAEscribir, size);
		return segmentoSolicitado->base+offset;
	}
	return -1;
}

bool comparador(void *dato1, void *dato2) {

	NodoDeSegmentoAuxiliar *nodo1 = dato1;
	NodoDeSegmentoAuxiliar *nodo2 = dato2;

	if (nodo1->posicionMP < nodo2->posicionMP)
		return true;
	else
		return false;
}

//El algoritmo FirstFit guarda el segmento en el primer bloque que encuentra (y que tiene tamaño suficiente para albergarlo)
//Retorna el nodo con todos los datos guardados.

NodoDeSegmento crearSegmento_firstFit(NodoDeProceso datosDelProceso,
		NodoDeSegmento datosDelSegmento) {
	int i;

//Recorro la lista auxiliar y le busco un lugar a mi segmento nuevo

	for (i = 0; i < list_size(listaAuxiliar); i++) {
		NodoDeSegmentoAuxiliar *nodoSegmentoActual = list_get(listaAuxiliar, i);
		NodoDeSegmentoAuxiliar *nodoSegmentoSiguiente = list_get(listaAuxiliar,
				i + 1);
		NodoDeSegmentoAuxiliar *nodoSegmentoNuevo = malloc(
				sizeof(NodoDeSegmentoAuxiliar));

		//Pregunto si hay lugar
		if ((nodoSegmentoSiguiente->posicionMP
				- (nodoSegmentoActual->posicionMP + nodoSegmentoActual->size))
				> datosDelSegmento.size) {

			datosDelSegmento.posicionMP = 1
					+ ((int) nodoSegmentoActual->posicionMP)
					+ nodoSegmentoActual->size;

			datosDelSegmento.base = datosDelSegmento.posicionMP
					- (u_int32_t) bloquePrincipal - 1;

			//Alocar el segmento nuevo
			agregarSegmento(datosDelProceso.id_proceso, datosDelSegmento.base,
					datosDelSegmento.size, datosDelSegmento.posicionMP);
			//Agregar el segmento nuevo a la lista auxiliar

			nodoSegmentoNuevo->base = datosDelSegmento.base;
			nodoSegmentoNuevo->posicionMP = datosDelSegmento.posicionMP;
			nodoSegmentoNuevo->size = datosDelSegmento.size;
			nodoSegmentoNuevo->proceso = datosDelProceso.id_proceso;

			list_add(listaAuxiliar, nodoSegmentoNuevo);

			//Ordeno la lista Auxiliar (se podría hacer agregar ordenado
			//directamente, pero no esta la funcion en list.h)

			list_sort(listaAuxiliar, comparador);
			return datosDelSegmento;
		}
	}
		datosDelSegmento.base = -1;
		return datosDelSegmento;
}

// TODO **NO PROBADO, EN TEORIA FUNCIONA
//El algoritmo WorstFit busca el bloque mas grande disponible y guarda el segmento ahí
//Retorna el nodo del segmento con todos los datos cargados

NodoDeSegmento crearSegmento_worstFit(NodoDeProceso datosDelProceso,
		NodoDeSegmento datosDelSegmento) {

	NodoDeSegmentoAuxiliar *nodoSegmentoNuevo = malloc(
			sizeof(NodoDeSegmentoAuxiliar));

	int i;
	int posicionElegida = -1;
	int sizeMaximo = 0;

	int sizeDisponible = 0;
//Recorro la lista auxiliar y le busco un lugar a mi segmento nuevo

	for (i = 0; i < list_size(listaAuxiliar); i++) {
		NodoDeSegmentoAuxiliar *nodoSegmentoActual = list_get(listaAuxiliar, i);
		NodoDeSegmentoAuxiliar *nodoSegmentoSiguiente = list_get(listaAuxiliar,	i + 1);

		//Calculo el tamaño que hay disponible en ese hueco
		sizeDisponible = nodoSegmentoSiguiente->posicionMP
				- (nodoSegmentoActual->posicionMP + nodoSegmentoActual->size);

		//Pregunto si es mas grande que el que tengo almacenado y si entra ahí
		if (sizeDisponible > datosDelSegmento.size
				&& sizeDisponible > sizeMaximo) {
			sizeMaximo = sizeDisponible;
			posicionElegida = nodoSegmentoActual->posicionMP
					+ nodoSegmentoActual->size;
		}
	}

	if (posicionElegida == -1)
	{
		datosDelSegmento.base = -1;
				return datosDelSegmento;
	}

	datosDelSegmento.posicionMP = 1 + posicionElegida;

	datosDelSegmento.base = datosDelSegmento.posicionMP
			- (u_int32_t) bloquePrincipal - 1;

	//Alocar el segmento nuevo
	agregarSegmento(datosDelProceso.id_proceso, datosDelSegmento.base,
			datosDelSegmento.size, datosDelSegmento.posicionMP);
	//Agregar el segmento nuevo a la lista auxiliar

	nodoSegmentoNuevo->base = datosDelSegmento.base;
	nodoSegmentoNuevo->posicionMP = datosDelSegmento.posicionMP;
	nodoSegmentoNuevo->size = datosDelSegmento.size;
	nodoSegmentoNuevo->proceso = datosDelProceso.id_proceso;

	list_add(listaAuxiliar, nodoSegmentoNuevo);

	//Ordeno la lista Auxiliar (se podría hacer agregar ordenado
	//directamente, pero no esta la funcion en list.h)

	list_sort(listaAuxiliar, comparador);
	return datosDelSegmento;
}

//Funcion para debug, no usar! **PRIVADO
void imprimirListaAuxiliar() {
	int i;
	for (i = 0; i < list_size(listaAuxiliar); i++) {
		NodoDeSegmentoAuxiliar *nodoSegmentoActual = list_get(listaAuxiliar, i);
		printf("PosMP segmento: %d \n Size Segmento: %d \n Base Segmento: %d\n",
				nodoSegmentoActual->posicionMP, nodoSegmentoActual->size,
				nodoSegmentoActual->base);
	}
}

void compactar()
{
	int i;
//	pthread_mutex_lock(&mutex);
	for (i = 0; i < list_size(listaAuxiliar) - 2; i++)
	{
		NodoDeSegmentoAuxiliar *nodoSegmentoActual = list_get(listaAuxiliar, i);
		NodoDeSegmentoAuxiliar *nodoSegmentoSiguiente = list_get(listaAuxiliar,	i + 1);

		if (nodoSegmentoSiguiente->posicionMP
				!= (nodoSegmentoActual->posicionMP + nodoSegmentoActual->size + 1))
		{	//Me fijo si el proximo nodo empieza cuando termina el actual.
			//Entonces me guardo los datos de esa posicionMP y los escribo en la nueva posicionMP
			char *datosAMover = solicitarBloque(nodoSegmentoSiguiente->proceso,
												nodoSegmentoSiguiente->base,
												0,
												nodoSegmentoSiguiente->size);

			//Lo copio al final del nodo actual TODO estoy offseteando ese +1 aca y cuando creo segmento también, hay que arreglar esto
			//ya que los que tenian en realidad 15 de size pasan a tener 16

			memcpy(
					(char*) nodoSegmentoActual->posicionMP
							+ nodoSegmentoActual->size + 1, datosAMover,
					strlen(datosAMover));

			//guardarBloque(nodoSegmentoActual->proceso,nodoSegmentoActual->base,0, nodoSegmentoActual->size, datosAMover); Esto no va.
			/*TODO Hay algo interesante a tener en cuenta. Si yo destruyo un SEGMENTO y luego compacto, la compactación trata de
			 * recuperar los datos que había en ese segmento que borre y copiarlos. Sin embargo no los encuentra por que el nodo ya fue destruido
			 * y obtenerSegmentoPorBase no devuelve nada. Ya que solicitarBloque y guardarBloque trabajan según la Base y dicha base ya no existe.
			 *
			 * Una solución es hacer un memcopy directamente en la compactación, sin utilizar estas funciones de solicitar y guardar.
			 * Por ahora voy a hacer eso, si trae problemas a futuro hay que tener en cuenta esto para cambiarlo.
			 */

			//Agarro el nodo de la tabla de segmentos, lo voy a necesitar mas adelante
			NodoDeSegmento *nodoEnTabla = obtenerSegmentoPorBase(
					nodoSegmentoSiguiente->proceso,
					nodoSegmentoSiguiente->base);

			//Actualizo el nodo en la lista Auxiliar.

			nodoSegmentoSiguiente->posicionMP = 1
					+ nodoSegmentoActual->posicionMP + nodoSegmentoActual->size;

			/*nodoSegmentoSiguiente->base = nodoSegmentoSiguiente->posicionMP   //Esto no debería actualizarlo
			 - (u_int32_t) &bloquePrincipal - 1;*/

			//Actualizo el nodo en la tabla de segmentos
			//La base no se actualiza, queda la misma (Ver mails de Elizabeth 05/05/14)
			nodoEnTabla->posicionMP = nodoSegmentoSiguiente->posicionMP;
		}
	}
//	pthread_mutex_unlock(&mutex);
}

