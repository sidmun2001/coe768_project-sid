/* time_server.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>

#define BUFLEN 100

const char REGISTER_REQ= 'R';
const char DOWNLOAD_REQ = 'D';
const char SEARCH_REQ = 'S';
const char DE_REGISTER_REQ = 'T';
const char LIST_REQ = 'O';
const char ACK_REQ = 'A';
const char ERROR_EQ = 'E';

struct pdu {
    char type;
    char data[99];
};

void serialize(char type,char data[99] , char buffer[BUFLEN]) {
	buffer[0] = type;
	int i;
	for(i = 0 ; i < BUFLEN-1; i++) {
		buffer[i+1] = data[i];
	}
}

void deserialize(struct pdu pdu_, char buffer[BUFLEN]) {
	pdu_.type = buffer[0];
	fprintf(stderr,"%c",buffer[0]);
	int i;
	for(i = 0; i < BUFLEN-1; i++) {
		pdu_.data[i] = buffer[i+1];
	}
}

void register_client_server(struct pdu server_info) {

}

void deregister_client_server(struct pdu server_info) {

}

struct pdu find_client_server_for_file(char fileName[100]) {

}

/*------------------------------------------------------------------------
 * Driver of Index Server
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	struct  sockaddr_in fsin;	/* the from address of a client	*/
	int	sock;			/* server socket		*/
	int	alen;			/* from-address length		*/
	struct  sockaddr_in sin; /* an Internet endpoint address         */
        int     s, type;        /* socket descriptor and socket type    */
	int 	port=3000;
	struct pdu req_pdu, res_pdu;
	char req_buffer[100], res_buffer[100], test_buf[50];
                                                                                

	switch(argc){
		case 1:
			break;
		case 2:
			port = atoi(argv[1]);
			break;
		default:
			fprintf(stderr, "Usage: %s [port]\n", argv[0]);
			exit(1);
	}

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(port);
                                                                                                 
    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
		fprintf(stderr, "can't creat socket\n");
                                                                                
    /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "can't bind to %d port\n",port);
        listen(s, 5);	
	alen = sizeof(fsin);

	while (1) {
		
		if (recvfrom(s, req_buffer, sizeof(req_buffer), 0,
				(struct sockaddr *)&fsin, &alen) < 0)
			fprintf(stderr, "recvfrom error\n");
		
			req_pdu.type = req_buffer[0];
			//fprintf(stderr,"%c",buffer[0]);
			int i;
			for(i = 0; i < BUFLEN-1; i++) req_pdu.data[i] = req_buffer[i+1];

		if(req_pdu.type == 'C') {
			fprintf(stderr,"requesting file %s", req_pdu.data);
			FILE *file = fopen(req_pdu.data, "rb");
            if (file == NULL) {
                res_pdu.type = 'E';
				strcpy(res_pdu.data,"404: File not found");
				serialize(res_pdu.type,res_pdu.data, res_buffer);
				(void) sendto(s, res_buffer, sizeof(res_buffer), 0,
				(struct sockaddr *)&fsin, sizeof(fsin));
                continue;
            }
			
			fseek(file, 0L, SEEK_END);
			int file_size = ftell(file);
			fseek(file, 0L, SEEK_SET);

			int total_bytes_sent = 0;
			int numBytes = 0;

			while( (numBytes = fread(res_pdu.data, 1, sizeof(res_pdu.data), file)) > 0){
				total_bytes_sent += numBytes;
				if(total_bytes_sent == file_size) res_pdu.type = 'F';
				else res_pdu.type = 'D';
				fprintf(stderr,"Data sent: %d\n", total_bytes_sent);
				fprintf(stderr, "Numbytes: %d\n", numBytes);
				fprintf(stderr, "type: %c\n", res_pdu.type);
				serialize(res_pdu.type,res_pdu.data, res_buffer);
				(void) sendto(s, res_buffer, sizeof(res_buffer), 0,
				(struct sockaddr *)&fsin, sizeof(fsin));
			}
			fclose(file);	
		} else {
			printf("Unsopported request");
			res_pdu.type = 'E';
			strcpy(res_pdu.data, "Unsupported request");
			serialize(res_pdu.type, res_pdu.data, res_buffer);
			(void) sendto(s, res_buffer, sizeof(res_buffer), 0,
			(struct sockaddr *)&fsin, sizeof(fsin));
		}
	}
}
