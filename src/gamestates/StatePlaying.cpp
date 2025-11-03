#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateDeath.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include "../Constants.h"
#include <memory>
#include <iostream>
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>

StatePlaying::StatePlaying(StateStack& stateStack)
	: m_stateStack(stateStack)
{
}

bool StatePlaying::init()
{
	m_ground.setSize({ScreenWidth, Ground_height});
	m_ground.setPosition({0.0f, Ground_height});
	m_ground.setFillColor(sf::Color::Green);


	m_pPlayer = std::make_unique<Player>(this);
	if (!m_pPlayer || !m_pPlayer->init())
		return false;

	m_pPlayer->setPosition(sf::Vector2f(200, Ground_height - 200));

	std::unique_ptr<Ground> pStartingGround = std::make_unique<Ground>();
	if (pStartingGround->init())
	{
		pStartingGround->setScale(sf::Vector2f(80.0f, 3.0f));
		sf::FloatRect bounds = pStartingGround->getGlobalBounds();

		float posX = ScreenWidth / 2.0f;
		float posY = Ground_height + bounds.size.y / 2.0f;

		pStartingGround->setPosition(sf::Vector2f(posX, posY));

		m_grounds.push_back(std::move(pStartingGround));
	}

	return true;
}

void StatePlaying::update(float dt)
{
	m_timeUntilEnemySpawn -= dt;
	m_timeUntilGroundSpawn -= dt;

	// std::cout << "m_timeUntilEnemySpawn: " << m_timeUntilEnemySpawn <<  std::endl;
	// std::cout << "m_timeUntilGroundSpawn: " << m_timeUntilGroundSpawn <<  std::endl;

	if (m_timeUntilEnemySpawn < 0.0f)
	{
		m_timeUntilEnemySpawn = enemySpawnInterval;
		std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
		if (pEnemy->init())
		{
			sf::FloatRect bounds = pEnemy->getGlobalBounds();
			pEnemy->setPosition(sf::Vector2f(ScreenWidth, Ground_height + bounds.size.y / 2));
			m_enemies.push_back(std::move(pEnemy));
		}
	}

	if (m_timeUntilGroundSpawn < 0.0f)
	{
		m_timeUntilGroundSpawn = groundSpawnInterval;
		std::unique_ptr<Ground> pGround = std::make_unique<Ground>();
		if (pGround->init())
		{
			sf::FloatRect bounds = pGround->getGlobalBounds();
			float yOffset = m_groundAmplitude * std::sin(m_waveOffset * m_groundFrequency);
			m_waveOffset += 2.0f;
			float yPos = Ground_height + bounds.size.y / 2.0f - yOffset;
			pGround->setPosition(sf::Vector2f(ScreenWidth, yPos));
			m_grounds.push_back(std::move(pGround));
		}
	}

	bool isPauseKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
	m_hasPauseKeyBeenReleased |= !isPauseKeyPressed;
	if (m_hasPauseKeyBeenReleased && isPauseKeyPressed)
	{
		m_hasPauseKeyBeenReleased = false;
		m_stateStack.push<StatePaused>();
	}

	m_pPlayer->update(dt);

	for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
	{
		pEnemy->update(dt);
	}

	for (const std::unique_ptr<Ground>& pGround : m_grounds)
	{
		pGround->update(dt);
	}

	// Detect collisions with enemies
	for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
	{
		float distance = (m_pPlayer->getPosition() - pEnemy->getPosition()).lengthSquared();
		float minDistance = std::pow(Player::collisionRadius + pEnemy->getCollisionRadius(), 2.0f);
		const sf::Vector2f playerPosition = m_pPlayer->getPosition();

		if (distance <= minDistance)
		{
			m_pPlayer->m_isDead = true;
			break;
		}
	}

	// Detect collisions with ground
	for (const std::unique_ptr<Ground>& pGround : m_grounds)
	{
		// float distance = (m_pPlayer->getPosition() - pGround->getPosition()).lengthSquared();
		// float minDistance = std::pow(Player::collisionRadius + pGround->getCollisionRadius(), 2.0f);
		// const sf::Vector2f playerPosition = m_pPlayer->getPosition();
		if (pGround.get()->isCollided(m_pPlayer.get()))
		{
			m_pPlayer->m_isDead = true;
			break;
		}
		// if (distance <= minDistance)
		// {
		// 	m_pPlayer->m_isDead = true;
		// 	break;
		// }
	}

	// End Playing State on player death
	if (m_pPlayer->m_isDead)
	{
		m_stateStack.push<StateDeath>();
	}
}

void StatePlaying::render(sf::RenderTarget& target) const
{
	target.draw(m_ground);
	for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
		pEnemy->render(target);
	for (const std::unique_ptr<Ground>& pGround : m_grounds)
		pGround->render(target);
	m_pPlayer->render(target);
}

std::vector<Ground*> StatePlaying::getGrounds() const
{
	std::vector<Ground*> pRectangles;

	for (auto& pGround : m_grounds)
	{
		pRectangles.push_back(pGround.get());
	}
	return (pRectangles);
}
