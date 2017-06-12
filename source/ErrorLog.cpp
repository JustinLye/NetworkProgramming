#include"../include/ErrorLog.h"
jl::Error::Error() : _message(nullptr), _lineNum(0), _fileName(nullptr) { _BuildMessageStream();  }
jl::Error::Error(const Error &other) : _message(other._message), _lineNum(other._lineNum), _fileName(other._fileName) { _BuildMessageStream();  }
jl::Error::Error(LPSTR Msg, int LineNum, LPSTR FileName) :
	_message(Msg),
	_lineNum(LineNum),
	_fileName(FileName) {
	_BuildMessageStream();
}

void jl::Error::_BuildMessageStream() {
	_msgStream.clear(); // clear current string stream
	// Build up error string inserting UNKNOWN where values are missing

	_msgStream << "Error: ";
	if (_fileName != nullptr)
		_msgStream << _fileName;
	else
		_msgStream << "UNKNOWN FILE";
	_msgStream << " [";
	if (_lineNum > 0)
		_msgStream << _lineNum << "]\n";
	else
		_msgStream << "UNKNOWN LINE]\n";
	if (_message != nullptr)
		_msgStream << _message;
	else
		_msgStream << "UNKNOWN ERROR\n";
	
}

void jl::Error::SetError(LPSTR Msg, int LineNum, LPSTR FileName) {
	_message = Msg;
	_lineNum = LineNum;
	_fileName = FileName;
	_BuildMessageStream();
}

void jl::ErrorLog::LogError(const DWORD &ErrorCode, int LineNum, LPSTR FileName) {
	LPSTR pBuffer = nullptr; // Buffer for format message result
	int err = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ErrorCode, 0, (LPSTR)&pBuffer, 0, nullptr); // Search for system error message
	_LogError(Error(pBuffer, LineNum, FileName)); // Log error
}

void jl::ErrorLog::LogError(const LPSTR ErrorMsg, int LineNum, LPSTR FileName) {
	_LogError(Error(ErrorMsg, LineNum, FileName));
}