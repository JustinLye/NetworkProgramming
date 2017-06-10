#include"../include/Socket.h"

bool jl::Socket::_StartUp() {
	// initialize use of Winsock DLL by a process
	if ((_lastError = WSAStartup(_versionReq, &_wsaData)) != 0) {
		_errorLog.LogError(_lastError, __LINE__ - 1, JL_FILENAME);
		return false; // possible error codes https://msdn.microsoft.com/en-us/library/windows/desktop/ms742213(v=vs.85).aspx
	}

	// Check if WinSock DLL supports version
	if (HIBYTE(_wsaData.wVersion) != _majorVer ||
		LOBYTE(_wsaData.wVersion) != _minorVer) {
		_errorLog.LogError("WinSock DLL does not match requested version\n", __LINE__ - 1, JL_FILENAME);
		return false;
	}
	return true;
}

bool jl::Socket::_ResolveServerAddr() {
	// Resolve server address and port
	if ((_lastError = getaddrinfo(_serverName, _port, &_hints, &_result)) != 0) {
		_errorLog.LogError(_lastError, __LINE__ - 1, JL_FILENAME);
		return false;
	}
	return true;
}

bool jl::Socket::_ResolveServerAddr(struct addrinfo* r) {
	if ((_lastError = getaddrinfo(_serverName, _port, &_hints, &r)) != 0) {
		_errorLog.LogError(_lastError, __LINE__ - 1, JL_FILENAME);
		return false;
	}
	return true;
}

bool jl::Socket::_CreateSocket(struct addrinfo *ptr) {
	if (ptr == nullptr) {
		_errorLog.LogError("Null pointer passed to _CreateSocket().\n", __LINE__, JL_FILENAME);
		return false;
	}
	_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (_socket == INVALID_SOCKET) {
		_lastError = WSAGetLastError();
		_errorLog.LogError(_lastError, __LINE__ - 3, JL_FILENAME);
		WSACleanup();
		return false;
	}
	return true;
}

void jl::Socket::_Close() {
	freeaddrinfo(_result);
	closesocket(_socket);
	WSACleanup();
}

int jl::ClientSocket::_Send(PCSTR sbuf) const {
	int len = (int)strlen(sbuf);
	if (len > MAX_BUFFER_SIZE)
		return ERR_EXCEEDS_MAX_BUFFER_SIZE;
	if (send(_socket, sbuf, len, 0) == SOCKET_ERROR) 
		return WSAGetLastError();
	return 0;
}

int jl::ClientSocket::Send(PCSTR sbuf) {
	int result = _Send(sbuf);
	if (result != 0)
		if (result == ERR_EXCEEDS_MAX_BUFFER_SIZE)
			_errorLog.LogError("Length of send buffer exceeds the maximum size allowed.\n", __LINE__, JL_FILENAME);
		else
			_errorLog.LogError(result, __LINE__, JL_FILENAME);
		_lastError = result;
	return result;
}

int jl::ClientSocket::ClientSocket::_ShutDown() {
	if (shutdown(_socket, SD_BOTH) == SOCKET_ERROR) {
		_lastError = WSAGetLastError();
		_errorLog.LogError(_lastError, __LINE__ - 2, JL_FILENAME);
		return _lastError;
	}
	return 0;
}

