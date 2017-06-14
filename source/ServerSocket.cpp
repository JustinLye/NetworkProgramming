#include"../include/ServerSocket.h"

jl::ServerSocket::ServerSocket() : listenSocket(INVALID_SOCKET), acceptConn(false), shouldClose(false) {}
jl::ServerSocket::ServerSocket(const jl::SocketRequest &SocketReqInfo) : listenSocket(INVALID_SOCKET), acceptConn(false), shouldClose(false) {
	if (Initialize(SocketReqInfo) != 0)
		errorLog.LogError("Unable to construct ServerSocket due to error during initialization process.\n", __LINE__ - 1, JL_FILENAME);
}
jl::ServerSocket::~ServerSocket() {
	CloseSocket();
}


// Attempt to initialize Server Socket
int jl::ServerSocket::Initialize(const jl::SocketRequest &SocketReqInfo) {
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

	// Create socket to listen on
	listenSocket = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
	if (listenSocket == INVALID_SOCKET) { // Check if listen socket is valid
		errorLog.LogError(result = WSAGetLastError(), __LINE__ - 2, JL_FILENAME);
		freeaddrinfo(addrInfo);
		WSACleanup();
		return result;
	}
	// Bind the listening socket
	result = bind(listenSocket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		errorLog.LogError(result = WSAGetLastError(), __LINE__ - 2, JL_FILENAME);
		freeaddrinfo(addrInfo);
		closesocket(listenSocket);
		WSACleanup();
		return result;
	}
	freeaddrinfo(addrInfo);

	// Listen on socket
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		errorLog.LogError(result = WSAGetLastError(), __LINE__ - 1, JL_FILENAME);
		closesocket(listenSocket);
		WSACleanup();
		return result;
	}
	accptConnThd = std::thread(&ServerSocket::acceptingManager, std::ref(*this));
	clientMgrThd = std::thread(&ServerSocket::clientManager, std::ref(*this));

	return 0;
}

int jl::ServerSocket::CloseSocket() {
	// will need code to close down any client sockets
	shouldClose = true;
	closesocket(listenSocket); // this will also close
	WSACleanup();
	if (accptConnThd.joinable()) {
		std::unique_lock<std::mutex> locker(mu_AcceptConn);
		acceptConn = true;
		cond_acceptConn.notify_all();
		accptConnThd.join();
		locker.unlock();
	}

	if (clientMgrThd.joinable()) {
		cond_newClient.notify_all();
		clientMgrThd.join();
	}

	return 0;
}

int jl::ServerSocket::AcceptConnections() {
	std::unique_lock<std::mutex> locker(mu_AcceptConn);
	if (!acceptConn) {
		std::cout << "Server is now excepting connections...\n";
		acceptConn = true;
	}
	locker.unlock();
	cond_acceptConn.notify_one();
	return 0;
}

void jl::ServerSocket::clientWorker(SOCKET clientSocket) {
	int result;
	char rBuffer[512];
	ZeroMemory(&rBuffer, SOCKET_BUFFER_SIZE);

	do {
		result = recv(clientSocket, rBuffer, SOCKET_BUFFER_SIZE, 0);
		if (result < 0) {
			std::unique_lock<std::mutex> locker(mu_ErrorLog);
			errorLog.LogError(WSAGetLastError(), __LINE__ - 3, __FILE__);
			std::cout << "Error occurred receiving from client...\n";
			return;
		} else if (result > 0) {
			rBuffer[result] = '\0';
			std::cout << rBuffer << std::endl;
		}
	} while (result != 0);
}

void jl::ServerSocket::clientManager() {
	while (!shouldClose) {
		std::unique_lock<std::mutex> locker(mu_clientQueue);
		cond_newClient.wait(locker, [&]() { return shouldClose || !clientQueue.empty(); });
		if (!clientQueue.empty()) {
			clientThreads.push_back(std::thread(&ServerSocket::clientWorker, std::ref(*this), clientQueue.back()));
			clientQueue.pop_back();
		}
		locker.unlock();
	}

}

//This function runs on a separate thread that is initaized by ServerSocket::Initialize().
//Loops while !shouldClose.
//Waits on cond_acceptConn condition variable, predicated by acceptConn.
//A call to ServerSocket::AcceptConnections wakes this thread.
//Thread blocks on accept()
//Logs error in case of accept() failure
//Pushes newly created client socket to clientQueue deque
//Notifies threads waiting on cond_newClient condition variable.
void jl::ServerSocket::acceptingManager() {
	SOCKET clientSocket = INVALID_SOCKET;
	while (!shouldClose) {
		std::unique_lock<std::mutex> locker(mu_listenSocket);
		cond_acceptConn.wait(locker, [&]() { return acceptConn; });
		clientSocket = accept(listenSocket, NULL, NULL);
		locker.unlock();
		if (clientSocket == INVALID_SOCKET) {
			std::unique_lock<std::mutex> errLocker(mu_ErrorLog);
			std::unique_lock<std::mutex> lstLocker(mu_WSALastError);
			errorLog.LogError(WSAGetLastError(), __LINE__ - 4, JL_FILENAME);
			lstLocker.unlock();
			errLocker.unlock();
		} else {
			std::unique_lock<std::mutex> clientLocker(mu_clientQueue);
			clientQueue.push_front(clientSocket);
			clientLocker.unlock();
			cond_newClient.notify_one();
		}
	}
}
