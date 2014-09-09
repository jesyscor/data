/*
 * Kernel.c
 *
 *  Created on: 23/04/2014
 *      Author:lemmau
 */

#include "Primitivas.h"

const int SIZE_VARIABLE = 5;

//Modificado para probar

void agregarADiccionario(char identificador_variable, u_int32_t posicion) {
	t_dic_variables *varDiccionario = malloc(sizeof(t_dic_variables));

	varDiccionario->variable = malloc(2);
	varDiccionario->variable[0] = identificador_variable;
	varDiccionario->variable[1] = '\0';
	varDiccionario->posicion = posicion;

	dictionary_put(dictionaryVariables, varDiccionario->variable,
			&varDiccionario->posicion);

	printf("Se agrego [%d] \n", varDiccionario->posicion);
	//free(varDiccionario->variable);
	//free(varDiccionario);
}

int imprimir(t_valor_variable valor_mostrar) {

    paq_numero datos;
    datos.numero = valor_mostrar;

	preparar_paquete(sockfd_cte_Kernel, IMPRIMIR_ENTERO, &datos);

	return cuentaDigitos(valor_mostrar);
}

int imprimirTexto(char* texto){

	mensaje* datos;
	datos->out = texto;
	int tamanho = strlen(texto); //TODO:Verificar si hay que agregar +1 a la longitud

	preparar_paquete(sockfd_cte_Kernel, IMPRIMIR_TEXTO, &datos);

	return(tamanho);
}

t_puntero definirVariable(t_nombre_variable identificador_variable) {

	stream_t *serializado;
	u_int32_t sizeTotal;

	int id;
	//Paso la variable a un char*
	char* variable = malloc(SIZE_VARIABLE);
	memcpy(variable, &identificador_variable, 1);

	//Creo el tamaño de contexto necesario
	u_int32_t sizeContexto = pcbActual.tamanioCotextoActual * SIZE_VARIABLE;

	//Pido a la UMV que guarde la variable
	serializado = serialize_struct_sol_Escritura(pcbActual.segmentoDeStack,
			sizeContexto, SIZE_VARIABLE, variable, &sizeTotal);

	send_msg_to(sockfd_cte_UMV, SOLICITUD_ESCRITURA_ENVIADA,
			serializado->data, sizeTotal);

	//Recibo la posicion donde se guardo la variable ACA HAY QUE COMPROBAR SI HUBO STACK OVERFLOW, fijarse que la UMV mande -1
	char* paquete;
	u_int32_t posicion;

	recv_msg_from(sockfd_cte_UMV, &id, &paquete);

	memcpy(&posicion, paquete, sizeof(posicion));

	if (posicion == -1) {
		perror("Stack overflow.");
		return 0;
	}

	//Agregar al diccionario de variables
	//Me muevo para no guardar la posicion de la variable, sino de donde tengo que empezar a escribir
	posicion++;

	agregarADiccionario(identificador_variable, posicion);

	printf("Se tiene a [%c]  en la pos [%d] \n", variable[0], posicion);
	//Modifico el tamaño del contexto
	pcbActual.tamanioCotextoActual++;

	return posicion;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	t_puntero posVariable;

	char* key = malloc(2);
	key[0] = identificador_variable;
	key[1] = '\0';

	posVariable = *(t_puntero*) dictionary_get(dictionaryVariables, key);

	printf("Recupero pos de variable [%c] = [%d] \n", identificador_variable, posVariable);
	free(key);

	return posVariable;
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {
	int id;
	stream_t * serializado;
	char * paquete;
	t_valor_variable valor;
	int size = sizeof(paq_sol_LeerMemoria);

	printf("De-referenciando [%d] \n", direccion_variable);

	serializado = serialize_struct_sol_Lectura(pcbActual.segmentoDeStack, direccion_variable - pcbActual.segmentoDeStack, sizeof(u_int32_t));
	send_msg_to(sockfd_cte_UMV, SOLICITUD_LECTURA_ENVIADA, serializado->data, size);

	//Espera respuesta
	recv_msg_from(sockfd_cte_UMV, &id, &paquete);

	memcpy(&valor, deserialize_struct_res_SolicitudLectura(paquete)->datos, sizeof(u_int32_t));

	printf("Se dereferencio el valor [%d] \n", valor);

	return valor;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {
	int id;
	char* paquete;
	stream_t *serializado;
	u_int32_t sizeTotal;
	u_int32_t posicionOffset = direccion_variable - pcbActual.segmentoDeStack;

	printf("Asignando a [%d] el valor [%d] \n", posicionOffset , valor);

	//Pido a la UMV que guarde la variable
	serializado = serialize_struct_sol_Escritura(pcbActual.segmentoDeStack,	posicionOffset, sizeof(u_int32_t), (char*)&valor , &sizeTotal);

	send_msg_to(sockfd_cte_UMV, SOLICITUD_ESCRITURA_ENVIADA, serializado->data, sizeTotal);

	//Espera respuesta, aunque no la usemos
	recv_msg_from(sockfd_cte_UMV, &id, &paquete);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {

	printf("Obtener valor compartida");
	return 0;
}

t_valor_variable asignarValorCompartida(t_nombre_compartida t_valor_variable) {

	printf("Asignar Valor Compartida");
	return 0;
}
//
//t_puntero_instruccion irAlLabel(t_nombre_etiqueta etiqueta){
//
//}
//
//t_puntero_instruccion llamarSinRetorno(t_nombre_etiqueta etiqueta, t_puntero_instruccion linea_en_ejecucion){
//
//}
//
//t_puntero_instruccion llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar, t_puntero_instruccion linea_en_ejecucion){
//
//}
//
//t_puntero_instruccion finalizar(void){
//
//}
//
//t_puntero_instruccion retornar(t_valor_variable retorno){
//
//}
//
//int entradaSalida(t_nombre_dispositivo dispositivo, int tiempo){
//
//}
//
//int wait(t_nombre_semaforo identificador_semaforo){
//
//}
//
//int signal(t_nombre_semaforo identificador_semaforo){
//
//}

