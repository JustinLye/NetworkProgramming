#include"SocketDefs.h"

#if !defined(__JL_SERVER_SOCKET_HEADER__)
#define __JL_SERVER_SOCKET_HEADER__


namespace jl {
	class ServerSocket : public Socket {
	protected:
		SOCKET listenSocket; // socket the server listens on
		std::deque<ClientInfo> clientQueue; // double-ended queue of accepted client sockets. queue is produced by clientManger() and consumed by acceptingManager()
		std::thread accptConnThd; // thread initiated in Initialize() method. this thread accepts client connections for the server
		std::thread clientMgrThd;
		std::vector<std::pair<ClientInfo,std::thread>> clientThreads;
		bool acceptConn;
		bool shouldClose;
		DLLEXPORT virtual void acceptingManager();
		DLLEXPORT virtual void clientWorker(const ClientInfo& ci);
		DLLEXPORT virtual void clientManager();
		std::mutex mu_clientQueue;
		std::mutex mu_listenSocket;
		std::mutex mu_ErrorLog;
		std::mutex mu_WSALastError;
		std::mutex mu_AcceptConn;
		std::condition_variable cond_acceptConn;
		std::condition_variable cond_newClient;
		UINT activeClients;
		LogFile log;
	public:
		DLLEXPORT ServerSocket();
		DLLEXPORT ServerSocket(const SocketRequest &SocketReqInfo);
		DLLEXPORT ~ServerSocket();
		DLLEXPORT virtual int Initialize(const SocketRequest &SocketReqInfo); // Initializes WinSock, creates socket, listens on socket, starts clientManager and acceptingWorker threads
		DLLEXPORT virtual int CloseSocket();
		DLLEXPORT virtual int AcceptConnections();
	};
};
#endif