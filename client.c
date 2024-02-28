// Client IPv6 - Clientul se conecteaza la un Server IPv6 HTTP si trimite o cerere GET dupa care primeste un raspuns de la server.
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// In aceasta functie se creeaza socket-ul, clientul se conecteaza la server si trimite o cerere de tip GET.

int create_connect_request() {
        
         // Creeare socket client.
 
         int network_socket;
         network_socket = socket(PF_INET6, SOCK_STREAM, 0);
 
         if (network_socket == -1) {
                  perror("Socket creation failed! \n\n");
                  exit(EXIT_FAILURE);
          } else {
                  printf("  * The client socket was created successfully! \n\n");
          }
 
          // Conectare Client la Server.
 
          struct sockaddr_in6 servAddr;
          servAddr.sin6_family = AF_INET6;               // IPv6
          servAddr.sin6_port = htons(80);                // Port default pentru HTTP.
          // inet_pton(AF_INET6, "2001:62a:4:70::80:135", &servAddr.sin6_addr);          // www.univie.ac.at (nu merge!)
          inet_pton(AF_INET6, "2001:470:0:503::2", &servAddr.sin6_addr);                 // Adresa IPv6 a serverului (he.net)
          int connectStatus = connect(network_socket, (struct sockaddr*)&servAddr, sizeof(servAddr));
 
          if (connectStatus == -1) {
                  perror("Error connecting to the server!\n\n");
                  exit(EXIT_FAILURE);
          } else {
                  printf("  * Client connected to the server successfully!\n\n");
          }
 
          // Trimitere a unei cereri de tip GET de la client catre server.
 
          const char request[] = "GET / HTTP/1.0\r\n\r\n";                  // Cererea de tip GET.
          int bytes_send = send(network_socket, request, strlen(request), 0);   // Trimitere cerere catre server.
 
          if (bytes_send == -1) {
                  perror("Error sending the request!\n\n");
                  exit(EXIT_FAILURE);
          } else if (bytes_send != strlen(request)) {
                  printf("Couldn't transmit the whole message!\n\n");
          } else {
                  printf("  * Client send the request to the server successfully! Bytes send: %d.\n\n", bytes_send);
          }
	  return network_socket;                          // Se returneaza descriptorul de comunicatie pentru a fi folosit in functia receive().
}

// In aceasta functie clientul primeste raspunsul de la server. Raspunsul se salveaza local intr-un fisier index.html si intr-un response_buffer
// pentru a fi trimis catre clientul IPv4.
void receive(char *response_buffer, size_t buffer_size) {

	  // Se creeaza un fisier local cu extensia .html in care se va scrie raspunsul primit de la server.
	 
	  FILE *html_file;
	  html_file = fopen("/home/g1s1e7/final/index.html", "w");
	  if (html_file == NULL) {
		  perror("Error opening the file");
		  exit(EXIT_FAILURE);
	  }

	  // Primire raspuns de la server.

          char response[1024];               // Variabila in care se va stoca raspunsul primit de la server.
          int bytes_received;
          int network_socket = create_connect_request();           // Se apeleaza prima functia pentru a prelua network socket-ul.
          do {
            bytes_received = recv(network_socket, response, sizeof(response), 0);
            if (bytes_received == -1) {
                   printf("Response error!\n\n");
            } else if (bytes_received == 0) {
                   printf("  * Connection was closed by the remote site!\n\n");
            } else {
		   strncat(response_buffer, response, buffer_size - strlen(response_buffer)-1);     // Salvare raspuns de la server pentru a fi trimis catre clientul IPv4.
		   fprintf(html_file, response);      // Se scrie raspunsul primit de la server in fisierul index.html.
		   fflush(html_file);                 // Clear buffer.
                   continue;
           }
          } while (bytes_received > 0);
	 fclose(html_file);                          // Inchidere fisier.
	 close(network_socket);                      // Inchidere socket.
}
