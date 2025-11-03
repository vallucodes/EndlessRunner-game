#include "Ground.h"
#include "Player.h"
#include "ResourceManager.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>

bool Ground::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("ground.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(m_scale);
    m_collisionRadius = collisionRadius;

    return true;
}

void Ground::update(float dt)
{
	m_position.x -= 200 * dt;
	if (m_position.x < -1000)
		m_isKilled = true;
}

void Ground::render(sf::RenderTarget& target) const
{
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}

void Ground::setScale(const sf::Vector2f& scale)
{
	m_scale = scale;
	m_pSprite->setScale(m_scale);
}

inline float VecLength(sf::Vector2f vec)
{
	return sqrtf(vec.x*vec.x + vec.y*vec.y);
}

// This doesnt work if there would be alternative route above the palyer
float Ground::distanceTo(Player* pOther)
{
	sf::FloatRect groundBounds = getGlobalBounds();
	sf::FloatRect playerBounds = pOther->getGlobalBounds();

	if (!((playerBounds.position.x + playerBounds.size.x > groundBounds.position.x) &&
		(playerBounds.position.x < groundBounds.position.x + groundBounds.size.x)))
			return 1000000;

	float groundTopY = groundBounds.position.y;
	float playerBottomY = playerBounds.position.y + playerBounds.size.y;
	float distance = groundTopY - playerBottomY;

	return distance;
}

bool Ground::isCollided(Player* pOther)
{
	sf::FloatRect playerBounds = pOther->getGlobalBounds();
	sf::FloatRect groundBounds = getGlobalBounds();

	if (playerBounds.findIntersection(groundBounds) == std::nullopt)
		return false;
	else
		return true;
}
