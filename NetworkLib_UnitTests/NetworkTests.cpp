#include "CppUnitTest.h"

#include "..\NetworkLib\NetworkServer.h"
#include "..\NetworkLib\NetworkClient.h"

#include <memory>
#include <thread>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace chrono = std::chrono;

namespace Multiorb_UnitTests
{
	TEST_CLASS(NetworkTests)
	{
		static std::unique_ptr<NetworkLib::NetworkServer> CreateServer()
		{
			return std::make_unique<NetworkLib::NetworkServer>(12345);
		};

		static std::unique_ptr<NetworkLib::NetworkClient> CreateClient() {
			return std::make_unique<NetworkLib::NetworkClient>("localhost", "12345", 23456);
		};

		const chrono::milliseconds sleepDuration = chrono::milliseconds(5);

	public:
		TEST_METHOD(NetworkServerConstructorShouldWork)
		{
			auto server = CreateServer();
		};

		TEST_METHOD(NetworkServerShouldHaveNoMessagesWhenCreated)
		{
			auto server = CreateServer();
			Assert::IsFalse(server->HasMessages());
		};

		TEST_METHOD(NetworkClientConstructorShouldWork)
		{
			auto client = CreateClient();
			Assert::IsFalse(client->HasMessages());
		};

		TEST_METHOD(NetworkClientShouldHaveNoMessagesWhenCreated)
		{
			auto client = CreateClient();
			Assert::IsFalse(client->HasMessages());
		};

		TEST_METHOD(SendMessageFromClientToServerShouldProduceSameMessage)
		{
			auto server = CreateServer();
			auto client = CreateClient();
			std::string message("Test message");

			// Send client->server
			client->Send(message);

			std::this_thread::sleep_for(sleepDuration);

			Assert::IsTrue(server->HasMessages());
			Assert::IsFalse(client->HasMessages());

			auto receivedMessage = server->PopMessage().first;
			Assert::IsTrue(message == receivedMessage);

			Assert::IsFalse(server->HasMessages());
			Assert::IsFalse(client->HasMessages());
		}

		TEST_METHOD(SendMessageFromServerToClientShouldProduceSameMessage)
		{
			auto server = CreateServer();
			auto client = CreateClient();
			std::string message("Test message");

			// Sleep for a bit so that server has time to 
			// receive client announcement message
			std::this_thread::sleep_for(sleepDuration);
			
			// Send from server to client
			// TODO: get client ID from server itself
			server->SendToClient(message, 1);
			std::this_thread::sleep_for(sleepDuration);

			Assert::IsFalse(server->HasMessages());

			auto receivedMessage = client->PopMessage();
			Assert::IsTrue(message == receivedMessage);

			Assert::IsFalse(server->HasMessages());
			Assert::IsFalse(client->HasMessages());
		}
	};
}
