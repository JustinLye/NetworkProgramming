#include"../include/ServerSocket.h"

jl::ServerSocket::ServerSocket(const char* Log_filename) :
	Socket(),
	listenSocket(INVALID_SOCKET),
	acceptConn(false),
	shouldClose(false),
	Log(Log_filename)
{

}
jl::ServerSocket::ServerSocket(const struct jl::SocketRequest& SocketReqInfo, const char* Log_filename) : 
	Socket(),
	listenSocket(INVALID_SOCKET),
	acceptConn(false),
	shouldClose(false),
	Log(Log_filename)
{
	if (Initialize(SocketReqInfo) != 0)
		Log.log_message(__FUNCTION__, "Failed to initialize WinSock2.");
}
jl::ServerSocket::~ServerSocket() {
	CloseSocket();

}


// Attempt to initialize Server Socket
int jl::ServerSocket::Initialize(const struct jl::SocketRequest &SocketReqInfo) {
	int result;
	WSADATA wsaData;
	struct addrinfo *addrInfo = nullptr;
	char *pBuffer = nullptr;
	Log.log_message(__FUNCTION__, "New server session -- starting Initialization");

	// Initialize WinSock
	result = WSAStartup(MAKEWORD(SocketReqInfo.majorVerion, SocketReqInfo.minorVersion), &wsaData); // Call startup function
	if (result != 0) { // Check if startup was successful
		Log.log_message(__FUNCTION__, result);
		return result;
	}
	// Resolve address and port
	result = getaddrinfo(SocketReqInfo.host, SocketReqInfo.port, &SocketReqInfo.hints, &addrInfo);
	if (result != 0) { // Check if address was successfully resolved
		Log.log_message(__FUNCTION__, result);
		WSACleanup();
		return result;
	}

	// Create socket to listen on
	listenSocket = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
	if (listenSocket == INVALID_SOCKET) { // Check if listen socket is valid
		Log.log_message(__FUNCTION__, result = WSAGetLastError());
		freeaddrinfo(addrInfo);
		WSACleanup();
		return result;
	}
	// Bind the listening socket
	result = bind(listenSocket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		Log.log_message(__FUNCTION__, result = WSAGetLastError());
		freeaddrinfo(addrInfo);
		closesocket(listenSocket);
		WSACleanup();
		return result;
	}
	freeaddrinfo(addrInfo);

	// Listen on socket
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		Log.log_message(__FUNCTION__, result = WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return result;
	}
	accptConnThd = std::thread(&ServerSocket::acceptingManager, std::ref(*this));
	clientMgrThd = std::thread(&ServerSocket::clientManager, std::ref(*this));
	Log.log_message(__FUNCTION__, "Initialization successfully completed");
	return 0;
}

int jl::ServerSocket::CloseSocket() {
	// will need code to close down any client sockets
	shouldClose = true;
	closesocket(listenSocket); 
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

	for (int i = 0; i < clientThreads.size(); i++) {
		closesocket(clientThreads[i].first.clientSocket);
		if (clientThreads[i].second.joinable()) {
			clientThreads[i].second.join();
		}
	}
	Log.log_message(__FUNCTION__, "Closing server session");
	return 0;
}

int jl::ServerSocket::AcceptConnections() {
	std::unique_lock<std::mutex> locker(mu_AcceptConn);
	if (!acceptConn) {
		acceptConn = true;
		Log.log_message(__FUNCTION__, "Server is now excepting connections");
	}
	locker.unlock();
	cond_acceptConn.notify_one();
	return 0;
}

void jl::ServerSocket::clientWorker(const jl::ClientInfo& ci) {
	int result;
	char rBuffer[BUFFER_SIZE];
	ZeroMemory(&rBuffer, BUFFER_SIZE);

	while (!shouldClose) {
		do {
			result = recv(ci.clientSocket, rBuffer, BUFFER_SIZE, 0);
			if (result < 0) {
				Log.log_message(__FUNCTION__, WSAGetLastError());
				return;
			} else if (result > 0) {
				rBuffer[result] = '\0';
				std::cout << rBuffer << std::endl;
				std::ostringstream os;
				os << "Packet received from client: " << ci << "\n\t" << rBuffer << "\n\t" << "Bytes: " << result;
				Log.log_message(__FUNCTION__, os.str().c_str());
			}
		} while (result != 0);
	}
	std::ostringstream os;
	os << "Client disconnected\n\t" << ci;
	Log.log_message(__FUNCTION__, os.str().c_str());
	shutdown(ci.clientSocket, SD_BOTH);
	closesocket(ci.clientSocket);
}

// This thread starts client worker threads
// Sleeps on cond_newClient condition variable
void jl::ServerSocket::clientManager() {
	while (!shouldClose) {
		std::unique_lock<std::mutex> locker(mu_clientQueue);
		cond_newClient.wait(locker, [&]() { return shouldClose || !clientQueue.empty(); });
		if (!clientQueue.empty()) {
			clientThreads.push_back(std::pair<ClientInfo, std::thread>(clientQueue.back(), std::thread(&ServerSocket::clientWorker, std::ref(*this), clientQueue.back())));
			std::ostringstream os;
			os << "New client connection\n\t" << clientQueue.back();
			Log.log_message(__FUNCTION__, os.str().c_str());
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
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	while (!shouldClose) {
		std::unique_lock<std::mutex> locker(mu_listenSocket);
		cond_acceptConn.wait(locker, [&]() { return acceptConn; });
		clientSocket = accept(listenSocket, (struct sockaddr*)&addr, &addrlen);
		locker.unlock();
		if (clientSocket == INVALID_SOCKET) {
			std::unique_lock<std::mutex> locker2(mu_ErrorLog);
			Log.log_message(__FUNCTION__, WSAGetLastError());
			locker2.unlock();
		} else {
			std::unique_lock<std::mutex> clientLocker(mu_clientQueue);
			clientQueue.push_front(ClientInfo(clientSocket, addr, addrlen));
			clientLocker.unlock();
			cond_newClient.notify_one();
		}
	}
}
