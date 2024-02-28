// Server IPv4 - Asteapta sa primeasca comenzi de la Clientul IPv4.
// Daca comanda primita este 12#, serverul trimite clientului pagina html, altfel trimite mesajul: Comanda neimplementata!
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

// Variabila globala
int close_server=1;

// Declararea prototipurilor de functii ale fisierului client.c.
int create_connect_request();
void receive(char*, size_t); 

// Functie realizata pentru a afisa un mesaj de confirmare atunci cand utilizatorul doreste sa inchida serverul cu CTRL+C.
void signal_handler() {     
	   if (close_server==1)	{  
		  char c;
	   	  printf("Do you want to close the server? (Y/N):");
		  c = getchar();
		  if (c == 'y' || c == 'Y')
	        	exit(0);
	   } else close_server = 1;    // Pentru a inchide serverul dupa ce clientul a fost servit.
}

int main() {
	signal(SIGINT, signal_handler);      // Semnalul SIGINT corespunzator CTRL+C.

        // Creeare socket server.

	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		 perror("Socket creation failed! \n\n");
                 exit(EXIT_FAILURE);
        } else {
                  printf("\nServer socket created succesfully! \n\n");
        }

	// Apelare bind() pentru a asocia adresa IP si portul cu socket-ul.
	
	struct sockaddr_in myServerAddr;
	myServerAddr.sin_family = AF_INET;
	myServerAddr.sin_port = htons(22117);            // Port Server 22GSE.
	myServerAddr.sin_addr.s_addr = inet_addr("46.243.115.196");           // Adresa IPv4 Server.
	memset(myServerAddr.sin_zero, '\0', sizeof myServerAddr.sin_zero);

        int bindStatus = bind(server_socket, (struct sockaddr *)&myServerAddr, sizeof myServerAddr);

        if (bindStatus == -1) {
		perror("Error binding!\n\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Binding was succesful!\n\n");
	}

        // Apelare listen() pentru a marca server_socket ca fiind pregatit sa astepte conexiuni de la clienti.
	
	int listenStatus = listen(server_socket, 5);
	if (listenStatus == -1) {
		perror("Error listening!\n\n");
		exit(EXIT_FAILURE);
	} else {
		printf("The server is listening...\n\n");
	}
        
	// Serverul va astepta conexiuni pe o durata infinita ( poate fi intrerupt cu CTRL+C ).
	for(;;) {
	
	close_server=1;

	// Apelare accept() pentru a accepta conexiuni de la clienti.
	
	struct sockaddr_in client;
	int len;
	len = sizeof(client);
	int new_socket;
	new_socket = accept(server_socket,(struct sockaddr*)&client, &len);     // accept() returneaza un socket nou.

	if (new_socket == -1) {
                  perror("Error accepting the connection from the client!\n\n");
                  exit(EXIT_FAILURE);
        } else {
                  printf("Client connected succesfully!\n\n");
        }
	
        
        // Primire comanda de la client.
	
        char receive_buffer[1024];
	int bytes_received;
        
	do {
        char msg[30000];               // In aceasta variabila se va stoca continutul .html daca comanda primita este 12#
      	close_server=1;       
	printf("Waiting for command...\n\n");
	bytes_received = recv(new_socket, receive_buffer, sizeof(receive_buffer), 0);
	close_server=0;
	if (bytes_received == -1) {
		printf("Command not received!\n\n");
		break;
	} else if (bytes_received == 0) {	
		printf("Client has disconnected!\n\n");
		break;
	}	
	else {
		printf("  * Command received! Bytes received: %d\n\n", bytes_received);
		if (strncmp(receive_buffer, "12#", 3) == 0) {            // Se verifica daca comanda primita de la client este 12.
			printf("  * Command is 12#!\n\n");
			receive(msg, sizeof(msg));                     // Se apeleaza functia receive() din client.c pentru a stoca in msg continutul .html.
		} else {
			printf("  * Command is not implemented!\n\n");
                        strcpy(msg, "Comanda neimplementata!\n");              // Daca comanda primita nu este 12, se va trimite mesajul: Comanda neimplementata!
		}
	}
	// Trimiterea mesajului(msg) catre clientul IPv4.
        int bytes_send = send(new_socket, msg, strlen(msg), 0);    // Se trimite mesajul catre client.
	if (bytes_send == -1) {
                  printf("Error sending the message!\n\n");
                  exit(0);
        } else if (bytes_send != strlen(msg)) {
                  printf("Couldn't transmit the whole message!\n\n");
        } else {
                  printf("  * Message was sent succesful to the client! Bytes send: %d.\n\n\n\n", bytes_send);
        }

	// Inchidere server dupa ce s-a servit clientul curent daca se solicita.
	if (close_server==1) {	
		char c;
		printf("Do you want to close the server? (Y/N)2");
                c=getchar();                  
	       	if (c == 'y' || c == 'Y') {
		 close(new_socket);
	         close(server_socket);
	         exit(0);
		}
	}
	memset(msg, 0, sizeof(msg));	// Dupa ce mesajul se trimite catre client msg se reseteaza.
	} while (bytes_received > 0);

        close(new_socket); 	// Inchidere socket client.

	printf("The server is listening...\n\n");
	} // end for

	close(server_socket);              // Inchidere socket server.

        return 0;
}
