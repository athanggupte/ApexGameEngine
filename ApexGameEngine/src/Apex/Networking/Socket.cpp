#include <apex_pch.h>

#ifdef APEX_PLATFORM_WINDOWS
#include "Socket.h"

#include "NetworkManager.h"

namespace Apex {
	
#define SRV_REPLY_MSG "Roger that!"
#define SRV_REPLY_MSG_LEN 12

	TCPServerSocket::TCPServerSocket()
	{
		// Initialize WinSock2
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult != 0) {
			APEX_CORE_ERROR("TCPServerSocket :: WSAStartup FAILED: {0}", iResult);
			return;
		}

		// Set addrinfo for server socket
		struct addrinfo* result = nullptr, * ptr = nullptr, hints;
		ZeroMemory(&hints, sizeof hints); // memset 0
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE; // indicates the returned socket will be used in bind()

		iResult = getaddrinfo(nullptr, "27015", &hints, &result);
		
		APEX_CORE_ASSERT(result, "TCPServerSocket :: Returned socket is NULL");

		if (iResult != 0) {
			APEX_CORE_ERROR("TCPServerSocket :: getaddrinfo FAILED: {0}", iResult);
			WSACleanup();
			return;
		}

		// Create client socket
		SOCKET listenSocket = INVALID_SOCKET;

		ptr = result;

		listenSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (listenSocket == INVALID_SOCKET) {
			APEX_CORE_ERROR("TCPServerSocket :: Error at socket(): {0}", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return;
		}

		{
			sockaddr_in* sin = (sockaddr_in*)ptr->ai_addr;
			APEX_CORE_INFO("TCPClientSocket :: socket :: {0}:{1}", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
		}

		// Bind socket to address
		iResult = bind(listenSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPServerSocket :: bind FAILED: {0}", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(listenSocket);
			WSACleanup();
			return;
		}

		freeaddrinfo(result);

		// Listen on socket
		iResult = listen(listenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPServerSocket :: Listen FAILED: {0}", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return;
		}

		APEX_CORE_DEBUG("TCPServerSocket :: Listening for connections ...");

		// Accept connection
		SOCKET clientSocket = INVALID_SOCKET;

		clientSocket = accept(listenSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			APEX_CORE_ERROR("TCPServerSocket :: Accept FAILED: {0}", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return;
		}

		closesocket(listenSocket);

		// Use socket for receiving and sending
		// ...
		APEX_CORE_INFO("TCPServerSocket :: Client connected");

		while (true) {
			CommunicationPacket packet;
			iResult = recv(clientSocket, (char*)&packet, (int)sizeof(packet), 0);
			if (iResult > 0) {
				APEX_CORE_INFO("TCPServerSocket :: Bytes received : {0}", iResult);

				APEX_CORE_TRACE("TCPServerSocket :: Received Message : #{0} <{1}>", packet.packetID, packet.timestamp.count());

				int iSendResult = send(clientSocket, SRV_REPLY_MSG, SRV_REPLY_MSG_LEN, 0);
				if (iSendResult == SOCKET_ERROR) {
					APEX_CORE_ERROR("TCPServerSocket :: send FAILED: {0}", WSAGetLastError());
					closesocket(clientSocket);
					WSACleanup();
					return;
				}

				APEX_CORE_DEBUG("TCPServerSocket :: Bytes sent : {0}", iSendResult);
			}
			else if (iResult == 0) {
				APEX_CORE_WARN("TCPServerSocket :: Closing Socket ...");
				break;
			}
			else {
				APEX_CORE_ERROR("TCPServerSocket :: recv FAILED: {0}", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return;
			}
		}

		APEX_CORE_INFO("TCPServerSocket :: Successfully completed all SERVER functions");
		
		// Shutdown sending on socket
		iResult = shutdown(clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPServerSocket :: Shutdown FAILED: {0}", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return;
		}

		// Use socket fo receiving only 
		// ...

		// Close socket
		closesocket(clientSocket);
		WSACleanup();
		return;
	}

	TCPServerSocket::TCPServerSocket(const std::string& portNumber)
		: isListening(false), m_Socket(INVALID_SOCKET), m_AddrInfo(nullptr)
	{
		int iResult;

		addrinfo hints;
		ZeroMemory(&hints, sizeof hints); // memset 0
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE; // indicates the returned socket will be used in bind()

		iResult = getaddrinfo(nullptr, portNumber.c_str(), &hints, &m_AddrInfo);

		APEX_CORE_ASSERT(m_AddrInfo, "TCPServerSocket :: Returned socket is NULL");

		if (iResult != 0) {
			APEX_CORE_ERROR("TCPServerSocket :: getaddrinfo FAILED: {0}", iResult);
			NetworkManager::Cleanup();
			return;
		}
	}

	TCPServerSocket::~TCPServerSocket()
	{
		Unbind();
		freeaddrinfo(m_AddrInfo);
	}

	void TCPServerSocket::BindNListen()
	{
		int iResult;

		isListening = false;

		m_Socket = socket(m_AddrInfo->ai_family, m_AddrInfo->ai_socktype, m_AddrInfo->ai_protocol);

		if (m_Socket == INVALID_SOCKET) {
			APEX_CORE_ERROR("TCPServerSocket :: Error at socket(): {0}", WSAGetLastError());
			freeaddrinfo(m_AddrInfo);
			NetworkManager::Cleanup();
			return;
		}

		{
			sockaddr_in* sin = (sockaddr_in*)m_AddrInfo->ai_addr;
			APEX_CORE_INFO("TCPServerSocket :: socket :: {0}:{1}", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
		}

		// Bind socket to address
		iResult = bind(m_Socket, m_AddrInfo->ai_addr, (int)m_AddrInfo->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPServerSocket :: bind FAILED: {0}", WSAGetLastError());
			freeaddrinfo(m_AddrInfo);
			Unbind();
			NetworkManager::Cleanup();
			return;
		}

		// Listen on socket
		iResult = listen(m_Socket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPServerSocket :: Listen FAILED: {0}", WSAGetLastError());
			Unbind();
			NetworkManager::Cleanup();
			return;
		}

		isListening = true;
	}

	Ref<TCPClientSocket> TCPServerSocket::Accept()
	{
		SOCKET clientSocket = INVALID_SOCKET;

		clientSocket = accept(m_Socket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			APEX_CORE_ERROR("TCPServerSocket :: Accept FAILED: {0}", WSAGetLastError());
			Unbind();
			NetworkManager::Cleanup();
		}

		return std::make_shared<TCPClientSocket>(clientSocket);
	}

	void TCPServerSocket::Unbind()
	{
		closesocket(m_Socket);
		isListening = false;
		APEX_CORE_WARN("TCPServerSocket :: Server has stopped listening for new connections.");
	}

	////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////    TCP Client    ////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	TCPClientSocket::TCPClientSocket()
	{
		// Initialize WinSock2
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult != 0) {
			APEX_CORE_ERROR("TCPClientSocket :: WSAStartup FAILED: {0}", iResult);
			return;
		}

		// Set addrinfo for client socket
		struct addrinfo *result = nullptr, *ptr = nullptr, hints;
		ZeroMemory(&hints, sizeof hints); // memset 0
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		iResult = getaddrinfo("localhost", "27015", &hints, &result);

		if (iResult != 0) {
			APEX_CORE_ERROR("TCPClientSocket :: getaddrinfo FAILED: {0}", iResult);
			WSACleanup();
			return;
		}

		APEX_CORE_DEBUG("TCPClientSocket :: getaddrinfo :: {0}", iResult);


		// Connect socket to server
		SOCKET connectSocket = INVALID_SOCKET;
		APEX_CORE_TRACE("TCPClientSocket :: Connecting to server ...");
		
		for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {

			connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (connectSocket == INVALID_SOCKET) {
				APEX_CORE_ERROR("TCPClientSocket :: Error at socket(): {0}", WSAGetLastError());
				freeaddrinfo(result);
				WSACleanup();
				return;
			}

			{
				sockaddr_in* sin = (sockaddr_in*)ptr->ai_addr;
				APEX_CORE_INFO("TCPClientSocket :: socket :: {0}:{1}", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
			}

			//APEX_CORE_DEBUG("TCPClientSocket :: <before connect> iResult :: {0}", iResult);

			iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (iResult == SOCKET_ERROR) {
				closesocket(connectSocket);
				connectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (connectSocket == INVALID_SOCKET) {
			APEX_CORE_ERROR("TCPClientSocket :: Unable to connect to server!");
			WSACleanup();
			return;
		}

		// Use socket for receiving and sending
		// ...
		const char* sendbuf = "Hello from client";

		iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);

		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPClientSocket :: send FAILED: {0}", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return;
		}

		APEX_CORE_DEBUG("TCPClientSocket :: Bytes sent : {0}", iResult);

		// Shutdown sending on socket
		iResult = shutdown(connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPClientSocket :: Shutdown FAILED: {0}", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return;
		}

		// Use socket for receiving only 
		// ...
		while (true) {
			char recvbuf[512];
			int recvbuflen = 512;
			iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				APEX_CORE_INFO("TCPClientSocket :: Bytes received : {0}", iResult);
				APEX_CORE_TRACE("TCPClientSocket :: Received Message : {0}", recvbuf);
			}
			else if (iResult == 0) {
				APEX_CORE_WARN("TCPClientSocket :: Connection Closed.");
				break;
			}
			else {
				APEX_CORE_ERROR("TCPClientSocket :: recv FAILED: {0}", WSAGetLastError());
				break;
			}
		}

		APEX_CORE_INFO("TCPClientSocket :: Successfully completed all CLIENT functions");

		// Close socket
		closesocket(connectSocket);
		WSACleanup();
		return;
	}

	TCPClientSocket::TCPClientSocket(const std::string& hostName, const std::string& portNumber)
		: isConnected(false), m_Socket(INVALID_SOCKET), m_AddrInfo(nullptr)
	{
		int iResult;

		addrinfo hints;
		ZeroMemory(&hints, sizeof hints); // memset 0
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		iResult = getaddrinfo(hostName.c_str(), portNumber.c_str(), &hints, &m_AddrInfo);

		if (iResult != 0) {
			APEX_CORE_ERROR("TCPClientSocket :: getaddrinfo FAILED: {0}", iResult);
			NetworkManager::Cleanup();
			return;
		}

		APEX_CORE_DEBUG("TCPClientSocket :: getaddrinfo :: {0}", iResult);

	}

	TCPClientSocket::TCPClientSocket(SOCKET socket)
	{
		m_Socket = socket;
		isConnected = true;
		m_AddrInfo = nullptr;
	}

	TCPClientSocket::~TCPClientSocket()
	{
		Disconnect();
		freeaddrinfo(m_AddrInfo);
	}

	void TCPClientSocket::Connect()
	{
		APEX_CORE_TRACE("TCPClientSocket :: Connecting to server ...");
		int iResult;

		isConnected = false;

		for (addrinfo* ptr = m_AddrInfo; ptr != nullptr; ptr = ptr->ai_next) {

			m_Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (m_Socket == INVALID_SOCKET) {
				APEX_CORE_ERROR("TCPClientSocket :: Error at socket(): {0}", WSAGetLastError());
				freeaddrinfo(m_AddrInfo);
				NetworkManager::Cleanup();
				return;
			}

			/*{
				sockaddr_in* sin = (sockaddr_in*)ptr->ai_addr;
				APEX_CORE_INFO("TCPClientSocket :: socket :: {0}:{1}", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
			}*/

			//APEX_CORE_DEBUG("TCPClientSocket :: <before connect> iResult :: {0}", iResult);

			iResult = connect(m_Socket, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (iResult == SOCKET_ERROR) {
				Disconnect();
				m_Socket = INVALID_SOCKET;
				continue;
			}
			break;
		}
		
		if (m_Socket == INVALID_SOCKET) {
			APEX_CORE_ERROR("TCPClientSocket :: Unable to connect to server!");
			NetworkManager::Cleanup();
			return;
		}
		
		APEX_CORE_INFO("TCPClientSocket :: Connected successfully!");
		isConnected = true;
	}

	int TCPClientSocket::Send(const CommunicationPacket& packet, size_t packetSize)
	{
		int iResult = send(m_Socket, (char*)&packet, (int)packetSize, 0);

		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPClientSocket :: send FAILED: {0}", WSAGetLastError());
			Disconnect();
			NetworkManager::Cleanup();
		}

		return iResult;
	}

	int TCPClientSocket::Recv(CommunicationPacket& packet, size_t packetSize)
	{
		int	iResult = recv(m_Socket, (char*)&packet, (int)packetSize, 0);
		if (iResult > 0) {
			//APEX_CORE_INFO("TCPClientSocket :: Bytes received : {0}", iResult);
			APEX_CORE_TRACE("TCPClientSocket :: Received Packet : #{0} <{1}>", packet.packetID, packet.timestamp.count());
		}
		else if (iResult == 0) {
			APEX_CORE_WARN("TCPClientSocket :: Connection Closed.");
		}
		else {
			APEX_CORE_ERROR("TCPClientSocket :: recv FAILED: {0}", WSAGetLastError());
			Disconnect();
			NetworkManager::Cleanup();
		}
		
		return iResult;
	}

	void TCPClientSocket::Shutdown()
	{
		int iResult;
		iResult = shutdown(m_Socket, SD_SEND);
		isConnected = false;
		if (iResult == SOCKET_ERROR) {
			APEX_CORE_ERROR("TCPClientSocket :: Shutdown FAILED: {0}", WSAGetLastError());
			Disconnect();
			NetworkManager::Cleanup();
			return;
		}
	}

	void TCPClientSocket::Disconnect()
	{
		closesocket(m_Socket);
		isConnected = false;
		APEX_CORE_WARN("TCPClientSocket :: Client has disconnected.");
	}

	const SocketAddress& TCPClientSocket::GetSocketAddress()
	{
		SocketAddress sAddress = { "NA", 0 };
		if (isConnected) {
			sockaddr saddr;
			int slen = -1;
			getsockname(m_Socket, &saddr, &slen);
			sockaddr_in* sin = (sockaddr_in*)&saddr;
			sAddress.addr = inet_ntoa(sin->sin_addr);
			sAddress.port = htons(sin->sin_port);
		}
		return sAddress;
	}


}

#endif
