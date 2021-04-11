#ifdef APEX_PLATFORM_WINDOWS

#include <apex_pch.h>
#include "NetworkManager.h"

#include "Socket.h"
#include "Apex/Core/Timer.h"

namespace Apex {

	NetworkManager* NetworkManager::s_Instance = nullptr;
	bool NetworkManager::isStarted = false;

	void NetworkManager::Startup()
	{
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult != 0) {
			APEX_CORE_ERROR("TCPClientSocket :: WSAStartup FAILED: {0}", iResult);
			isStarted = false;
			return;
		}

		isStarted = true;
		s_Instance = new NetworkManager();
	}

	void NetworkManager::Cleanup()
	{
		WSACleanup();
		isStarted = false;
	}

	void NetworkManager::__StartServerWorkerThread__(std::function<bool(const Ref<TCPClientSocket>&, CommunicationPacket&)> function, std::chrono::microseconds diffTime)
	{
		std::thread([this, function, diffTime] {
			Ref<TCPServerSocket> server = std::make_shared<TCPServerSocket>("27015");
			server->BindNListen();
			if (server->IsListening()) {
				auto client = server->Accept();
				__StartClientWorkerThread__(client, function, diffTime);
			}
			server->Unbind();
		}).detach();
	}

	void NetworkManager::__StartClientWorkerThread__(const Ref<TCPClientSocket>& client, std::function<bool(const Ref<TCPClientSocket>&, CommunicationPacket&)> function, std::chrono::microseconds diffTime)
	{
		std::thread([function, diffTime, client] {
			if(!client->IsConnected()) // if not connected then attempt to connect
				client->Connect();
			if (client->IsConnected()) { // if failed to connect then exit else start loop
				auto start_time = std::chrono::steady_clock::now();
				uint32_t id = 0;
				CommunicationPacket packet;
				while (true) {
					auto currTime = std::chrono::steady_clock::now();
					std::chrono::microseconds now = std::chrono::duration_cast<std::chrono::microseconds>(currTime - start_time);
					packet = { id, now };
					if (!function(client, packet))
						break;
					id++;
					std::this_thread::sleep_until(currTime + diffTime);
				}
				client->Disconnect();
			}
		}).detach();
	}

}

#endif
