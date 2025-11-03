#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>

class StatePlaying;

namespace sf { class Sprite; }

class Player final : public Entity
{
	public:
		static constexpr float collisionRadius = 42.0f;

		Player(StatePlaying* pState);
		virtual ~Player() = default;

		bool init() override;
		void update(float dt) override;
		void render(sf::RenderTarget& target) const override;

		sf::FloatRect getGlobalBounds() const;

		bool	m_isMovingUp = false;
		bool	m_isDead = false;
	private:
		sf::Vector2f	m_acceleration;
		float		m_gas = 100.0f;
		float			m_repelForce = 0.0f;
		float			m_distance = 100.0f;



		StatePlaying* m_pState;
};
