/* time_client.c - main */

#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>                                                                            
#include <netinet/in.h>
#include <arpa/inet.h>
                                                                                
#include <netdb.h>

#define BUFLEN		100	/* buffer length */
#define	MSG		"Any Message \n"

struct pdu {
    char type;
    char data[99];
};

void handle_register_content(void);
void handle_deregister_content(void);
void handle_list_and_download(void);
void clode_application(void);
void handle_std_input(void);
void handle_list_content(int udp_socket, struct sockaddr_in index_server);
void handle_search_content(int udp_socket, struct sockaddr_in index_server);
void handle_download_content(int tcp_socket, struct sockaddr_in client);

void send_udp_request();
void send_tcp_request();

void receive_udp_response(int socket, char* response, size_t response_size);
void receive_tcp_response(int socket, char* response, size_t response_size);

void handle_error_response(char response_type);

char peer_name[11], std_buf[100], *host = "localhost", req_buffer[100], file_req_buffer[1640];
int port = 3000, mode=0, indx_sock;
fd_set afds, rfds;

void serialize(char type,  char* data, size_t data_size ,char buffer[BUFLEN]) {
	buffer[0] = type;
	int i;
	for(i = 0 ; i < BUFLEN-1; i++) buffer[i+1] = data[i];
}

void deserialize(struct pdu data, size_t data_size ,char buffer[BUFLEN]) {
	data.type = buffer[0];
	int i;
	for(i = 0; i < BUFLEN-1; i++) data.data[i] = buffer[i+1];
}


int display_menu() {

	switch(mode) {
	case 0:
		printf("===== Menu =====\n");
		printf("1. Register Content\n");
		printf("2. Deregister Content\n");
		printf("3. List and Download Available Content\n");
		printf("0. Quit\n");
		printf("================\n");
		printf("Enter your choice: ");
	break;
	case 1:
		printf("===== Register Content =====\n");
		printf("Please enter a file name: \n");
	break;
	case 2:
		printf("===== De-Register Content =====\n");
		printf("Please enter the filename");
	break;
	case 3:
		printf("Listing available content");
		handle_list_and_download();
	break;
	}
    
}

void handle_std_input() {
	char str_input[100];
	int new_mode;
	switch(mode) {
		case 0: 
			scanf("%d", &new_mode);
			if(new_mode >=0 && new_mode <= 3) {
				mode = new_mode;
			} else {
				printf("Invalid input. Please select an appropriate option\n");
			}
		break;
		case 1:
			scanf("%d", &str_input);
			handle_register_content();
		break;
		case 2:
			scanf("%d", &str_input);
			handle_deregister_content();
		break;
	}
}

void handle_socket_input() {

}

void handle_register_content() {
	struct sockaddr_in reg_addr;
	int s, alen;
	s = socket(AF_INET, SOCK_STREAM, 0);
	reg_addr.sin_family = AF_INET;
	reg_addr.sin_port = htons(0);
	reg_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s, (struct sockaddr *)&reg_addr, sizeof(reg_addr));
	alen = sizeof (struct sockaddr_in);
	getsockname(s, (struct sockaddr *) &reg_addr, &alen);
	printf("Socket port: %d", reg_addr.sin_port);
	printf("Socket addr: %s", reg_addr.sin_addr);
	//make udp connection to register content using filename
}

void handle_deregister_content() {
	//make udp connection to content using filename
	//close socket and thread
}

void handle_list_content(int udp_socket, struct sockaddr_in index_server) {
	//make udp connection to find content
	printf("Files available for download: \n");
}

void handle_search_content(int udp_socket, struct sockaddr_in index_server) {
	printf("Searching for content: %s", std_buf);
}

void send_udp_request(size_t file_size) {
	write(indx_sock, req_buffer,file_size);
}

void send_tcp_request(int socket, size_t file_size) {
	write(socket, req_buffer, file_size);
}
/*------------------------------------------------------------------------
 * main - UDP client for TIME service that prints the resulting time
 *------------------------------------------------------------------------
 */
int
main(int argc, char **argv){

	struct hostent	*phe;	/* pointer to host information entry	*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, n, j, type;	/* socket descriptor and socket type	*/
	char req_buffer[101], res_buffer[101];


	//add stdin to fds
	FD_ZERO(&afds);
	FD_SET(0, &afds);
	memcpy(&rfds, &afds, sizeof(rfds));

	struct pdu spdu;
	struct pdu file_pdu;
	memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;                                                                
        sin.sin_port = htons(port);

	//get index server port and ip from command line arguments
	switch (argc) {
	case 1:
		break;
	case 2:
		host = argv[1];
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "usage: UDPtime [host [port]]\n");
		exit(1);
	}

	//creating socket for udp server connection.
	/* Map host name to IP address, allowing for dotted decimal */
	if ( phe = gethostbyname(host) ){
			memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	}
	else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
	fprintf(stderr, "Can't get host entry \n");
																			
/* Allocate a socket */
	indx_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (indx_sock < 0)
	fprintf(stderr, "Can't create socket \n");

																			
/* Connect the socket */
	if (connect(indx_sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	fprintf(stderr, "Can't connect to %s %s \n", host, "Time");

	//run code
	while (1) {
	display_menu();
	if (select(FD_SETSIZE, &rfds, NULL, NULL, NULL) == -1) {
	perror("Select failed");
	break;  // Exit the loop on select failure
	}
	if(FD_ISSET(0, &rfds)) {
		handle_std_input();
	} else {
		handle_socket_input();
	}

	}
	exit(0);
}
