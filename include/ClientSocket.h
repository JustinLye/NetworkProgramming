#if !defined(__JL_CLIENT_SOCKET_HEADER__)
#define __JL_CLIENT_SOCKET_HEADER__
#include"SocketDefs.h"

namespace jl {
	class ClientSocket : public Socket {
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