#include<gtest/gtest.h>
#include"../include/LogFile.h"
#if !defined(JL_FILENAME) // JL_FILENAME r qqemoves the directory path from __FILE__ leaving just the file name
#include<string>
#if defined(_WIN32)
#define JL_FILENAME ((strrchr(__FILE__, '\\')) ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif defined(_UNIX)
#define JL_FILENAME ((strrchr(__FILE__, '/')) ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#endif


TEST(LogFile, LogFileTesting) {
	jl::Log_file lf("Test logfile.txt");
	lf.log_message("LogFileTesting", "This is a logfile test");
}

TEST(LogFile, LogFileSysError) {
	jl::Log_file lf("Test logfile.txt");
	lf.log_message("LogFile Testing", 10041);
}
jl::Log_file logfile("Test logfile2.txt");
void threadTest(const char* source) {
	std::ostringstream s;
	for (int i = 0; i < 100; i++) {
		std::ostringstream s;
		s << "Message: " << i;
		logfile.log_message(source, s.str().c_str());
	}
}
TEST(Message_Test, Info_message)
{
	jl::Log_file lf("test.log");
	jl::Message_source mb(JL_FILENAME, __FUNCTION__, __LINE__);
	jl::Info_message im(mb, "Testing");
	lf.log_message(im);

}

TEST(Message_Test, Error_message)
{
	jl::Log_file lf("test.log");
	jl::Message_source mb(JL_FILENAME, __FUNCTION__, __LINE__);
	jl::System_error_message sm(mb, 1);
	lf.log_message(sm);
}

TEST(Messasge_Test, User_error_message)
{
	jl::Log_file lf("test.log");
	jl::Message_source mb(JL_FILENAME, __FUNCTION__, __LINE__);
	jl::Application_error_message um(mb, "Something is wrong with you implementation sir.");
	lf.log_message(um);
}

TEST(Macro_Builder_Test, Info_message)
{
	jl::Log_file lf("macrotest.log");
	lf.log_message(INFO_MESSAGE("Testing info message macro."));
}

TEST(Macro_Builder_Test, System_error_message)
{
	jl::Log_file lf("macrotest.log");
	lf.log_message(SYS_ERROR_MESSAGE(10043));
}

TEST(Macro_Builder_Test, User_error_message)
{
	jl::Log_file lf("macrotest.log");
	lf.log_message(APP_ERROR_MESSAGE("Testing user message macro."));
}

int main(int argc, char* argv[]) {
	std::thread t1(threadTest, "Thread 1");
	std::thread t2(threadTest, "Thread 2");
	t1.join();
	t2.join();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}