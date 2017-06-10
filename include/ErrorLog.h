#include<iostream>
#include<vector>
#include<sstream>
#include<Windows.h>


#if !defined(__JL_ERROR_LOG__)
#define __JL_ERROR_LOG__
namespace jl {
#define ERROR_MSG_DIVIDER "-------------------------------------------------------"
#define ERR_EXCEEDS_MAX_BUFFER_SIZE 0x3E80 // Going to check out error Message Text files later. Plan is to use codes defined here with message text files
	// Simple error info class
	class Error {
	private:
		LPSTR _message; // Description of error
		int _lineNum; // Line number where error occurred
		LPSTR _fileName; // Filename where error occurred
		std::stringstream _msgStream; // String stream to hold msg, linenum, and filename
	protected:
		virtual void _BuildMessageStream(); // Sets _msgStream using msg, line, and filename
	public:
		Error();
		Error(const Error& other);
		explicit Error(LPSTR Msg, int LineNum, LPSTR FileName);
		virtual void SetError(LPSTR Msg, int LineNum, LPSTR FileName); // Sets member vars and builds _msgStream
		inline virtual void output(std::ostream& o) const { // Output an error
			if(_msgStream)
				o << _msgStream.str().c_str();
		}
		inline friend std::ostream& operator<<(std::ostream &o, const Error &e) { // Output op.
			e.output(o);
			return o;
		}
		
	};

	// Log of errors. Designed for use when application may want to store errors instead of immediate output
	class ErrorLog {
	protected:
		std::vector<Error> _Log; // Container for errors
		inline virtual void _LogError(const Error &e) { _Log.push_back(e); }
	public:
		// Clear current error log
		inline virtual void Clear() { _Log.clear(); }
		
		// Get total number of errors in the log
		inline virtual size_t Count() const { return _Log.size(); }
		
		// Check if the log is empty
		inline virtual bool Empty() const { return _Log.empty(); }

		// Add new error to log
		virtual void LogError(
			const DWORD &ErrorCode,
			int LineNum = 0,
			LPSTR FileName = nullptr);
		
		virtual void LogError(
			const LPSTR ErrorMsg,
			int LineNum = 0,
			LPSTR FileName = nullptr);
		
		inline virtual void LogError(const Error &e) { _LogError(e); }
		
		// Output error log
		inline virtual void output(std::ostream& o) const { if (!_Log.empty()) { for (auto i : _Log) o << i << std::endl << ERROR_MSG_DIVIDER << std::endl; } else { o << "No errors detected." << std::endl << ERROR_MSG_DIVIDER << std::endl; } }
		
		// Error log output operator
		friend inline std::ostream& operator<<(std::ostream& o, const ErrorLog &e) {
			e.output(o);
			return o;
		}
	};

};

#endif
