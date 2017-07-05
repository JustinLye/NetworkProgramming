#include<iostream>
#include<iomanip>

#if !defined(__JL_MESSAGE_HEADER__)
#define __JL_MESSAGE_HEADER__


#if !defined(DLLEXPORT)
#if defined(MAKEDLL)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#endif

// Macro for windows system error codes
#if !defined(GET_SYSTEM_ERROR)
#if defined(_WIN32)
#if !defined(FormatMessage)
#include<Windows.h>
#endif //inlcude Windows.h
#if defined(FormatMessage)
#define GET_SYSTEM_ERROR(errorCode, pBuffer) FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, 0, (LPSTR)&pBuffer, 0, nullptr)
#endif //windows FormatMessage
#elif defined(_UNIX)
#include<errno.h>
#define GET_SYSTEM_ERROR(Error_code, Buffer) strerror_r(Error_code, Buffer, 512)
#endif
#endif

namespace jl {

	struct Message_source {
		const char* File_name;
		const char* Function_name;
		int Line_number;
		DLLEXPORT Message_source(
			const char* File = "NULL",
			const char* Function = "NULL",
			const int& Line = 0
		);
		DLLEXPORT friend std::ostream& operator<<(std::ostream& o, const Message_source& mb);
	};

	class Message {
	protected:
		Message_source Msg_blck;
		DLLEXPORT virtual const char* type() const = 0;
		DLLEXPORT virtual const char* detail() const = 0;
		DLLEXPORT virtual void output(std::ostream& o) const;
	public:
		DLLEXPORT Message() {}
		DLLEXPORT Message(const Message_source& Msgb) : Msg_blck(Msgb) {}
		DLLEXPORT friend std::ostream& operator<<(std::ostream& o, const Message& m);
	};

	class Info_message : public Message {
	protected:
		const char* Message_desc;
		DLLEXPORT virtual const char* type() const;
		DLLEXPORT virtual const char* detail() const;
	public:
		DLLEXPORT Info_message() : Message(), Message_desc("NULL") {}
		DLLEXPORT Info_message(const jl::Message_source& mb, const char* Desc = "NULL") :
			Message(mb),
			Message_desc(Desc) {}
	};

	class System_error_message : public Message {
	protected:
		int System_error_code;
		const char* System_error_desc;
		DLLEXPORT virtual const char* type() const;
		DLLEXPORT virtual const char* detail() const;
	public:
		DLLEXPORT System_error_message();
		DLLEXPORT System_error_message(const jl::Message_source& mb, const int& Error_code);
	};

	class Application_error_message : public Info_message {
	protected:
		DLLEXPORT virtual const char* type() const;
	public:
		DLLEXPORT Application_error_message();
		DLLEXPORT Application_error_message(const jl::Message_source& mb, const char* Desc = "NULL");

	};

} //jl namspace
#endif