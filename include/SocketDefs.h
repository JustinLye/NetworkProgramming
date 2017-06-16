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

#define SRVCMDIN stdin
#define BUFFER_SIZE 512
#define MAX_CLIENT_THREADS 10
#define CMD_EXIT "exit"
#define CMD_ACCEPT "accept"
#define CMD_DISCONNECT "disconnect"
#define EXIT 0x1000
#define ACCEPT 0x1001
#define DISCONNECT 0x1002
#define INVALID 0x1003


namespace jl {
	struct DLLEXPORT SocketRequest {
		struct addrinfo hints;
		PCSTR host;
		PCSTR port;
		BYTE majorVerion;
		BYTE minorVersion;
	};
	struct DLLEXPORT ClientInfo {
		SOCKET clientSocket;
		struct sockaddr_in addr;
		int addrlen;
		ClientInfo(SOCKET s, struct sockaddr_in a, int al) :
			clientSocket(s),
			addr(a),
			addrlen(al) {}
		friend std::ostream& operator<<(std::ostream& o, const ClientInfo& c) {
			char str[INET_ADDRSTRLEN];
			o << "Client Connected\nSocket: " << c.clientSocket << "\nAddress: \n";
			o << "\tsin_family: " << c.addr.sin_family << std::endl;
			o << "\tsin_port: " << c.addr.sin_port << std::endl;
			inet_ntop(c.addr.sin_family, &(c.addr.sin_addr), str, INET_ADDRSTRLEN);
			o << "\tin_addr: " << str  << std::endl;
			o << "\tsin_zero: " << c.addr.sin_zero << std::endl;
			return o;
		}
	};
	class Socket {
	protected:
		ErrorLog errorLog;
	public:
		DLLEXPORT virtual int Initialize(const SocketRequest &SocketReqInfo) = 0;
		DLLEXPORT virtual int CloseSocket() = 0;
		DLLEXPORT inline const ErrorLog &GetErrorLog() const { return errorLog; }
		DLLEXPORT inline static bool ExitRequested(const char *pbuffer) {
			if (pbuffer == nullptr || (int)strlen(pbuffer) > BUFFER_SIZE)
				return false;
			return (strncmp(pbuffer, CMD_EXIT, (int)strlen(CMD_EXIT)) == 0);
		}
		DLLEXPORT inline static bool StartAcceptingConnections(const char *pbuffer) {
			return (strncmp(pbuffer, CMD_ACCEPT, (int)strlen(CMD_ACCEPT)) == 0);
		}
		
	};
};
#endif