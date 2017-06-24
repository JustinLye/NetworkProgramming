#include"../include/ServerSocket.h"

jl::ServerSocket::ServerSocket(const char* Log_filename) :
	Listen_socket(INVALID_SOCKET),
	Accept_connection(false),
	Should_close(false),
	Log(Log_filename)
{

}

jl::ServerSocket::ServerSocket(const struct jl::SocketRequest& SocketReqInfo, const char* Log_filename) : 
	Listen_socket(INVALID_SOCKET),
	Accept_connection(false),
	Should_close(false),
	Log(Log_filename)
{
	if (initialize(SocketReqInfo) != 0)
		Log.log_message(APP_ERROR_MESSAGE("Failed to initialize WinSock2!"));
}
jl::ServerSocket::~ServerSocket() {
	close_socket();

}


// Attempt to initialize Server Socket
int jl::ServerSocket::initialize(const struct jl::SocketRequest &SocketReqInfo) {
	int result;
	WSADATA wsaData;
	struct addrinfo *addrInfo = nullptr;
	char *pBuffer = nullptr;
	Log.log_message(INFO_MESSAGE("New server session -- starting Initialization"));

	// Initialize WinSock
	result = WSAStartup(MAKEWORD(SocketReqInfo.majorVerion, SocketReqInfo.minorVersion), &wsaData); // Call startup function
	if (result != 0) { // Check if startup was successful
		Log.log_message(SYS_ERROR_MESSAGE(result));
		return result;
	}
	// Resolve address and port
	result = getaddrinfo(SocketReqInfo.host, SocketReqInfo.port, &SocketReqInfo.hints, &addrInfo);
	if (result != 0) { // Check if address was successfully resolved
		Log.log_message(SYS_ERROR_MESSAGE(result));
		WSACleanup();
		return result;
	}

	// Create socket to listen on
	Listen_socket = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
	if (Listen_socket == INVALID_SOCKET) { // Check if listen socket is valid
		Log.log_message(SYS_ERROR_MESSAGE(result = WSAGetLastError()));
		freeaddrinfo(addrInfo);
		WSACleanup();
		return result;
	}
	// Bind the listening socket
	result = bind(Listen_socket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		Log.log_message(SYS_ERROR_MESSAGE(result = WSAGetLastError()));
		freeaddrinfo(addrInfo);
		closesocket(Listen_socket);
		WSACleanup();
		return result;
	}
	freeaddrinfo(addrInfo);

	// Listen on socket
	if (listen(Listen_socket, SOMAXCONN) == SOCKET_ERROR) {
		Log.log_message(SYS_ERROR_MESSAGE(result = WSAGetLastError()));
		closesocket(Listen_socket);
		WSACleanup();
		return result;
	}
	Accept_connection_thread = std::thread(&ServerSocket::Accepting_manager, std::ref(*this));
	Client_manager_thread = std::thread(&ServerSocket::Client_manager, std::ref(*this));
	Log.log_message(INFO_MESSAGE("Initialization successfully completed"));
	return 0;
}

int jl::ServerSocket::close_socket() {
	// will need code to close down any client sockets
	Should_close = true;
	closesocket(Listen_socket); 
	WSACleanup();
	if (Accept_connection_thread.joinable()) {
		std::unique_lock<std::mutex> locker(Accept_connection_mux);
		Accept_connection = true;
		Accept_connect_condition.notify_all();
		Accept_connection_thread.join();
		locker.unlock();
	}

	if (Client_manager_thread.joinable()) {
		New_client_condition.notify_all();
		Client_manager_thread.join();
	}

	for (int i = 0; i < Client_threads.size(); i++) {
		closesocket(Client_threads[i].first.clientSocket);
		if (Client_threads[i].second.joinable()) {
			Client_threads[i].second.join();
		}
	}
	Log.log_message(INFO_MESSAGE("Closing server session"));
	return 0;
}

int jl::ServerSocket::accept_connections() {
	std::unique_lock<std::mutex> locker(Accept_connection_mux);
	if (!Accept_connection) {
		Accept_connection = true;
		Log.log_message(INFO_MESSAGE("Server is now excepting connections"));
	}
	locker.unlock();
	Accept_connect_condition.notify_one();
	return 0;
}

void jl::ServerSocket::Client_worker(const jl::ClientInfo& ci) {
	int result;
	char rBuffer[BUFFER_SIZE];
	ZeroMemory(&rBuffer, BUFFER_SIZE);

	while (!Should_close) {
		do {
			result = recv(ci.clientSocket, rBuffer, BUFFER_SIZE, 0);
			if (result < 0) {
				Log.log_message(SYS_ERROR_MESSAGE(WSAGetLastError()));
				return;
			} else if (result > 0) {
				rBuffer[result] = '\0';
				std::cout << rBuffer << std::endl;
				std::ostringstream os;
				os << "Packet received from client: " << ci << "\n\t" << rBuffer << "\n\t" << "Bytes: " << result;
				Log.log_message(INFO_MESSAGE(os.str().c_str()));
			}
		} while (result != 0);
	}
	std::ostringstream os;
	os << "Client disconnected\n\t" << ci;
	Log.log_message(INFO_MESSAGE(os.str().c_str()));
	shutdown(ci.clientSocket, SD_BOTH);
	closesocket(ci.clientSocket);
}

// This thread starts client worker threads
// Sleeps on cond_newClient condition variable
void jl::ServerSocket::Client_manager() {
	while (!Should_close) {
		std::unique_lock<std::mutex> locker(Client_queue_mux);
		New_client_condition.wait(locker, [&]() { return Should_close || !Client_queue.empty(); });
		if (!Client_queue.empty()) {
			Client_threads.push_back(std::pair<ClientInfo, std::thread>(Client_queue.back(), std::thread(&ServerSocket::Client_worker, std::ref(*this), Client_queue.back())));
			std::ostringstream os;
			os << "New client connection\n\t" << Client_queue.back();
			Log.log_message(INFO_MESSAGE(os.str().c_str()));
			Client_queue.pop_back();
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
void jl::ServerSocket::Accepting_manager() {
	SOCKET clientSocket = INVALID_SOCKET;
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	while (!Should_close) {
		std::unique_lock<std::mutex> locker(Listen_socket_mux);
		Accept_connect_condition.wait(locker, [&]() { return Accept_connection; });
		clientSocket = accept(Listen_socket, (struct sockaddr*)&addr, &addrlen);
		locker.unlock();
		if (clientSocket == INVALID_SOCKET) {
			std::unique_lock<std::mutex> locker2(Error_log_mux);
			Log.log_message(SYS_ERROR_MESSAGE(WSAGetLastError()));
			locker2.unlock();
		} else {
			std::unique_lock<std::mutex> clientLocker(Client_queue_mux);
			Client_queue.push_front(ClientInfo(clientSocket, addr, addrlen));
			clientLocker.unlock();
			New_client_condition.notify_one();
		}
	}
}
