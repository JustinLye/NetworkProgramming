#include"../include/Socket.h"

#define SRVBUFSIZE 512
#define SRVCMDIN stdin
#define SRVCMD_EXIT "exit"
#define SRVCMD_CONNECT "connect"
#define SRVCMD_DISCONNECT "disconnect"

void commandLoop(jl::ServerSocket *ss);

int main() {
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	jl::SocketRequest sr;
	sr.hints = hints;
	sr.host = "127.0.0.1";
	sr.port = "27123";
	sr.majorVerion = 2;
	sr.minorVersion = 2;

	jl::ServerSocket ss(sr);
	commandLoop(&ss);
	std::cout << ss.GetErrorLog() << std::endl;

	return 0;
}

void commandLoop(jl::ServerSocket *ss) {
	char buf[SRVBUFSIZE];
	
	do {
		fgets(buf, SRVBUFSIZE - 1, SRVCMDIN);
		if (strncmp(buf, SRVCMD_CONNECT, strlen(SRVCMD_CONNECT)) == 0) {
			ss->AcceptConnections();
		}
	} while (strncmp(buf, SRVCMD_EXIT, strlen(SRVCMD_EXIT)) != 0);
}