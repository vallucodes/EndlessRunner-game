#include "Ground.h"
#include "Player.h"
#include "ResourceManager.h"
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

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

float Ground::distanceTo(Player* pOther)
{
    // Get the scaled bounds for both objects (SFML 3.x naming)
    sf::FloatRect groundBounds = getGlobalBounds();
    sf::FloatRect playerBounds = pOther->getGlobalBounds();

    // 1. HORIZONTAL OVERLAP CHECK
    // This is the critical step to ensure the player is above THIS ground piece.
    bool horizontallyOverlapping =
        // Player's right edge is to the right of the ground's left edge
        (playerBounds.position.x + playerBounds.size.x > groundBounds.position.x) &&
        // Player's left edge is to the left of the ground's right edge
        (playerBounds.position.x < groundBounds.position.x + groundBounds.size.x);

    if (!horizontallyOverlapping)
    {
        // If there is no horizontal overlap, this ground piece is irrelevant.
        // Return a large positive distance to indicate no collision or contact.
        return 9999.0f;
    }

    // 2. VERTICAL SEPARATION CALCULATION (Only if horizontally aligned)

    // Y-coordinate of the Ground's top surface (top edge)
    float groundTopY = groundBounds.position.y;

    // Y-coordinate of the Player's bottom edge
    float playerBottomY = playerBounds.position.y + playerBounds.size.y;

    // The distance is the vertical gap: Ground Top Y - Player Bottom Y.
    // Result:
    //   > 0: Player is ABOVE the ground (e.g., distance = 10 means 10px above)
    //   = 0: Player is TOUCHING the ground
    //   < 0: Player is PENETRATING the ground (overlap)
    float verticalDistance = groundTopY - playerBottomY;

    return verticalDistance;
}

#include <SFML/Graphics.hpp>

bool Ground::isCollided(Player* pOther)
{
	sf::FloatRect playerBounds = pOther->getGlobalBounds();
	sf::FloatRect groundBounds = getGlobalBounds();

	if (playerBounds.findIntersection(groundBounds) == std::nullopt)
		return false;
	else
		return true;
}
