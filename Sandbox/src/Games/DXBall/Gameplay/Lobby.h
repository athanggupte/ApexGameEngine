#pragma once

#include "Apex/Core.h"
#include "Apex/Networking/NetworkManager.h"
#include <vector>

#include "Player.h"

namespace DXBall {

	class MyServerPacket : public Apex::CommunicationPacket
	{
	public:
		std::vector<glm::vec2> positions;
	};

	class MyClientPacket : public Apex::CommunicationPacket
	{
	public:
		uint32_t playerId;
		glm::vec2 position;
	};

	class Lobby
	{
	public:
		Lobby(uint32_t maxSize = 2) : m_MaxSize(maxSize) {}

		uint32_t AddPlayer(const Player& player)
		{
			if (m_Players.size() < m_MaxSize) {
				m_Players.push_back(std::make_shared<Player>(player));
				return m_Players.size() - 1;
			}
			return UINT32_MAX;
		}

		uint32_t GetMaxSize() { return m_MaxSize; }

		uint32_t GetLobbySize() { return m_Players.size(); }

		const std::vector<Apex::Ref<Player>>& GetPlayerList() { return m_Players; }

		Apex::Ref<Player> GetPlayer(uint32_t index) { return m_Players.at(index); }

	private:
		uint32_t m_MaxSize;
		std::vector<Apex::Ref<Player>> m_Players;
	};

}