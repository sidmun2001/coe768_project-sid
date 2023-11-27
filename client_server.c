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
#define FILEDATABUFFLEN		1024	/* file data length */
#define	MSG		"Any Message \n"
#define MAXFILES 5

struct pdu {
    char type;
    char data[BUFLEN-1];
};

void handle_register_content();
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

char peer_name[11], std_buf[100], req_buffer[100], file_req_buffer[1640], res_buffer[100], std_input[100], ip_add[10];
int mode=0, indx_sock;
fd_set afds, rfds;
struct pdu req_pdu, res_pdu;

void serialize() {
	req_buffer[0] = req_pdu.type;
	int i;
	for(i = 0 ; i < BUFLEN-1; i++) req_buffer[i+1] = req_pdu.data[i];
}

void deserialize() {
	res_pdu.type = res_buffer[0];
	int i;
	for(i = 0; i < BUFLEN-1; i++) res_pdu.data[i] = res_buffer[i+1];
	memset(res_buffer, 0, sizeof(res_buffer));
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
		printf("====Listing available content====\n");
		handle_list_and_download();
	break;
	}
    
}

void handle_std_input() {
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
			scanf("%s", std_input);
			printf("registering file: %s\n", std_input);
			handle_register_content();
			printf("file registered");
		break;
		case 2:
			scanf("%d", &std_input);
			handle_deregister_content();
			mode=0;
		break;
	}
}

void handle_socket_input(int socket) {
	int j;
	int i;
	while (j != 0) { /*repeatedly reading until termination*/
	j = read(socket, res_buffer, BUFLEN);
	if(j <0 ){
		printf("Error");
		close(socket);
	}
	}
	deserialize();
	printf("received request of type: %c\n", res_pdu.type);
	switch(mode) {
		case 0: {
			if(res_pdu.type == 'D') {
				printf("responding to download request...\n");
			} else {
				printf("Unsupported request");
			}
		}
		break;
		case 1: {
			if(res_pdu.type = 'A') {
				printf("Acknowledgement received\n");
				mode=0;
			} else if(res_pdu.type = 'E') {
				printf("Error registering content: %s\n", res_pdu.data);
				mode = 0;
			} else {
				printf("Unsupported request");
			}
		}
	}
}

void handle_list_and_download() {
	int i, loopactive=1, j;
	char filenames[MAXFILES][11];
	req_pdu.type = 'O';
	send_udp_request();
	j = read(indx_sock, res_buffer, BUFLEN);
	if(j < 0) printf("error");
	serialize();
	printf("response recived of type: %c\n", res_pdu.type);

	memcpy(filenames, res_pdu.data, sizeof(filenames));
	for(i=0; i< MAXFILES; i++)
		printf("%d: %s\n",i, filenames[i]);
}

void handle_register_content() {
	struct sockaddr_in reg_addr;
	int sock_id, alen, loopactive=1, j=0;
	sock_id = socket(AF_INET, SOCK_STREAM, 0);
	reg_addr.sin_family = AF_INET;
	reg_addr.sin_port = htons(0);
	reg_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sock_id, (struct sockaddr *)&reg_addr, sizeof(reg_addr));
	alen = sizeof (struct sockaddr_in);
	getsockname(sock_id, (struct sockaddr *) &reg_addr, &alen);
	printf("Socket port: %d\n", reg_addr.sin_port);
	req_pdu.type = 'R';
	memcpy(req_pdu.data, peer_name , strlen(peer_name));
	memcpy(req_pdu.data+11, std_input , strlen(std_input));
	memcpy(req_pdu.data+22, ip_add , sizeof(ip_add));
	memcpy(req_pdu.data+32, &reg_addr.sin_port , sizeof(reg_addr.sin_port));
	send_udp_request();
	printf("Data send to server. Awaiting acknowledgement....\n");
	while(loopactive) {
		j = read(indx_sock, res_buffer, BUFLEN);
		if(j < 0) printf("Error");
		if(res_buffer[0] = 'A'){
			printf("Acknowledgment received\n");
			loopactive=0;
			mode=0;
		} else if(res_buffer[0] = 'E') {
			loopactive=0;
			mode=0;
			printf("File already registered");
		} 
	}
	
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

void send_udp_request() {
	serialize();
	write(indx_sock, req_buffer,sizeof(req_buffer));
	memset(req_buffer, 0, sizeof(req_buffer));
	memset(req_pdu.data, 0, sizeof(req_pdu.data));
	req_pdu.type = '\0';
}

void send_tcp_request(int socket) {
	if(req_pdu.type != 'C') {
		serialize();
		write(socket, req_buffer, BUFLEN);
	} else {
		write(socket, file_req_buffer, FILEDATABUFFLEN);
	}
}
/*------------------------------------------------------------------------
 * main - UDP client for TIME service that prints the resulting time
 *------------------------------------------------------------------------
 */
int
main(int argc, char **argv){

	struct hostent	*phe;	/* pointer to host information entry	*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, n, j, type, fds_indx, port = 3000;	/* socket descriptor and socket type	*/
	char *host = "localhost";

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

	struct pdu spdu;
	struct pdu file_pdu;
	memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;                                                                
        sin.sin_port = htons(port);


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
		printf("Can't create socket \n");

																			
/* Connect the socket */
	if (connect(indx_sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		printf("Can't connect to %s %s \n", host, "Time");

	printf("Please enter a peer name (max 10 chars): ");
	scanf("%s", peer_name);
	printf("\nPlease enter your ip: ");
	scanf("%s", ip_add);

	//add stdin to fds
	FD_ZERO(&afds);
	FD_SET(0, &afds);
	memcpy(&rfds, &afds, sizeof(rfds));


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
		for(fds_indx=1; fds_indx < FD_SETSIZE; fds_indx++) {
			if(FD_ISSET(fds_indx, &rfds)) {
				handle_socket_input(fds_indx);
			}
		}
	}

	}
	exit(0);
}
