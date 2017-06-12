//Created by: Justin Lye
//Description: Self-Study of WinSock. Goal is to create Client, Server socket classes that can be compiled for Windows or Linux
//For WinSock I using the tutorial found here: https://msdn.microsoft.com/en-us/library/windows/desktop/ms738545(v=vs.85).aspx

#include<WinSock2.h>
#include<WS2tcpip.h>
#include<vector>
#include<deque>
#include<mutex>
#include<thread>

#pragma comment(lib, "Ws2_32.lib")
#if !defined(__JL_SOCKET_HEADER__)
#define __JL_SOCKET_HEADER__
#include"ErrorLog.h"

//modified solution taken from: https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#if !defined(JL_FILENAME)
#include<string>
#if defined(_WIN32)
#define JL_FILENAME ((strrchr(__FILE__, '\\')) ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif defined(_UNIX)
#define JL_FILENAME ((strrchr(__FILE__, '/')) ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#endif
#if !defined(DLLEXPORT)
#define DLLEXPORT __declspec(dllexport)
#endif

namespace jl {
#define DEFAULT_BUFFER_SIZE 512
#define MAX_BUFFER_SIZE 2048


	struct DLLEXPORT SocketRequest {
		struct addrinfo hints;
		PCSTR host;
		PCSTR port;
		BYTE majorVerion;
		BYTE minorVersion;
	};

	class SocketFactory {
	protected:
		ErrorLog errorLog;
	public:
		DLLEXPORT virtual int Initialize(const SocketRequest &SocketReqInfo) = 0;
		DLLEXPORT virtual int CloseSocket() = 0;
		DLLEXPORT inline const ErrorLog &GetErrorLog() const { return errorLog; }
	};

	class ServerSocket : public SocketFactory {
	protected:
		SOCKET listenSocket;
		std::deque<SOCKET> clientQueue;
		std::thread accptConnThd;
		std::thread clientMgrThd;
		std::vector<std::thread> clientThreads;
		bool acceptConn;
		bool shouldClose;
		DLLEXPORT virtual void acceptingWorker();
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
		DLLEXPORT virtual int Initialize(const SocketRequest &SocketReqInfo);
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
