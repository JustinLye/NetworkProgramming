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
	//It does not feel like using a mutex lock in a destructor is well advised, but 
	//I am thinking in future version of this class it may be a good idea to maintain
	//some sort of reference counter and/or list of reference to a ofstream resource.
	//Use the reference count to determine if the file should be closed or not.

	//I don't even know if I can have to instances of a Log_file that use the smae
	//ofstream resource.

	//Test multiple instances of a Log_file using the same ofstream resource. 
	std::lock_guard<std::mutex> Append_locker(Append_mutx);
	if (log.rdstate() != std::ios_base::goodbit)
		log.setstate(std::ios_base::goodbit);
	log.close();
	if (log.rdstate() != std::ios_base::goodbit)
		throw std::system_error::runtime_error("Log file failed to be closed.");
	
}

void jl::Log_file::log_message(const char* Message_source, const char* Message)
{
	//get current local time
	struct std::tm Log_timestruct;
	std::time_t Log_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	localtime_s(&Log_timestruct, &Log_time);
	
	//lock append mutex and write to log file
	std::lock_guard<std::mutex> Append_locker(Append_mutx);
	log << MSG_DIV << std::endl << Message_source << std::endl << Message << std::endl << MSG_DIV << std::endl;
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