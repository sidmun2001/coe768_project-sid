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

void serialize(char type,  char data[99], char buffer[BUFLEN]) {
	buffer[0] = type;
	int i;
	for(i = 0 ; i < BUFLEN-1; i++) buffer[i+1] = data[i];
}

void deserialize(struct pdu data, char buffer[BUFLEN]) {
	data.type = buffer[0];
	int i;
	for(i = 0; i < BUFLEN-1; i++) data.data[i] = buffer[i+1];
}

/*------------------------------------------------------------------------
 * main - UDP client for TIME service that prints the resulting time
 *------------------------------------------------------------------------
 */
int
main(int argc, char **argv)
{
	char *host = "localhost";
	int	port = 3000, active=1;
	struct hostent	*phe;	/* pointer to host information entry	*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, n, j, type;	/* socket descriptor and socket type	*/
	char req_buffer[101], res_buffer[101];

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
                                                                                        
    /* Map host name to IP address, allowing for dotted decimal */
        if ( phe = gethostbyname(host) ){
                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
        }
        else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
		fprintf(stderr, "Can't get host entry \n");
                                                                                
    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
		fprintf(stderr, "Can't create socket \n");
	
                                                                                
    /* Connect the socket */
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "Can't connect to %s %s \n", host, "Time");

		int mode;
		while(active==1) {
			printf("Welcome to Our COE768 Lab4. Please Select an option below. \n");
			printf("1. Download a file. \n2. Exit. \n");
			scanf("%d",&mode);
			if(mode == 1) {
				FILE *clientfileptr;
				spdu.type = 'C'; // Set the type to FILENAME PDU
				printf("Please enter the filename: ");
				scanf("%s", spdu.data);
				serialize(spdu.type, spdu.data, req_buffer);
				write(s, req_buffer, sizeof(req_buffer));
				clientfileptr = fopen(spdu.data, "wb");
				int total_bytes_received = 0;
				int loopend=1;
				while (loopend == 1) { /*repeatedly reading until termination*/
				j = read(s, res_buffer, BUFLEN);
					if(j <0 ){
						printf("Error");
						close(s);
					}
					//deserialize(file_pdu, res_buffer);
					file_pdu.type = res_buffer[0];
					int i;
					for(i = 0; i < BUFLEN-1; i++) file_pdu.data[i] = res_buffer[i+1];
					switch(file_pdu.type) {
						case 'D':
							fwrite(file_pdu.data, 1, j, clientfileptr);
							printf("%d bytes written", j);
							total_bytes_received += j;
						break;
						case 'F':
							fwrite(file_pdu.data, 1, j, clientfileptr);
							fclose(clientfileptr);
							printf("%d bytes written", j);
							total_bytes_received += j;
							printf("File received. Total Bytes written: %d\n",total_bytes_received);
							loopend=0;
						break;
						case 'E':
							printf("Error receiving file");
							loopend=0;
						break;
						default:
							printf("filname not supported");
							loopend=0;
						break;
						
					}
				}
			} else if(mode == 2) {
				printf("Thank you for using. \n");
				active=0;
			} else {
				printf("Invalid Choice.\n");
			}
		}
	exit(0);
}
