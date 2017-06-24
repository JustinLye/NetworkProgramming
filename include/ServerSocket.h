#include"SocketDefs.h"

#if !defined(__JL_SERVER_SOCKET_HEADER__)
#define __JL_SERVER_SOCKET_HEADER__


namespace jl {
	class ServerSocket : public Socket {
	protected:
		SOCKET Listen_socket; // socket the server listens on
		std::deque<ClientInfo> Client_queue; // double-ended queue of accepted client sockets. queue is produced by clientManger() and consumed by acceptingManager()
		std::thread Accept_connection_thread; // thread initiated in Initialize() method. this thread accepts client connections for the server
		std::thread Client_manager_thread;
		std::vector<std::pair<ClientInfo,std::thread>> Client_threads;
		bool Accept_connection;
		bool Should_close;
		DLLEXPORT virtual void Accepting_manager();
		DLLEXPORT virtual void Client_worker(const ClientInfo& ci);
		DLLEXPORT virtual void Client_manager();
		std::mutex Client_queue_mux;
		std::mutex Listen_socket_mux;
		std::mutex Error_log_mux;
		std::mutex WSALastError_mux;
		std::mutex Accept_connection_mux;
		std::condition_variable Accept_connect_condition;
		std::condition_variable New_client_condition;
		UINT Active_clients;
		Log_file Log;
	public:
		DLLEXPORT ServerSocket(const char* Log_filename = SRV_LOGFILENAME);
		DLLEXPORT ServerSocket(const struct SocketRequest& SocketReqInfo, const char* Log_filename = SRV_LOGFILENAME);
		DLLEXPORT ~ServerSocket();
		DLLEXPORT virtual int initialize(const struct SocketRequest& SocketReqInfo);
		DLLEXPORT virtual int close_socket();
		DLLEXPORT virtual int accept_connections();
	};
};
#endif