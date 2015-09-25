#include "targetver.h"
#include "NetworkClient.h"
#include "Log.h"

namespace NetworkLib {
	NetworkClient::NetworkClient(std::string host, std::string server_port, unsigned short local_port) :
		socket(io_service, udp::endpoint(udp::v4(), local_port)),
		service_thread(&NetworkClient::run_service, this)
	{
		udp::resolver resolver(io_service);
		udp::resolver::query query(udp::v4(), host, server_port);
		server_endpoint = *resolver.resolve(query);
		Send("");
	}

	NetworkClient::~NetworkClient()
	{
		io_service.stop();
		service_thread.join();
	}

	void NetworkClient::start_receive()
	{
		socket.async_receive_from(asio::buffer(recv_buffer), remote_endpoint,
			[this](std::error_code ec, std::size_t bytes_recvd){ this->handle_receive(ec, bytes_recvd); });
	}

	void NetworkClient::handle_receive(const std::error_code& error, std::size_t bytes_transferred)
	{
		if (!error)
		{
			std::string message(recv_buffer.data(), recv_buffer.data() + bytes_transferred);
			incomingMessages.push(message);
			statistics.RegisterReceivedMessage(bytes_transferred);
		}
		else
		{
			Log::Error("NetworkClient::handle_receive:", error);
		}

		start_receive();
	}

	void NetworkClient::Send(std::string message)
	{
		socket.send_to(asio::buffer(message), server_endpoint);
		statistics.RegisterSentMessage(message.size());
	}

	bool NetworkClient::HasMessages()
	{
		return !incomingMessages.empty();
	}

	std::string NetworkClient::PopMessage()
	{
		if (incomingMessages.empty()) 
			throw std::logic_error("No messages to pop");
		return incomingMessages.pop();
	}

	void NetworkClient::run_service()
	{
		start_receive();
		while (!io_service.stopped()) {
			try {
				io_service.run();
			}
			catch (const std::exception& e) {
				Log::Warning("Client: network exception: ", e.what());
			}
			catch (...) {
				Log::Error("Unknown exception in client network thread");
			}
		}
	}
}