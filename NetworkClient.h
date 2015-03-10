#ifndef NETWORKLIB_NETWORKCLIENT
#define NETWORKLIB_NETWORKCLIENT

#include "Constants.h"
#include "NetworkStatistics.h"

#include "LockedQueue.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <memory>
#include <array>


using boost::asio::ip::udp;

namespace NetworkLib {
	class NetworkClient {
	public:
		NetworkClient(std::string host, std::string server_port, unsigned short local_port = 0);
		~NetworkClient();

		void Send(std::string message);
		inline bool HasMessages() { return !incomingMessages.empty(); };
		inline std::string PopMessage() { if (incomingMessages.empty()) throw std::logic_error("No messages to pop"); return incomingMessages.pop(); };

		NetworkStatistics Statistics;
	private:
		// Network send/receive stuff
		boost::asio::io_service io_service;
		udp::socket socket;
		udp::endpoint server_endpoint;
		udp::endpoint remote_endpoint;
		std::array<char, NetworkBufferSize> recv_buffer;
		boost::thread service_thread;

		// Queues for messages
		LockedQueue<std::string> incomingMessages;

		void start_receive();
		void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
		void run_service();

		NetworkClient(NetworkClient&); // block default copy constructor

		// Statistics
	};
}
#endif