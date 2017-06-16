#include"../include/ClientSocket.h"

jl::ClientSocket::ClientSocket() : clientSocket(INVALID_SOCKET) {}
jl::ClientSocket::ClientSocket(const SocketRequest &SocketReqInfo) : clientSocket(INVALID_SOCKET) {
	Initialize(SocketReqInfo);
}
jl::ClientSocket::~ClientSocket() {
	CloseSocket();
}

// Attempt to initialize Server Socket, Create Socket, Listen on Socket, Start clientManger and acceptingManager threads
int jl::ClientSocket::Initialize(const jl::SocketRequest &SocketReqInfo) {
	int result;
	WSADATA wsaData;
	struct addrinfo *addrInfo = nullptr;

	// Initialize WinSock
	result = WSAStartup(MAKEWORD(SocketReqInfo.majorVerion, SocketReqInfo.minorVersion), &wsaData); // Call startup function
	if (result != 0) { // Check if startup was successful
		errorLog.LogError(result, __LINE__ - 2, JL_FILENAME);
		return result;
	}

	// Resolve address and port
	result = getaddrinfo(SocketReqInfo.host, SocketReqInfo.port, &SocketReqInfo.hints, &addrInfo);
	if (result != 0) { // Check if address was successfully resolved
		errorLog.LogError(result, __LINE__ - 2, JL_FILENAME);
		WSACleanup();
		return result;
	}

	// Creat socket to communicate with server

	for (addrinfo *ptr = addrInfo; ptr != nullptr; ptr = ptr->ai_next) {
		clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (clientSocket == INVALID_SOCKET) { // Check if listen socket is valid
			errorLog.LogError(result = WSAGetLastError(), __LINE__ - 2, JL_FILENAME);
			freeaddrinfo(addrInfo);
			WSACleanup();
			return result;
		}
		// Bind the listening socket
		result = connect(clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (result == SOCKET_ERROR) {
			errorLog.LogError(result = WSAGetLastError(), __LINE__ - 2, JL_FILENAME);
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(addrInfo);

	return 0;
}

int jl::ClientSocket::CloseSocket() {
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}

int jl::ClientSocket::Communicate() {
	// not probably need to handle buffer in a different way
	char pBuffer[BUFFER_SIZE];
	int result = 0;
	do {
		std::cout << ">>> ";
		fgets(pBuffer, BUFFER_SIZE - 1, SRVCMDIN);
		result = send(clientSocket, pBuffer, strlen(pBuffer), 0);
		if (result == SOCKET_ERROR) {
			errorLog.LogError(WSAGetLastError(), __LINE__ - 2, JL_FILENAME);
			break;
		}
	} while (!ExitRequested(pBuffer));
	closesocket(clientSocket);
	return 0;
}