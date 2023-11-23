# CPS714 P2P Network Project

## Project Summary
In this project we are developing a file sharing service using TCP and UDP connections. This service will feature one index server which manages a list of clients and their files. The index server will allow clients to register and deregister files available for download and also request a list of files available for download and also search for files available for download in which ase the address of serving clients will be forwarded to the downloading client. This project will utilize a mix of TCP and UDP connections.

Each request will utilize a request header indicating what type of action is to be taken followed by a data packet. The data packet will contain information in varying formats depending on the request header. In this way we are defining our own protocol, and must ensure that all members adhere to this protocol.

## User Interface
Here we describe the various ways that users can use this service.

### Index Server
Since the index server only receives and processes requests it has no UI, but will print out information via the command line so that users can monitor server functionality.

### Client Server
Client server will be usable via the command line by text-based input.

## Process Outline.
This outline showcases the process of how our service is intended to be used and assumes that the index server and all client servers are initialized with no errors and empty lists of data.

1. **Users run client server for first time**
    1. Users prompted for server name (use unique port number on computer.)
    2. Main menu screen displayed.
2. **Client server 1 registers file to server: User selects registration option.**
    1. User prompted for file name input
    2. Client server 1 creates TCP socket for file.
    3. Client server 1 sends out registration request.
    4. Index server receives request and sends acknlowledgement.
3. **Client server 2 views list of available files: User selects browse option**
    1. Client server 2 sends out request to index server to view list of files available for download.
    2. Index server returns list of files to client server 2.
    3. Client server 2 displays list to user.
4. **Client server 2 downloads file: User selects one of the files.**
    1. Client server 2 sends request to receive download adddress for a file.
    2. Index server searches for file name and selects the least used client server containing this file. Sends address of client server 1 to client server 2
    3. Client server 2 receives the address from index server and initiates TCP connection with Client server 1.
    4. Client server 1 esablished TCP connection and begins sending data to client server 1.
    5. Client server 2 continues to receive data from client server 1 and saves file until complete or error is received.
    6. TCP connection is broken when file has been fully received or error is received.
5. **Client server 2 registers file with index server: No user action necessary, automatic process**
    1. Client server 2 creates TCP socket for new file.
    2. Client server 2 sends registration request to index server.
    3. Index server receives request and sends acknowledgement.
6. **Client server (1 or 2) quits shell: User selects quit option**
    1. Client server sends request to deregister all files.
    2. Client server closes TCP sockets when deregistration is complete.
    3. Client server quits shell.

## Index Server Functionality

- All connections to the index from a peer will utilize the UDP standard.
- Index server will typically send out a response to each request which will be either an Error respone or as specified below.

### Registering File from client
- **Action**: Registers Client as server of filename.
- **Protocol**: UDP
- **Request Header**: R
- **Data Size Total**: 99 bytes
- **Request Data**: Peer Name (10 bytes), File Name (10 bytes), Address containing ip address and port number (79 bytes).
- **Returns**: Acknowledgement Packet
- **Error Case**: If entry containing peer name and filename already exists.


### Deregistering file from client
- **Action**: Removes registered client from list.
- **Protocol**: UDP
- **Request Header**: T
- **Data Size Total**: 20 bytes
- **Request Data**: Peer Name(10 bytes), File Name (10 bytes).
- **Returns**: Acknowledgement Packet
- **Error Case**: No content matching peer name and file name exists.

### Listing available content
- **Action**: Sends a list of all available content to peer.
- **Protocol**: UDP
- **Request Header**: O
- **Data Size total**: 0 bytes
- **Request Data**: Null.
- **Returns**: O type response of the form: "Content name 1","Content name 2"..." containing maximum 9 entries (10 bytes each).

### Content Download Request
- **Action**: Searches for content by content name.
- **Protocol**: UDP
- **Request Header**: S
- **Data Size total**: 10 bytes
- **Request Data**: Content Name: (10 bytes).
- **Returns**: S type response with format: Address(79 bytes). Must select least used content server.
- **Error Case**: No content found with peer name and content name.

## Client Server Functionality
- Client server communicates with both index server and other client servers.
- Client server will utilize both UDP and TCP connections, typically UDP with index and TCP with other clients.
- Client server can retreive files from other client servers and serve files to other client servers.
- Client server should look out for an E type response on all requests and should print out error message to user.

### Outbound Requests
- Here we describe the type of requests which can be sent out by the client server and what responses they expect. 
- It is possibly for each and every response here to be returned with an error response and so this must be handled elegantly.
- It is possible that the client may have some actions to do granted if these requests are successful.

#### Registering content
- **Action**: Sends register content request to index server
- **Protocol**: UDP
- **Request Header**: R
- **Data Size total**: 99 bytes
- **Request Data**: Peer Name (10 bytes), content name (10 bytes), address of corresponding TCP port for this specific file.(79 bytes)
- **Expects**: A type response.
- **Following actions**: None

#### Deregistering Content
- **Action**: Sends request to deregister content on index server
- **Protocol**: UDP
- **Request Header**: T
- **Data Size Total**: 20 bytes
- **Request Data**: Peer Name (10 bytes), Content name(10 bytes)
- **Expects**: A type response.
- **Following actions**: None

#### Listing Available Content
- **Action**: Sends request to view list of content
- **Protocol**: UDP
- **Request Header**: O
- **Data Size total**: 0 bytes
- **Request Data**: Null
- **Expects**: O type response of format: "Content Name 1","Content Name 2",... of maximum size 99 bytes.
- **Following Actions**: None

#### Search for content
- **Action**: Sends request to find specific content by file name
- **Protocol** UDP
- **Request Header**: S
- **Data Size total**: 10 bytes
- **Request Data**: Content Name(10 bytes)
- **Expects**: S type respone of format: Client Address (79 bytes)
- **Following Actions**: Send download request to client

#### Download data from client
- **Action**: Sends request to client to download content
- **Protocol**: TCP
- **Request Header**: D
- **Data Size total**: 10 bytes
- **Request Data**: Content Name(10 bytes)
- **Expects**: multiple C type response of format: File Data(max 1639 bytes). A packet less than 1639 bytes indicates the last packet.
- **Following Actions**: Register content with index server.

### Incoming Requests
- Here we describe the requests which a client can expect to come in.
- Client must handle these requests or otherwise send out an Error response.

#### Content Download
- **Action**: Begin transmitting data to user.
- **Protocol**: TCP
- **Request Header**: D
- **Data Size total**: 10 bytes
- **Request Data**: Content Name (10 bytes)
- **Returns**: A series of C type packets containing file data (max 1639 bytes). a packet less than 1639 bytes indicates the last packet.
- **Error Case**: File does not exist.
