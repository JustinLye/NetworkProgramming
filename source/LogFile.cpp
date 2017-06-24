#include"../include/LogFile.h"


jl::Log_file::Log_file(const char* File_name)
{
	//open the file for appending, throw runtime_error if not in good state
	log.open(File_name, std::ios_base::app);
	if (log.rdstate() != std::ios_base::goodbit) {
		throw std::system_error::runtime_error("Log file was not successfully opened");
	}
	log << MSG_DIV2 << std::endl << MSG_NEWLOGGING_SESSION << std::endl << MSG_DIV2 << std::endl;
}

jl::Log_file::~Log_file()
{
	//output close message then close file
	std::lock_guard<std::mutex> Append_locker(Append_mutx);
	if (log.rdstate() != std::ios_base::goodbit)
		log.setstate(std::ios_base::goodbit);
	log << MSG_DIV2 << std::endl << MSG_CLOSELOGGIN_SESSION << std::endl << MSG_DIV2 << std::endl;
	log.close();
	if (log.rdstate() != std::ios_base::goodbit)
		throw std::system_error::runtime_error("Log file failed to be closed.");
	
}

struct std::tm jl::Log_file::get_localtime() const
{
	//get current local time
	struct std::tm Log_timestruct;
	std::time_t Log_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	localtime_s(&Log_timestruct, &Log_time);
	return Log_timestruct;
}

void jl::Log_file::log_message(const char* Message_source, const char* Message)
{
	//lock append mutex and write to log file
	std::lock_guard<std::mutex> Append_locker(Append_mutx);
	log << MSG_DIV << std::endl << std::put_time(&get_localtime(), "%F %T") << std::endl << Message_source << std::endl << Message << std::endl << MSG_DIV << std::endl;
	if (log.rdstate() != std::ios_base::goodbit) {
		log.setstate(std::ios_base::goodbit);
		log << MSG_DIV << std::endl << "Log file error" << std::endl << "Log file is no longer working" << MSG_DIV << std::endl;
	}
}

void jl::Log_file::log_message(const char* Message_source, const int& MS_error_code)
{
	const char* pbuffer = nullptr;
	GET_SYSTEM_ERROR(MS_error_code, pbuffer);
	log_message(Message_source, pbuffer);
}

void jl::Log_file::log_message(const Message& Msg)
{
	//lock append mutex and write to log file
	std::lock_guard<std::mutex> Append_locker(Append_mutx);
	log << MSG_DIV << std::endl << std::put_time(&get_localtime(), "%F %T") << std::endl << Msg << std::endl << MSG_DIV << std::endl;
	
	//check error occurred
	if (log.rdstate() != std::ios_base::goodbit) {
		log.setstate(std::ios_base::goodbit);
		log << MSG_DIV << std::endl << "Log file error" << std::endl << "Log file is no longer working" << MSG_DIV << std::endl;
	}
}

