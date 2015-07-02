#ifndef NETWORKLIB_NETWORKCLIENT
#define NETWORKLIB_NETWORKCLIENT

#include "Constants.h"
#include "Statistics.h"

#include "LockedQueue.h"

#include <asio.hpp>

#include <array>
#include <thread>


using asio::ip::udp;

namespace NetworkLib {
	class NetworkClient {
	public:
		NetworkClient(std::string host, std::string server_port, unsigned short local_port = 0);
		~NetworkClient();

		void Send(std::string message);

		inline bool HasMessages()
		{
			return !incomingMessages.empty();
		};

		inline std::string PopMessage()
		{
			if (incomingMessages.empty()) throw std::logic_error("No messages to pop");
			return incomingMessages.pop();
		};

	private:
		// Network send/receive stuff
		asio::io_service io_service;
		udp::socket socket;
		udp::endpoint server_endpoint;
		udp::endpoint remote_endpoint;
		std::array<char, NetworkBufferSize> recv_buffer;
		std::thread service_thread;

		// Queues for messages
		LockedQueue<std::string> incomingMessages;

		void start_receive();
		void handle_receive(const std::error_code& error, std::size_t bytes_transferred);
		void run_service();

		NetworkClient(NetworkClient&); // block default copy constructor

		// Statistics
		Statistics statistics;
	};
}
#endif
