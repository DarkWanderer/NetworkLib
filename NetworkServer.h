#ifndef NETWORKLIB_NETWORKSERVER
#define NETWORKLIB_NETWORKSERVER

#include "Constants.h"
#include "NetworkStatistics.h"

#include "LockedQueue.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bimap.hpp>
#include <boost/thread.hpp>

#include <string>
#include <array>

using boost::asio::ip::udp;

typedef boost::bimap<long long, udp::endpoint> ClientList;
typedef ClientList::value_type Client;
typedef std::pair<std::string, long long> ClientMessage;

namespace NetworkLib {
	class NetworkServer {
	public:
		NetworkServer(unsigned short local_port);
		~NetworkServer();

		bool HasMessages();
		ClientMessage PopMessage();

		void SendToClient(const std::string& message, unsigned long long clientID);
		void SendToAllExcept(const std::string& message, unsigned long long clientID);
		void SendToAll(const std::string& message);

		NetworkStatistics Statistics;
	private:
		// Network send/receive stuff
		boost::asio::io_service io_service;
		udp::socket socket;
		udp::endpoint server_endpoint;
		udp::endpoint remote_endpoint;
		std::array<char, NetworkBufferSize> recv_buffer;
		boost::thread service_thread;

		void start_receive();
		void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
		void handle_send(std::string /*message*/, const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/)	{}
		void run_service();
		unsigned long long get_client_id(udp::endpoint endpoint);

		void send(const std::string& message, udp::endpoint target);

		// Incoming messages queue
		LockedQueue<ClientMessage> incomingMessages;

		// Clients of the server
		ClientList clients;
		unsigned long long nextClientID;

		NetworkServer(NetworkServer&); // block default copy constructor
	};
}
#endif