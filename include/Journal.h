#include<iostream>
#include<string>
#include<deque>
#include<iomanip>
#include<ctime>
#include<chrono>

#if !defined(__JL_JOURNAL_HEADER__)
#define __JL_JOURNAL_HEADER__

#define JRNLENTRY_BUFFERSIZE 512;
#define JRNL_MSG_DIVIDER "-------------------------------------------------------"
#define JRNL_NULLSTR "NULL"
//#if defined(DLLEXPORT)
//#undef DLLEXPORT
//#endif
#if defined(MAKEDLL)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
namespace jl {

	struct JournalEntry {
		char *message;
		char *source; // (i.e. the source of the entry)
		struct std::tm entryTime;
		DLLEXPORT JournalEntry(
			const char *Entry = nullptr,
			const char *Author = nullptr,
			std::time_t TimePt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
		DLLEXPORT friend std::ostream& operator<<(std::ostream& o, const JournalEntry& j) {
			o << std::put_time(&j.entryTime, "%F %T") << " [" << j.source << "]: " << j.message << std::endl << JRNL_MSG_DIVIDER << std::endl;
			return o;
		}
	};

	class Journal {
	public:
		DLLEXPORT Journal();
		DLLEXPORT virtual void MakeEntry(const char *entry, const char *author);
		DLLEXPORT virtual void Clear();
		DLLEXPORT virtual void Output(std::ostream& o) const;
		DLLEXPORT friend std::ostream& operator<<(std::ostream& o, const Journal& j) {
			j.Output(o);
			return o;
		}
	protected:
		
	};

}; //namespace jl

#endif