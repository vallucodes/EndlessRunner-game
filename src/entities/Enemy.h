#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace sf { class Sprite; }

class Player;

class Enemy final : public Entity
{
	public:
		static constexpr float collisionRadius = 24.0f;

		Enemy() = default;
		virtual ~Enemy() = default;

		bool init() override;
		bool init(float scale);
		void update(float dt) override;
		void render(sf::RenderTarget& target) const override;

		bool isKilled() { return m_isKilled; }

		bool isCollided(Player* pOther);

		sf::FloatRect getGlobalBounds() const { return m_pSprite->getGlobalBounds(); }
	private:
		bool m_isKilled = false;
};
