#include"../include/ServerSocket.h"


void commandLoop(jl::ServerSocket *ss);

int main(int argc, char* argv[]) {
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	jl::SocketRequest sr;
	sr.hints = hints;
	sr.host = (argc >= 2) ? argv[1] : "127.0.0.1";
	sr.port = (argc >= 3) ? argv[2] : "27123";
	sr.majorVerion = 2;
	sr.minorVersion = 2;

	jl::ServerSocket ss(sr);
	commandLoop(&ss);
	std::cout << ss.GetErrorLog() << std::endl;

	return 0;
}

void commandLoop(jl::ServerSocket *ss) {
	char buf[BUFFER_SIZE];
	do {
		fgets(buf, BUFFER_SIZE - 1, SRVCMDIN);
		if (jl::Socket::StartAcceptingConnections(buf)) {
			ss->AcceptConnections();
		}
	} while (!jl::Socket::ExitRequested(buf));
}