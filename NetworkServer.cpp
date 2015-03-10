#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN
#include <boost/bind.hpp>

#include "NetworkServer.h"
#include "Log.h"

namespace NetworkLib {
	NetworkServer::NetworkServer(unsigned short local_port) :
		socket(io_service, udp::endpoint(udp::v4(), local_port)),
		nextClientID(0L),
		service_thread(std::bind(&NetworkServer::run_service, this))
	{
		Log::Info("Starting server on port ", local_port);
	};

	NetworkServer::~NetworkServer()
	{
		io_service.stop();
		service_thread.join();
	}

	void NetworkServer::start_receive()
	{
		socket.async_receive_from(boost::asio::buffer(recv_buffer), remote_endpoint,
			boost::bind(&NetworkServer::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void NetworkServer::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (!error)
		{
			try {
				auto message = ClientMessage(std::string(recv_buffer.data(), recv_buffer.data() + bytes_transferred), get_client_id(remote_endpoint));
				if (!message.first.empty())
					incomingMessages.push(message);
				Statistics.AddReceivedBytes(bytes_transferred);
				Statistics.AddReceivedMessage();
			}
			catch (std::exception ex) {
				Log::Error("handle_receive: Error parsing incoming message:", ex.what());
			}
			catch (...) {
				Log::Error("handle_receive: Unknown error while parsing incoming message");
			}
		}
		else
		{
			Log::Error("handle_receive: error: ", error.message());
		}

		start_receive();
	}

	void NetworkServer::send(const std::string& message, udp::endpoint target_endpoint)
	{
		socket.send_to(boost::asio::buffer(message), target_endpoint);
		Statistics.AddSentBytes(message.size());
		Statistics.AddSentMessage();
	}

	void NetworkServer::run_service()
	{
		start_receive();
		while (!io_service.stopped()){
			try {
				io_service.run();
			}
			catch (const std::exception& e) {
				Log::Error("NetworkServer: Network exception: ", e.what());
			}
			catch (...) {
				Log::Error("NetworkServer: Network exception: unknown");
			}
		}
		Log::Debug("Server network thread stopped");
	};

	unsigned long long NetworkServer::get_client_id(udp::endpoint endpoint)
	{
		auto cit = clients.right.find(endpoint);
		if (cit != clients.right.end())
			return (*cit).second;

		nextClientID++;
		clients.insert(Client(nextClientID, endpoint));
		return nextClientID;
	};

	void NetworkServer::SendToClient(const std::string& message, unsigned long long clientID)
	{
		try {
			send(message, clients.left.at(clientID));
		}
		catch (std::out_of_range) {
			Log::Error(__FUNCTION__": Unknown client ID ", clientID);
		}
	};

	void NetworkServer::SendToAllExcept(const std::string& message, unsigned long long clientID)
	{
		for (auto client : clients)
			if (client.left != clientID)
				send(message, client.right);
	};

	void NetworkServer::SendToAll(const std::string& message)
	{
		for (auto client : clients)
			send(message, client.right);
	};

	ClientMessage NetworkServer::PopMessage() {
		return incomingMessages.pop();
	}

	bool NetworkServer::HasMessages()
	{
		return !incomingMessages.empty();
	};
}