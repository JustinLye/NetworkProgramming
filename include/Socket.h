//Created by: Justin Lye
//Description: Self-Study of WinSock. Goal is to create Client, Server socket classes that can be compiled for Windows or Linux
//For WinSock I using the tutorial found here: https://msdn.microsoft.com/en-us/library/windows/desktop/ms738545(v=vs.85).aspx


#include<vector>
#include<deque>
#include<mutex>
#include<thread>


#if !defined(__JL_SOCKET_HEADER__)
#define __JL_SOCKET_HEADER__
#include"SocketDefs.h"


namespace jl {

	class ServerSocket : public SocketFactory {
	protected:
		SOCKET listenSocket; // socket the server listens on
		std::deque<SOCKET> clientQueue; // double-ended queue of accepted client sockets. queue is produced by clientManger() and consumed by acceptingManager()
		std::thread accptConnThd; // thread initiated in Initialize() method. this thread accepts client connections for the server
		std::thread clientMgrThd;
		std::vector<std::thread> clientThreads;
		bool acceptConn;
		bool shouldClose;
		DLLEXPORT virtual void acceptingManager();
		DLLEXPORT virtual void clientWorker(SOCKET clientSocket);
		DLLEXPORT virtual void clientManager();
		std::mutex mu_clientQueue;
		std::mutex mu_listenSocket;
		std::mutex mu_ErrorLog;
		std::mutex mu_WSALastError;
		std::mutex mu_AcceptConn;
		std::condition_variable cond_acceptConn;
		std::condition_variable cond_newClient;
	public:
		DLLEXPORT ServerSocket();
		DLLEXPORT ServerSocket(const SocketRequest &SocketReqInfo);
		DLLEXPORT ~ServerSocket();
		DLLEXPORT virtual int Initialize(const SocketRequest &SocketReqInfo); // Initializes WinSock, creates socket, listens on socket, starts clientManager and acceptingWorker threads
		DLLEXPORT virtual int CloseSocket();
		DLLEXPORT virtual int AcceptConnections();
	};

	class ClientSocket : public SocketFactory {
	protected:
		SOCKET clientSocket;

	public:
		DLLEXPORT ClientSocket();
		DLLEXPORT ClientSocket(const SocketRequest &SocketReqInfo);
		DLLEXPORT ~ClientSocket();
		DLLEXPORT virtual int Initialize(const SocketRequest &SocketReqInfo);
		DLLEXPORT virtual int CloseSocket();
		DLLEXPORT virtual int Communicate();
	};
};
#endif
