#include<WinSock2.h>
#include<WS2tcpip.h>
#include<vector>
#include<deque>
#include<mutex>
#include<thread>

#pragma comment(lib, "Ws2_32.lib")
#if !defined(__JL_SOCKET_DEFS_HEADER__)
#define __JL_SOCKET_DEFS_HEADER__
#include "ErrorLog.h"
//modified solution taken from: https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#if !defined(JL_FILENAME) // JL_FILENAME r qqemoves the directory path from __FILE__ leaving just the file name
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

#define SOCKET_BUFFER_SIZE 512
#define SRVCMDIN stdin
#define CMD_EXIT "exit"
#define CMD_ACCEPT "accept"
#define CMD_DISCONNECT "disconnect"
namespace jl {
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
};
#endif