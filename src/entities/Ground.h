#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>

namespace sf { class Sprite; }

class Player;

class Ground final : public Entity
{
	public:
		static constexpr float collisionRadius = 24.0f;

		Ground() = default;
		virtual ~Ground() = default;

		bool init() override;
		void update(float dt) override;
		void render(sf::RenderTarget& target) const override;

		void setScale(const sf::Vector2f& scale);

		float distanceTo(Player* pOther);
		bool isCollided(Player* pOther);

		sf::FloatRect getGlobalBounds() const { return m_pSprite->getGlobalBounds(); }

	private:
		sf::Vector2f m_scale = {3.0f, 3.0f};
};