int jl::ClientSocket::_Connect() {
	if (!_ResolveServerAddr()) {
		_errorLog.LogError("Unable to connect to server. Problem resolving server address and port.\n", __LINE__, JL_FILENAME);
		return _lastError;
	}
	for (struct addrinfo *ptr = _result; ptr != nullptr; ptr = ptr->ai_next) {
		if (!_CreateSocket(ptr)) {
			_errorLog.LogError("Unable to connect to server. Problem creating socket.\n", __LINE__ - 1, JL_FILENAME);
			return _lastError;
		}
		if (connect(_socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
			_lastError = WSAGetLastError();
			_errorLog.LogError(_lastError, __LINE__ - 2, JL_FILENAME);
			closesocket(_socket);
			_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(_result);
	_result = nullptr;
	if (_socket == INVALID_SOCKET) {
		_errorLog.LogError("Unable to connect to server.\n", __LINE__, JL_FILENAME);
		return _lastError;
		WSACleanup();
	}
	return 0;
}


void jl::ServerSocket::_Close() {
	if (_result != nullptr)
		freeaddrinfo(_result);
	if(_clientSocket != INVALID_SOCKET)
		closesocket(_clientSocket);
	if(_socket != INVALID_SOCKET)
		closesocket(_socket);
	WSACleanup();
}

int jl::ServerSocket::_Listen() {
	if (!_ResolveServerAddr()) {
		_errorLog.LogError("Unable to open connection. Problem resolving address and port.\n", __LINE__ - 1, JL_FILENAME);
		return _lastError;
	}

	if (!_CreateSocket(_result)) {
		_errorLog.LogError("Unable to bind socket. Problem creating socket.\n", __LINE__ - 1, JL_FILENAME);
		return _lastError;
	}

	if ((bind(_socket, _result->ai_addr, (int)_result->ai_addrlen) == SOCKET_ERROR)) {
		_lastError = WSAGetLastError();
		_errorLog.LogError(_lastError, __LINE__ - 2, JL_FILENAME);
		return _lastError;
	}
	freeaddrinfo(_result);
	_result = nullptr;
	if (listen(_socket, SOMAXCONN) == SOCKET_ERROR) {
		_lastError = WSAGetLastError();
		_errorLog.LogError(_lastError, __LINE__ - 2, JL_FILENAME);
		return _lastError;
	}
	return 0;
}

int jl::ServerSocket::_Accept() {
	_clientSocket = accept(_socket, NULL, NULL);
	if (_clientSocket == INVALID_SOCKET) {
		_lastError = WSAGetLastError();
		_errorLog.LogError(_lastError, __LINE__ - 3, JL_FILENAME);
		return _lastError;
	}
	return 0;
}

int jl::ServerSocket::_Receive() {
	int recStatus = recv(_clientSocket, _recBuffer, DEFAULT_BUFFER_SIZE, 0);
	if (recStatus == 0) {
		_shouldClose = true;
	} else if(recStatus < 0) {
		_lastError = WSAGetLastError();
		_errorLog.LogError(_lastError, __LINE__ - 7, JL_FILENAME);
		_shouldClose = true;
		return _lastError;
	} else {
		_recBuffer[recStatus] = '\0';
	}
	return 0;
}

int jl::ServerSocket::_ShutDown() {
	if (shutdown(_clientSocket, SD_SEND) == SOCKET_ERROR) {
		_lastError = WSAGetLastError();
		_errorLog.LogError(_lastError, __LINE__ - 2, JL_FILENAME);
		return _lastError;
	}
	return 0;
}

int jl::ServerSocket::_Send(PCSTR sbuf) const {
	int len = (int)strlen(sbuf);
	if (len > MAX_BUFFER_SIZE)
		return ERR_EXCEEDS_MAX_BUFFER_SIZE;
	if (send(_clientSocket, sbuf, len, 0) == SOCKET_ERROR) {
		return WSAGetLastError();
	}
	return 0;
}
int jl::ServerSocket::Send(PCSTR sbuf) {
	int result = _Send(sbuf);
	if (result != 0) {
		if (result == ERR_EXCEEDS_MAX_BUFFER_SIZE) { // Temporary, will move user-defined error code logging to ErrorLog in the future
			_errorLog.LogError("Length of send buffer exceeds the maximum size allowed.\n", __LINE__, JL_FILENAME);
		} else {
			_lastError = result;
			_errorLog.LogError(_lastError, __LINE__, JL_FILENAME);
		}
		return result;
	}
	return result;

}