#pragma once

#include "Socket.h"

namespace Apex {

	class NetworkManager
	{
	public:
		static void Startup();
		static void Cleanup();

		static void StartServerWorkerThread(std::function<bool(const Ref<TCPClientSocket>&, CommunicationPacket&)> function, std::chrono::microseconds diffTime) { s_Instance->__StartServerWorkerThread__(function, diffTime); }
		static void StartClientWorkerThread(const Ref<TCPClientSocket>& client, std::function<bool(const Ref<TCPClientSocket>&, CommunicationPacket&)> function, std::chrono::microseconds diffTime) { s_Instance->__StartClientWorkerThread__(client, function, diffTime); }

	protected:
		virtual void __StartServerWorkerThread__(std::function<bool(const Ref<TCPClientSocket>&, CommunicationPacket&)> function, std::chrono::microseconds diffTime);
		virtual void __StartClientWorkerThread__(const Ref<TCPClientSocket>& client, std::function<bool(const Ref<TCPClientSocket>&, CommunicationPacket&)> function, std::chrono::microseconds diffTime);

	public:
		static bool isStarted;

	private:
		static NetworkManager* s_Instance;
	};

}