#include<gtest/gtest.h>
#include"../include/Journal.h"

TEST(JournalEntry, Construction_and_Output) {
	jl::JournalEntry je;
	std::cout << je << std::endl;
	jl::JournalEntry je1("Test message", "JournalEntry Testing");
	std::cout << je1 << std::endl;
}

TEST(Journal, Construction_and_Output) {
	jl::Journal j;
	j.MakeEntry("Entry 1", "Journal Test");
	j.MakeEntry("Entry 2", "Journal Test");
	j.MakeEntry("Entry 3", "Journal Test");
	std::cout << j << std::endl;
}


int main(int argc, char* argv[]) {

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}