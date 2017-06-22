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
	class Log_file {
	public:
		DLLEXPORT Log_file(const char* File_name);
		Log_file(const Log_file&) = delete;
		DLLEXPORT ~Log_file();
		DLLEXPORT virtual void log_message(const char* Message_source, const char* Message);
		DLLEXPORT virtual void log_message(const char* Message_source, const int& MS_error_code);
	protected:
		std::mutex Append_mutx;
		std::ofstream log;
	};
};