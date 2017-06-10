#include"../include/Socket.h"

int main() {
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	jl::ServerSocket server(hints, "127.0.0.1", "27011", 2, 2);
	
	server.Listen();
	std::cout << "Server is listening...\n";
	server.Accept();
	std::cout << "Server accepted client...\n";
	do {
		server.Receive();
		std::cout << server.GetReceiptBuffer();

	} while (!server.ShouldClose() && server.GetReceiptBuffer() != "exit");
	std::cout << server.GetErrorLog() << std::endl;
	return 0;
}