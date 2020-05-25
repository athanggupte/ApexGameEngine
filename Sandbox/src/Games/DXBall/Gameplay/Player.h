#pragma once

#include "Apex/Networking/NetworkManager.h"

#include <glm/glm.hpp>

namespace DXBall {

	class Player
	{
	public:
		Player(const std::string& id = "INV_PLAYER", const glm::vec2& position = { 0.0f, 0.0f })
			: m_Position(position)
		{
			std::copy(id.begin(), id.begin() + 20, m_Id.data());
		}

		const std::string& GetId() { return std::string(m_Id.begin(), m_Id.end()); }
		const glm::vec2& GetPosition() { return m_Position; }

		void SetId(const std::string& id) { std::copy(id.begin(), id.begin() + 20, m_Id.data()); }
		void SetPosition(const glm::vec2& position) { m_Position = position; }

	private:
		std::array<char, 20> m_Id;
		glm::vec2 m_Position;
	};

}