#include"../include/Journal.h"

jl::JournalEntry::JournalEntry(
	const char *Entry,
	const char *Author,
	std::time_t TimePt) :
	message(Entry),
	source(Author) {
	if (message == nullptr)
		message = JRNL_NULLSTR;
	if (source == nullptr)
		source = JRNL_NULLSTR;
	localtime_s(&entryTime, &TimePt);

}

jl::Journal::Journal() {}
void jl::Journal::MakeEntry(const char *entry, const char *author) {
	journal.push_back(JournalEntry(entry, author));
}
void jl::Journal::Clear() { journal.clear(); }
void jl::Journal::Output(std::ostream& o) {
	while (!journal.empty()) {
		auto i = journal.front();
		o << i << std::endl << JRNL_MSG_DIVIDER << std::endl << std::endl;
		journal.pop_front();
	}
}
