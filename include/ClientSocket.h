#if !defined(__JL_CLIENT_SOCKET_HEADER__)
#define __JL_CLIENT_SOCKET_HEADER__
#include"SocketDefs.h"

namespace jl {
	class ClientSocket : public Socket {
	protected:
		SOCKET clientSocket;
		Log_file Log;
	public:
		DLLEXPORT ClientSocket(const char* Log_filename = "WinSock_client.log");
		DLLEXPORT ClientSocket(const SocketRequest &SocketReqInfo, const char* Log_filename = "WinSock_client.log");
		DLLEXPORT ~ClientSocket();
		DLLEXPORT virtual int Initialize(const SocketRequest &SocketReqInfo);
		DLLEXPORT virtual int CloseSocket();
		DLLEXPORT virtual int Communicate();
	};
};
#endif