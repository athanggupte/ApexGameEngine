#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace Apex {
	
	class CommunicationPacket
	{
	public:
		uint32_t packetID;
		std::chrono::microseconds timestamp;
	};

	struct SocketAddress
	{
		char* addr;
		uint16_t port;
	};

	class TCPClientSocket
	{
		TCPClientSocket();
	public:
		TCPClientSocket(const std::string& hostName, const std::string& portNumber);
		TCPClientSocket(SOCKET socket);
		virtual ~TCPClientSocket();

		void Connect();
		int Send(const CommunicationPacket& packet, size_t packetSize);
		int Recv(CommunicationPacket& packet, size_t packetSize);
		void Shutdown();
		void Disconnect();

		const SocketAddress& GetSocketAddress();

		bool IsConnected() { return isConnected; }

	private:
		bool isConnected;
		SOCKET m_Socket;
		addrinfo* m_AddrInfo;
	};

	class TCPServerSocket
	{
		TCPServerSocket();
	public:
		TCPServerSocket(const std::string& portNumber);
		virtual ~TCPServerSocket();

		void BindNListen();
		Ref<TCPClientSocket> Accept();
		void Unbind();

		bool IsListening() { return isListening; }

	private:
		bool isListening;
		SOCKET m_Socket;
		addrinfo* m_AddrInfo;
	};

}