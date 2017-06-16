#include"../include/LogFile.h"

jl::LogFile::LogFile() {}
jl::LogFile::~LogFile() {
	try {
		if (log.is_open()) {
			Close();
		}
	} catch (...) {

	}
}
void jl::LogFile::Open(const char* filename) {
	{
		std::lock_guard<std::mutex> locker(mu_write);
		try {
			if (!log.is_open()) {
				log.open(filename, std::ios_base::app);
			}
			if (!log.good())
				throw;
		} catch (...) {
			throw std::runtime_error(MSG_OPENFAIL);
		}
	}
	openLogMessage();
}

void jl::LogFile::Close() {
	closeLogMessage();
	{
		std::lock_guard<std::mutex> locker(mu_write);
		try {
			if (log.is_open()) {
				log.close();
			}
		} catch (...) {
			throw std::runtime_error(MSG_CLOSEFAIL);
		}
	}
	
}

void jl::LogFile::logMessage(const char *source, const char* msg) {
	std::lock_guard<std::mutex> locker(mu_write);
	try {
		if (!log.is_open())
			throw std::runtime_error(MSG_LOGFAIL_UNOPENED_FILE);
		tTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		localtime_s(&logTime, &tTime);
		log << std::put_time(&logTime, "%F %T") << " [" << source << "]\n\t" << msg << std::endl << MSG_DIV << std::endl;
	} catch (...) {
		throw;
	}
}

void jl::LogFile::closeLogMessage() {
	std::lock_guard<std::mutex> locker(mu_write);
	try {
		if (!log.is_open())
			throw std::runtime_error(MSG_LOGFAIL_UNOPENED_FILE);
		tTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		localtime_s(&logTime, &tTime);
		log << MSG_DIV2 << std::endl;
		log << std::put_time(&logTime, "%F %T") << " LOG FILE CLOSED" << std::endl << MSG_DIV2 << std::endl << std::endl;
	} catch (...) {
		throw;
	}
}

void jl::LogFile::openLogMessage() {
	std::lock_guard<std::mutex> locker(mu_write);
	try {
		if (!log.is_open())
			throw std::runtime_error(MSG_LOGFAIL_UNOPENED_FILE);
		tTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		localtime_s(&logTime, &tTime);
		log << MSG_DIV2 << std::endl;
		log << std::put_time(&logTime, "%F %T") << " LOG FILE OPENED" << std::endl << MSG_DIV2 << std::endl << std::endl;
	} catch (...) {
		throw;
	}
}