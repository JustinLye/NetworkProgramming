//Created by: Justin Lye
//Description: Self-Study of WinSock. Goal is to create Client, Server socket classes that can be compiled for Windows or Linux
//For WinSock I using the tutorial found here: https://msdn.microsoft.com/en-us/library/windows/desktop/ms738545(v=vs.85).aspx

#include<WinSock2.h>
#include<WS2tcpip.h>
#include<vector>

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



namespace jl {
#define DEFAULT_BUFFER_SIZE 512
#define MAX_BUFFER_SIZE 2048
	class Socket {
	protected:
		SOCKET _socket;
		struct addrinfo _hints;
		struct addrinfo *_result;
		PCSTR _serverName;
		PCSTR _port;
		WSADATA _wsaData;
		BYTE _majorVer;
		BYTE _minorVer;
		WORD _versionReq;
		int _lastError;
		ErrorLog _errorLog;
		LPSTR _recBuffer;
		LPSTR _sendBuffer;
		virtual bool _StartUp();
		virtual bool _ResolveServerAddr();
		virtual bool _ResolveServerAddr(struct addrinfo *r);
		virtual bool _CreateSocket(struct addrinfo *ptr);
		virtual void _Close();
		virtual int _Send(PCSTR sbuf) const = 0;
		virtual int _Receive() = 0;
		virtual int _ShutDown() = 0;


	public:
		Socket(const struct addrinfo& Hints, PCSTR ServName, PCSTR PortNum, BYTE MajorVersion = 2, BYTE MinorVersion = 2) :
			_socket(INVALID_SOCKET),
			_hints(Hints),
			_result(nullptr),
			_serverName(ServName),
			_port(PortNum),
			_majorVer(MajorVersion),
			_minorVer(MinorVersion),
			_versionReq(MAKEWORD(_majorVer, _minorVer)),
			_lastError(0),
			_recBuffer(new CHAR[DEFAULT_BUFFER_SIZE]),
			_sendBuffer(new CHAR[DEFAULT_BUFFER_SIZE]) {
			if (!_StartUp()) {
				_errorLog.LogError("Failed to construct socket.\n", __LINE__, JL_FILENAME);
				WSACleanup();
			}
		}
		~Socket() {
			if(_recBuffer != nullptr)
				delete[] _recBuffer;
			if(_recBuffer != nullptr)
				delete[] _sendBuffer;
			_Close();
		}
		inline const SOCKET &GetSocket() const { return _socket; }
		inline const ErrorLog& GetErrorLog() const { return _errorLog; }
	};

	class ClientSocket : public Socket {
	protected:
		virtual int _Send(PCSTR sbuf) const;
		virtual int _Receive() {
			return 0;
		}
		virtual int _ShutDown();

		virtual int _Connect();
	public:
		ClientSocket(const struct addrinfo& Hints, PCSTR ServName, PCSTR PortNum, BYTE MajorVersion = 2, BYTE MinorVersion = 2) :
			Socket(Hints, ServName, PortNum, MajorVersion, MinorVersion) {}
		inline virtual int Connect(PCSTR ServerName = nullptr, PCSTR PortNumber = nullptr) {
			if (ServerName != nullptr)
				_serverName = ServerName;
			if (PortNumber != nullptr)
				_port = PortNumber;
			return _Connect();
		}
		virtual int Send(PCSTR sbuf);
	};

	class ServerSocket : public Socket {
	protected:
		SOCKET _clientSocket;
		bool _shouldClose;
		virtual int _Send(PCSTR sbuf) const;
		virtual int _Receive();
		virtual int _ShutDown();
		virtual void _Close();
		virtual int _Listen();
		virtual int _Accept();
		
	public:
		ServerSocket(const struct addrinfo& Hints, PCSTR ServerName, PCSTR PortNum, BYTE MajorVersion = 2, BYTE MinorVersion = 2) :
			Socket(Hints, ServerName, PortNum, MajorVersion, MinorVersion), _clientSocket(INVALID_SOCKET), _shouldClose(false) {}
		virtual int Listen(PCSTR ServerName = nullptr, PCSTR PortNumber = nullptr) {
			if (ServerName != nullptr)
				_serverName = ServerName;
			if (PortNumber != nullptr)
				_port = PortNumber;
			return _Listen();
		}
		inline int Receive() { return _Receive(); }
		virtual int Send(PCSTR sbuf);
		virtual int Accept() { return _Accept(); }
		
		inline PCSTR GetReceiptBuffer() const { return _recBuffer; }
		inline bool ShouldClose() const { return _shouldClose; }
		virtual inline void Close() { _Close(); }
	};
};
#endif
/*
	Reference:
	Using WinSock - https://msdn.microsoft.com/en-us/library/windows/desktop/ms740632(v=vs.85).aspx
	WSAStartup - https://msdn.microsoft.com/en-us/library/windows/desktop/ms742213(v=vs.85).aspx


*/