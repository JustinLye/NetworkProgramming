#include"../include/Journal.h"

jl::JournalEntry::JournalEntry(
	const char *Entry,
	const char *Author,
	std::time_t TimePt) :
	entry(Entry),
	author(Author) {
	if (entry == nullptr)
		entry = JRNL_NULLSTR;
	if (author == nullptr)
		author = JRNL_NULLSTR;
	localtime_s(&entryTime, &TimePt);

}

jl::Journal::Journal() {}
void jl::Journal::MakeEntry(const char *entry, const char *author) {
	journal.push_back(JournalEntry(entry, author));
}
void jl::Journal::Clear() { journal.clear(); }
void jl::Journal::Output(std::ostream& o) const {
	o << JRNL_MSG_DIVIDER << std::endl << "Journal Entries:" << std::endl << std::endl << JRNL_MSG_DIVIDER; 
	for (auto i : journal) {
		o << i << std::endl << JRNL_MSG_DIVIDER << std::endl << std::endl;
	}
}
