#include<gtest/gtest.h>
#include"../include/LogFile.h"

TEST(LogFile, LogFileTesting) {
	jl::Log_file lf;
	lf.Open("Test logfile.txt");
	lf.LogMessage("LogFileTesting", "This is a logfile test");
	lf.Close();
}

TEST(LogFile, LogFileOperator) {
	jl::Log_file lf;
	lf.Open("Test logfile3.txt");
	lf("LogFile Testing", "This is a test of the operator()");
	lf.Close();
}

TEST(LogFile, LogFileSysError) {
	jl::Log_file lf;
	lf.Open("Test logfile4.txt");
	lf("LogFile Testing", 10041);
	lf.Close();
}
jl::Log_file logfile;
void threadTest(const char* source) {
	std::ostringstream s;
	for (int i = 0; i < 100; i++) {
		std::ostringstream s;
		s << "Message: " << i;
		logfile.LogMessage(source, s.str().c_str());
	}
}
int main(int argc, char* argv[]) {
	logfile.Open("Test2 logfile.txt");
	std::thread t1(threadTest, "Thread 1");
	std::thread t2(threadTest, "Thread 2");
	t1.join();
	t2.join();

	logfile.Close();
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}