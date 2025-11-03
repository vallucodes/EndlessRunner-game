#include "Player.h"
#include "ResourceManager.h"
#include "Constants.h"
#include "../gamestates/StatePlaying.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>
#include <algorithm>


Player::Player(StatePlaying* pState) : m_pState(pState)
{
}

bool Player::init()
{
	const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("some_yellow_thing.png");
	if (pTexture == nullptr)
		return false;

	m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
	if (!m_pSprite)
		return false;

	m_rotation = sf::degrees(0);
	sf::FloatRect localBounds = m_pSprite->getLocalBounds();
	m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
	m_pSprite->setPosition(m_position);
	m_pSprite->setScale(sf::Vector2f(3.0f, 3.0f));
	m_collisionRadius = collisionRadius;

	return true;
}

void Player::update(float dt)
{
	m_gas = std::min(m_gas + 60 * dt, 100.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		if (!(m_gas - 300 * dt < 0))
		{
			m_acceleration.y = PlayerVerticalAcceleration;
			m_gas = m_gas - 300 * dt;
		}
		else
			m_acceleration.y = 0;
	}
	else
		m_acceleration.y = 0;

	float repellingAcceleration = std::clamp(ReppellinForceCoefficient /  m_distance * Gravity, 0.0f, 1000.0f);
	float dampingAcceleration = m_velocity.y * DampingFactor;

	float total_Yacc = Gravity - m_acceleration.y - repellingAcceleration - dampingAcceleration;

	m_velocity.y += total_Yacc * dt;
	m_position.y += m_velocity.y * dt;

	// save distance to ground
	m_distance = 1000000;
	auto pGrounds = m_pState->getGrounds();
	for (auto& pGround : pGrounds)
	{
		if (pGround->distanceTo(this) < m_distance)
			m_distance = pGround->distanceTo(this);
	}

	if (m_distance < 0.0f)
	{
		m_isDead = true;
	}
}

void Player::render(sf::RenderTarget& target) const
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && m_acceleration.y > 0)
		drawGas(target);
	m_pSprite->setRotation(m_rotation);
	m_pSprite->setPosition(m_position);
	target.draw(*m_pSprite);
}

void Player::drawGas(sf::RenderTarget &target) const {

	sf::RectangleShape gas(m_gasSize);
	gas.setFillColor(sf::Color::Red);
	gas.setOrigin(sf::Vector2f(0.0f, 0.0f));
	gas.setRotation(sf::degrees(45));
	sf::FloatRect playerPos = getGlobalBounds();
	float gasX = playerPos.position.x - 5.0f;
	float gasY = playerPos.position.y + playerPos.size.y / 2 - m_gasSize.y / 2 - 1;
	gas.setPosition(sf::Vector2f(gasX, gasY));
	target.draw(gas);
}

sf::FloatRect Player::getGlobalBounds() const
{
	return m_pSprite->getGlobalBounds();
}
