#ifndef NETWORKLIB_NETWORKSTATISTICS
#define NETWORKLIB_NETWORKSTATISTICS

namespace NetworkLib {
	struct NetworkStatistics {
	private:
		unsigned __int32 receivedMessages;
		unsigned __int32 sentMessages;

		unsigned long long receivedBytes;
		unsigned long long sentBytes;

	public:
		NetworkStatistics() :
			receivedMessages(0), sentMessages(0), receivedBytes(0), sentBytes(0)
		{}

		NetworkStatistics(NetworkStatistics& other) :
			receivedMessages(other.receivedMessages),
			sentMessages(other.sentMessages),
			receivedBytes(other.receivedBytes),
			sentBytes(other.sentBytes) {};

		inline unsigned __int32 GetReceivedMessages(){ return receivedMessages; };
		inline unsigned __int32 GetSentMessages(){ return sentMessages; };

		inline unsigned long long GetReceivedBytes(){ return receivedBytes; };
		inline unsigned long long GetSentBytes(){ return sentBytes; };

	protected:
		inline void AddReceivedMessage() { receivedMessages++; }
		inline void AddSentMessage() { receivedMessages++; }

		inline void AddReceivedBytes(size_t bytes){ receivedBytes += bytes; };
		inline void AddSentBytes(size_t bytes){ sentBytes += bytes; };

		friend class NetworkServer;
		friend class NetworkClient;
	};
}

#endif