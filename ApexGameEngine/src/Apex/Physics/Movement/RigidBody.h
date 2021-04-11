#pragma once

#include "glm/glm.hpp" 

namespace Apex {

    class RigidBody2D // : public Collidable
    {
    public:
        RigidBody2D(glm::vec2 position, glm::vec2 velocity = { 0.0f, 0.0f }, glm::vec2 acceleration = { 0.0f, 0.0f })
            : m_Position(position), m_Velocity(velocity), m_Acceleration(acceleration)
        {
        }

        void SetPosition(const glm::vec2& position) { m_Position = position; }
        void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }
        void SetAcceleration(const glm::vec2& acceleration) { m_Acceleration = acceleration; }

        const glm::vec2& GetPosition() const { return m_Position; }
        const glm::vec2& GetVelocity() const { return m_Velocity; }
        const glm::vec2& GetAcceleration() const { return m_Acceleration; }

        void UpdateRigidBody();

    protected:
        std::atomic<glm::vec2> m_Position;
        std::atomic<glm::vec2> m_Velocity;
        std::atomic<glm::vec2> m_Acceleration;

        //mass, elasticity
    };

}