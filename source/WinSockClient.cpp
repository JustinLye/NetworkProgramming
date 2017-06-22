#include"../include/ClientSocket.h"

int main(int argc, char* argv[]) {
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	jl::SocketRequest sr;
	sr.hints = hints;
	sr.host = (argc >= 2) ? argv[1] : "127.0.0.1";
	sr.port = (argc >= 3) ? argv[2] : "27123";
	sr.majorVerion = 2;
	sr.minorVersion = 2;
	jl::ClientSocket client(sr);
	client.Communicate();
	return 0;
}
