/*
 * Enviar_Recibir_Paquete.c
 *
 *  Created on: 14/05/2014
 *      Author:lemmau
 *
 *      EJEMPLO DE USO:

        void *mensaje = NULL;
		u_int32_t identificador;

		// Se lee el mensaje
		read_msg_from (socket, &identificador, &mensaje);

			//Se hace el tratamiento del mensaje según el identificador
			switch (identificador)
			{
				case operacion1:
				{
					// Se hace un cast de void * a struc_operacion1 *, para tener más accesibles los datos recibidos
					Struct_operacion1 *datos = (Struct_operacion1 *)mensaje;
					... // tratamiento de la variables datos
					break;
				}

				case operacionN:
				{
					Struct_operacionN *datos = (Struct_operacionN *) mensaje;
					...//tratamiento de la variable datos (por ejemplo, desempaquetar)
					break;
				}
			}

			// Se libera el mensaje  cuando ya no lo necesitamos
			if (mensaje != NULL)
			{
				free (mensaje);
				mensaje = NULL;
			}
*/

#include "Enviar_Recibir_Paquete.h"

int endianess()
{
	unsigned i = 1;
	return ((*(char*)&i) == 1)? 1 : 0;
}

int sendall(int socket, void *buf, int *len)
{// Envia todos los bytes del buffer a traves de varios SEND's si es necesario.

	int n;
	int total = 0;			// how many bytes we've sent
	int bytesleft = *len;	// how many we have left to send

	while(total < *len) {
		n = send(socket, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total;			// return number actually sent here
	return n == -1 ? -1 : 0;// return -1 on failure, 0 on success
}


//int recvall(int socket, void *buf, int *len)
//{
//
//	return 0;
//}

void recv_msg_from(int socket, int *idMensaje, char **mensaje)
{
    Cabecera cabecera;
    *mensaje = NULL;  /* Ponemos el mensaje a NULL por defecto */
    int nbytes;

    if((nbytes = recv(socket, &cabecera, sizeof(cabecera), 0)) == -1) /* Se lee la cabecera */
    {
		perror("Error on 'recv_msg_from()' function at MiniGral_lib");
		exit(1);
    }

    /* Rellenamos el identificador para devolverlo */
    *idMensaje = cabecera.identificador; // --> El identificador es tu ID de operacion! Hacer enums para estos, son mas descriptivos.

    /* Si hay que leer una estructura detrás */
    if (cabecera.longitud > 0)
    {
        *mensaje = malloc (cabecera.longitud);  /* Se reserva espacio para leer el mensaje */
        recv(socket, *mensaje, cabecera.longitud, 0);
    }
}


void send_msg_to(int socket, int idMensaje, char *mensaje, int tamanho)
{// Enviamos la cabecera (id,length) y luego el cuerpo (mensaje)
    /* Se declara y rellena la cabecera */
    Cabecera cabecera;
    int bytesEnviados = 0;

    cabecera.identificador = idMensaje;
    cabecera.longitud = tamanho;
    bytesEnviados = sizeof(cabecera);

    /* Se envía la cabecera */
    if(sendall(socket, &cabecera, &bytesEnviados) == -1)
    {
    	perror("Error on 'send_msg_to()' function at MiniGral_lib.");
    	printf("Solo se enviaron [%d] bytes a causa del error.", bytesEnviados);
    	exit(1);
    }

    /* Si el mensaje no tiene cuerpo, hemos terminado */
    if ((mensaje == NULL) || (tamanho == 0))
        return;

    /* Se envía el cuerpo */
    bytesEnviados = tamanho;
    if(sendall(socket, mensaje, &bytesEnviados) == -1)
    {
    	perror("Error on 'send_msg_to()' function at MiniGral_lib.");
    	printf("Solo se enviaron [%d] bytes a causa del error.", bytesEnviados);
    	exit(1);
    }
}
