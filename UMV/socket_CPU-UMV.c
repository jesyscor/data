/*
 * socket_CPU-UMV.c
 *
 *  Created on: 16/05/2014
 *      Author:lemmau
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <socket_CPU-UMV.h>
#include <pthread.h>

#define MAXDATASIZE 256

extern char *ip_UMV;
extern char *puerto_CPU;

fd_set master_cpu; // master file descriptor list
fd_set read_fds_cpu; // temp file descriptor list for select()
int fdmax_cpu; // maximum file descriptor number
int listener_cpu; // listening socket descriptor
int newfd_cpu; // newly accept()ed socket descriptor
struct sockaddr_storage remoteaddr_cpu; // client address
socklen_t addrlen_cpu;
char buf_cpu[MAXDATASIZE]; // buffer for client data
int nbytes_cpu;
int yes_cpu = 1;
char remoteIP_cpu[INET6_ADDRSTRLEN];
int i, j, rv;
struct addrinfo hints_cpu, *ai_cpu, *p_cpu;

void *get_in_addr_cpu(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

//Hilo para cada CPU
void atenderCPU(int fdCPU){
	pthread_t cpu;
	int sError;

	sError = pthread_create(&cpu, NULL, cpu_Handler, (void*)fdCPU );

	if(sError)	{
		fprintf(stderr,"[UMV] Error on 'pthread_create()' function: [%d] \n",sError);
		exit(EXIT_FAILURE);
	}
}

int crear_listener_cpu(const char *ip, const char *puerto) {
	FD_ZERO(&master_cpu); // clear the master and temp sets
	FD_ZERO(&read_fds_cpu);

	memset(&hints_cpu, 0, sizeof(hints_cpu)); // get us a socket and bind it
	hints_cpu.ai_family = AF_INET;
	hints_cpu.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, puerto, &hints_cpu, &ai_cpu)) != 0) {
		fprintf(stderr, "[UMV]: Error on 'selectserver' -> %s\n", gai_strerror(rv));
		exit(1);
	}

	for (p_cpu = ai_cpu; p_cpu != NULL ; p_cpu = p_cpu->ai_next) {
		listener_cpu = socket(p_cpu->ai_family, p_cpu->ai_socktype, p_cpu->ai_protocol);
		if (listener_cpu < 0) {
			continue;
		}

		// lose the pesky "address already in use" error message
		setsockopt(listener_cpu, SOL_SOCKET, SO_REUSEADDR, &yes_cpu, sizeof(int));
		if (bind(listener_cpu, p_cpu->ai_addr, p_cpu->ai_addrlen) < 0) {
			close(listener_cpu);
			continue;
		}
		break;
	}

	// if we got here, it means we didn't get bound
	if (p_cpu == NULL ) {
		fprintf(stderr, "[UMV]: Error on 'selectserver' -> failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai_cpu); // all done with this

	// listen
	if (listen(listener_cpu, 10) == -1) {
		perror("[UMV]: Error on 'listen' function");
		exit(3);
	}
	// add the listener to the master set
	FD_SET(listener_cpu, &master_cpu);
	// keep track of the biggest file descriptor
	fdmax_cpu = listener_cpu; // so far, it's this one

	printf("[UMV]: Hilo a la espera de CPUs...\n");
	return listener_cpu;
}

void *inicializar_SocketCPUUMV() {

	crear_listener_cpu(ip_UMV, puerto_CPU);

	for (;;) {
		read_fds_cpu = master_cpu; // copy it (copiamos el master al read_fds)
		if (select(fdmax_cpu + 1, &read_fds_cpu, NULL, NULL, NULL ) == -1) //Ver los otros fd_set, tal vez los necesite.
		{
			perror("[UMV]: Error on 'select' function");
			exit(4);
		}
		// run through the existing connections looking for data to read
		for (i = 0; i <= fdmax_cpu; i++)
		{
			if (FD_ISSET(i, &read_fds_cpu))
			{ // we got one!!
				if (i == listener_cpu)
				{ // handle new connections
					addrlen_cpu = sizeof remoteaddr_cpu;
					newfd_cpu = accept(listener_cpu, (struct sockaddr *) &remoteaddr_cpu,
							&addrlen_cpu);
					if (newfd_cpu == -1) {
						perror("[UMV]: Error on 'accept' function (dentro de select)");
					} else {
						FD_SET(newfd_cpu, &master_cpu); // add to master set
						if (newfd_cpu > fdmax_cpu) { // keep track of the max
							fdmax_cpu = newfd_cpu;
						}
						printf("[UMV]: Nueva conexion en host [%s] y port [%s]\n",
								inet_ntop(remoteaddr_cpu.ss_family,
										get_in_addr_cpu((struct sockaddr*) &remoteaddr_cpu),
										remoteIP_cpu, INET6_ADDRSTRLEN),
										puerto_CPU);
						// tengo en cuenta RETARDO en milisegundos (puede cambiarse desde la CMD).
						usleep(RETARDO * 1000);
						atenderCPU(newfd_cpu);
					}
				} else {
					//handle data from a client
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

	return 0;

}

void cerrar_CPUUMV() {
	close(listener_cpu);
}
