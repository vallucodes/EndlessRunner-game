#include "Enemy.h"
#include "Player.h"
#include "ResourceManager.h"
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

bool Enemy::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("enemy.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(2.5f, 2.5f));
    m_collisionRadius = collisionRadius;

    return true;
}

bool Enemy::init(sf::Vector2f scale)
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("enemy.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(scale.x, scale.y));
    m_collisionRadius = collisionRadius;

    return true;
}

void Enemy::update(float dt)
{
    m_position.x -= 200 * dt;
	if (m_position.x < -200)
		m_isKilled = true;
}

void Enemy::render(sf::RenderTarget& target) const
{
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}

bool Enemy::isCollided(Player* pOther)
{
	sf::FloatRect playerBounds = pOther->getGlobalBounds();
	sf::FloatRect enemyBounds = getGlobalBounds();

	if (playerBounds.findIntersection(enemyBounds) == std::nullopt)
		return false;
	else
		return true;
}
