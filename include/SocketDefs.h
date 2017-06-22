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
#include "LogFile.h"
//modified solution taken from: https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#if !defined(JL_FILENAME) // JL_FILENAME r qqemoves the directory path from __FILE__ leaving just the file name
#include<string>
#if defined(_WIN32)
#define JL_FILENAME ((strrchr(__FILE__, '\\')) ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif defined(_UNIX)
#define JL_FILENAME ((strrchr(__FILE__, '/')) ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#endif
#if defined(MAKEDLL)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

#define SRVCMDIN stdin
#define BUFFER_SIZE 512
#define MAX_CLIENT_THREADS 10
#define CMD_EXIT "exit"
#define CMD_ACCEPT "accept"
#define CMD_DISCONNECT "disconnect"
#define CMD_JRNL "journal"
#define SRV_LOGFILENAME "WinSock_server.log"
#define EXIT 0x1000
#define ACCEPT 0x1001
#define DISCONNECT 0x1002
#define INVALID 0x1003
#if !defined(GET_SYSTEM_ERROR)
#define GET_SYSTEM_ERROR(errorCode, pBuffer) FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, 0, (LPSTR)&pBuffer, 0, nullptr)
#endif
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
			inet_ntop(c.addr.sin_family, &(c.addr.sin_addr), str, INET_ADDRSTRLEN);
			o << str << ':' << c.addr.sin_port << '[' << c.clientSocket << ']';
			return o;
		}
	};
	class Socket {
	public:

		DLLEXPORT virtual int Initialize(const struct SocketRequest &SocketReqInfo) = 0;
		DLLEXPORT virtual int CloseSocket() = 0;
		DLLEXPORT inline static bool ExitRequested(const char *pbuffer) {
			if (pbuffer == nullptr || (int)strlen(pbuffer) > BUFFER_SIZE)
				return false;
			return (strncmp(pbuffer, CMD_EXIT, (int)strlen(CMD_EXIT)) == 0);
		}
		DLLEXPORT inline static bool StartAcceptingConnections(const char *pbuffer) {
			return (strncmp(pbuffer, CMD_ACCEPT, (int)strlen(CMD_ACCEPT)) == 0);
		}
		DLLEXPORT inline static bool PrintJournal(const char *pbuffer) {
			return (strncmp(pbuffer, CMD_JRNL, (int)strlen(CMD_JRNL)) == 0);
		}
	};

	struct LogMsg {
		const char* msg;
		const char* source;
		LogMsg(const char *Msg = nullptr, const char *Source = nullptr) :
			msg((Msg == nullptr) ? "NULL" : Msg),
			source((Source == nullptr) ? "NULL" : Source) {}
		
	};

	class ServerLogFile : public Log_file {
	public:
		DLLEXPORT ServerLogFile() : Log_file(SRV_LOGFILENAME) {}
		ServerLogFile(const ServerLogFile&) = delete;
		DLLEXPORT virtual void LogMessage(const struct LogMsg& Msg) { log_message(Msg.source, Msg.msg); }

	};

};
#endif