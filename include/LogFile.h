#include<iostream>
#include<fstream>
#include<mutex>
#include<string>
#include<sstream>
#include<ctime>
#include<stdexcept>
#include<iomanip>
#include<Windows.h>
#define MSG_DIV "----------------------------------------------------"
#define MSG_DIV2 "*******************************************************************************************************"
#define MSG_OPENFAIL  "Error: Log file open attempt failed."
#define MSG_CLOSEFAIL "Error: Log file close attempt failed."
#define MSG_LOGFAIL_UNOPENED_FILE "Error: Log message attempt failed because log file is not open."
#define MSG_NEWLOGGING_SESSION "NEW LOGGING SESSION STARTED"
#define MSG_CLOSELOGGIN_SESSION "CLOSING LOGGING SESSION"
#if !defined(DLLEXPORT)
#if defined(MAKEDLL)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#endif

#if !defined(GET_SYSTEM_ERROR)
#define GET_SYSTEM_ERROR(errorCode, pBuffer) FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, 0, (LPSTR)&pBuffer, 0, nullptr)
#endif

namespace jl {
	class LogFile {
	public:
		DLLEXPORT LogFile();
		LogFile(const LogFile&) = delete;
		DLLEXPORT ~LogFile();
		DLLEXPORT virtual void Open(const char* fileName);
		DLLEXPORT virtual void Close();
		DLLEXPORT virtual LogFile& operator()(const char* source, const char* msg) { logMessage(source, msg); return *this; }
		DLLEXPORT virtual LogFile& operator()(const char *source, int errorCode) {
			char *pBuffer = nullptr;
			GET_SYSTEM_ERROR(errorCode, pBuffer);
			logMessage(source, pBuffer);
			return *this;
		}
		DLLEXPORT virtual LogFile& operator()(const char *source, const std::string& str) {
			logMessage(source, str.c_str());
			return *this;
		}
		DLLEXPORT virtual LogFile& operator()(const char *source, const std::ostringstream& os) {
			logMessage(source, os.str().c_str());
			return *this;
		}
		DLLEXPORT virtual void LogMessage(const char* source, const char* msg) { logMessage(source, msg); }
	protected:
		DLLEXPORT virtual void logMessage(const char* source, const char* msg);
		DLLEXPORT virtual void openLogMessage();
		DLLEXPORT virtual void closeLogMessage();
		std::mutex mu_write;
		std::ofstream log;
		struct std::tm logTime;
		std::time_t tTime;
	};
};