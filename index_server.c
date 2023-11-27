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
#define MAXNUMFILES 5
#define PEERNAMESIZE 10

const char REGISTER_REQ= 'R';
const char SEARCH_REQ = 'S';
const char DE_REGISTER_REQ = 'T';
const char LIST_REQ = 'O';
const char ACK_REQ = 'A';
const char ERROR_REQ = 'E';

char content_name_values[MAXNUMFILES][11];
char peer_name_values[MAXNUMFILES][11];
char ip_values[MAXNUMFILES][10];
in_port_t client_port_values[MAXNUMFILES];
int numClients = 0;

struct pdu req_pdu, res_pdu;
char req_buffer[100], res_buffer[100], test_buf[50];
in_port_t receiving_port;


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

int findIndexOfRecord(char peerName[10], char fileName[10]) {
	int i;
	for(i=0; i < numClients; i++) {
		if( strcmp(peer_name_values[i], peerName) && strcmp(content_name_values[i], fileName)) {
			return i;
		}
	}
	return -1;
}

struct pdu register_client_server(struct pdu req, struct sockaddr_in sockadd) {
	int i;
	struct pdu resPdu;
	char peerName[11];
	char fileName[11];
	char ip_sent[10];

	strncpy(peerName, req_pdu.data, sizeof(peerName));
	strncpy(fileName, req_pdu.data + 11, sizeof(fileName));
	strncpy(ip_sent, req_pdu.data + 22, sizeof(ip_sent));
	memcpy(&receiving_port, req.data+32, sizeof(receiving_port));

	
	fprintf(stderr, "peer name: %s\n", peerName);
	fprintf(stderr, "file name: %s\n",fileName);
	fprintf(stderr, "Port Number: %d\n", ntohs(receiving_port));
	fprintf(stderr, "IP: %s\n", ip_sent);
	if( findIndexOfRecord(peerName, fileName) == -1 && numClients < MAXNUMFILES) {
	
	numClients++;
	
	strncpy(peer_name_values[numClients], peerName, sizeof(peer_name_values[numClients]));
	strncpy(content_name_values[numClients], fileName, sizeof(content_name_values[numClients]));
	strncpy(ip_values[numClients], ip_sent, sizeof(ip_values[numClients]));
	client_port_values[numClients] = receiving_port;

	

	fprintf(stderr,"%s\n", peer_name_values[numClients]);
	fprintf(stderr,"%s\n", content_name_values[numClients]);
	fprintf(stderr,"%d\n", ntohs(client_port_values[numClients]));	
	fprintf(stderr,"%s\n", ip_values[numClients]);
	

	
	resPdu.type = 'A';

	} else {
		resPdu.type = 'E';
	}
	return resPdu;
}

struct pdu deregister_client_server(struct pdu req) {
	int i;
	struct pdu resPdu;
	char peerName[10];
	char fileName[10];
		for (i=0; i <10; i++) {
		peerName[i] = req.data[1+i];
		fileName[i] = req.data[11+i];
	}
	int index = findIndexOfRecord(peerName, fileName);
	if(index > -1) {
		for (i=0; i <10; i++) {
			peer_name_values[index][i] = '\0';
			content_name_values[index][i] = '\0';
		}
		resPdu.type = ACK_REQ;
	} else {
		resPdu.type = ERROR_REQ;
	}

	return resPdu;
}

struct pdu find_client_server_for_file(char fileName[10]) {

	int i;
	struct pdu resPdu;
	int lastIndx= -1;
	for(i = 0; i < numClients; i++) {
		if(strcmp(content_name_values[i], fileName)) {
			lastIndx = i;
		}
	}
	if(lastIndx > -1) {
		resPdu.type = SEARCH_REQ;
		strcpy(resPdu.data, client_port_values[lastIndx]);
	} else {
		resPdu.type = ERROR_REQ;
		strcpy(resPdu.data, "File not found");
	}
	return resPdu;
}

struct pdu list_files_in_library() {
	int i;
	int j;
	int total_indx = 0;
	struct pdu tmpPdu;
	tmpPdu.type = 'O';
	
	memcpy(tmpPdu.data, content_name_values, sizeof(tmpPdu.data));
	
	return tmpPdu;
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
		fprintf(stderr, "can't bind to %d port\n",port);;
        listen(s, 5);	
	alen = sizeof(fsin);
	while (1) {
		
		fprintf(stderr, "receive");
		if (recvfrom(s, req_buffer, sizeof(req_buffer), 0,
				(struct sockaddr *)&fsin, &alen) < 0)
			fprintf(stderr, "recvfrom error\n");
 			fprintf(stderr, "receiving");
		
			req_pdu.type = req_buffer[0];
			fprintf(stderr," request received: %c\n",req_buffer[0]);
			int i;
			for(i = 0; i < BUFLEN-1; i++) req_pdu.data[i] = req_buffer[i+1];
		
		switch(req_pdu.type) {
			case 'R':
				res_pdu = register_client_server(req_pdu, fsin);
				break;
				
			case 'T':
				fprintf(stderr,"request to deregister file received %s\n", req_pdu.data);
				res_pdu = deregister_client_server(req_pdu);
				continue;
			case 'S':
				fprintf(stderr,"request to search fo file received %s\n", req_pdu.data);
				res_pdu = find_client_server_for_file(req_pdu.data);
				break;
			case 'O':
				fprintf(stderr,"request to list files received: %s\n", req_pdu.data);
				res_pdu = list_files_in_library();
				break;
			case 'E':
			default:
				fprintf(stderr,"Error received %s", req_pdu.data);
				res_pdu.type = ERROR_REQ;
				strcpy(res_pdu.data, "Unsupported request");	
		}
		serialize(res_pdu.type,res_pdu.data, res_buffer);
		(void) sendto(s, res_buffer, sizeof(res_buffer), 0,
		(struct sockaddr *)&fsin, sizeof(fsin));
	}
}
