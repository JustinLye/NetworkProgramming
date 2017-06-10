#include"../include/Socket.h"

int main(int argc, char* argv[]) {
	struct addrinfo hints;
	char input_buffer[DEFAULT_BUFFER_SIZE];
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	jl::ClientSocket client(hints, "127.0.0.1", "27011", 2, 2);
	client.Connect();
	client.Send("Client connected...");
	do {
		ZeroMemory(&input_buffer, DEFAULT_BUFFER_SIZE);
		fgets(input_buffer, MAX_BUFFER_SIZE - 1, stdin);
		client.Send(input_buffer);
	} while (strncmp(input_buffer, "exit", strlen("exit")) != 0);
	std::cout << client.GetErrorLog() << std::endl;
	return 0;
}