#include"../include/Socket.h"

#define SRVBUFSIZE 512
#define SRVCMDIN stdin
#define CMD_EXIT "exit"
#define CMD_ACCEPT "connect"
#define CMD_DISCONNECT "disconnect"
#define SRVCMD_ERRORS "errors"

void commandLoop(jl::ClientSocket *ss);


int main(int argc, char* argv[]) {
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	jl::SocketRequest sr;
	sr.hints = hints;
	sr.host = "127.0.0.1";
	sr.port = "27123";
	sr.majorVerion = 2;
	sr.minorVersion = 2;
	jl::ClientSocket client(sr);
	client.Communicate();
	std::cout << client.GetErrorLog() << std::endl;
	return 0;
}

void commandLoop(jl::ClientSocket *ss) {
	char buf[SRVBUFSIZE];

	do {
		fgets(buf, SRVBUFSIZE - 1, SRVCMDIN);
		if (strncmp(buf, SRVCMD_ERRORS, strlen(SRVCMD_ERRORS)) == 0)
			std::cout << ss->GetErrorLog() << std::endl;
	} while (strncmp(buf, CMD_EXIT, strlen(CMD_EXIT)) != 0);
}