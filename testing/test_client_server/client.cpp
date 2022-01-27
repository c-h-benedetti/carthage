#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

using SockAddr = struct sockaddr;
using SockAddrIn = struct sockaddr_in;
using String = std::string;

void process(char* buffer, uint32_t size){
	std::cout << buffer;
}

int main(int argc, char* argv[], char* env[]){

	const uint16_t SERVER_PORT = 80; // Port the server is listening on
	const uint32_t MAX_LENGTH  = 4096; // Buffer size

	int sockfd; //, n, sendbytes;
	SockAddrIn serv_addr;
	char recvline[MAX_LENGTH];

	// String literal_serv_addr = "192.168.0.34";
	String literal_serv_addr = "193.49.104.251"; // pokebip: "163.172.53.248";

	// Creating the socket

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		std::cerr << "Error while creating the socket. Stoping." << std::endl;
		return 0;
	}

	// Name the socket

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);

	if (inet_pton(AF_INET, literal_serv_addr.c_str(), &serv_addr.sin_addr) <= 0){
		std::cerr << "Error while trying to parse the provided IP address" << std::endl;
		return 0;
	}

	// Connecting to the server

	if (connect(sockfd, (SockAddr*)&serv_addr, sizeof(serv_addr)) < 0){
		std::cerr << "Failed to connect to the server" << std::endl;
		return 0;
	}

	// Requesting to the server

	String http_request = 
	"GET / HTTP/1.1\r\nHost: www.lirmm.fr:80\r\nConnection: close\r\n\r\n";
	/*memset(send_line, 0, MAX_LENGTH);
	memcpy(send_line, http_request.c_str(), http_request.length());
	sendbytes = http_request.length();*/

	if (write(sockfd, http_request.c_str(), http_request.length()) != (ssize_t)http_request.length()){
		std::cerr << "Failed to write in the socket" << std::endl;
		return 0;
	}

	int64_t extracted = 0;

	do{

		memset(recvline, 0, MAX_LENGTH);
		extracted = read(sockfd, recvline, MAX_LENGTH-1);
		process(recvline, extracted);

	}while(extracted > 0);

	std::cout << std::endl;

	close(sockfd);

	if (extracted < 0){
		std::cerr << "Error while reading from the socket" << std::endl;
		return 0;
	}

	return 0;
}

/*

	LINKS:

https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
https://www.ietf.org/rfc/rfc0959.txt

-------------------------------------------

HTTP Server: 4th layer of OSI (transport)
The Transport layer is primarily responsible for ensuring that data is transferred from one point to another reliably and without errors.
Common protocols for this layer:
	- Transmission Control Protocol (TCP)  <-- used for HTTP
	- User Datagram Protocol (UDP) 
	- Sequenced Packet Exchange (SPX)

We need to implement the Transport Layer of HTTP which is TCP.

Steps:
	1. Create the socket
	2. Identify the socket
	3. On the server, wait for an incoming connection
	4. Send and receive messages
	5. Close the socket

------------------------------------------

AF_INET: AF = Address Family ; INET = Internet
SOCK_STREAM: (opposite of datagram)
0 = Default for stream socket = TCP (there could be several protocols for stream sockets, to achieve the same result)

------------------------------------------

hton* = host to network (just to correct endianness)
s = uint16_t
l = uint32_t

------------------------------------------

[!] For requests we could use UDP rather than TCP since it's a one block instruction

*/