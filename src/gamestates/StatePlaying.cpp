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
	m_pPlayer = std::make_unique<Player>(this);
	if (!m_pPlayer || !m_pPlayer->init())
		return false;

	m_pPlayer->setPosition(sf::Vector2f(200, Ground_height));

	std::unique_ptr<Ground> pStartingGround = std::make_unique<Ground>();
	if (pStartingGround->init())
	{
		pStartingGround->setScale(sf::Vector2f(80.0f, 3.0f));

		float posX = ScreenWidth / 2.0f;
		float posY = Ground_height + 100;

		pStartingGround->setPosition(sf::Vector2f(posX, posY));

		m_grounds.push_back(std::move(pStartingGround));
	}

	return true;
}

void StatePlaying::update(float dt)
{
	m_totalPassedTime += dt;
	m_timeUntilEnemySpawn -= dt;
	m_timeUntilGroundSpawn -= dt;
	m_timeUntilBigEnemySpawn -= dt;

	// Spawn enemies
	if (m_timeUntilEnemySpawn < 0.0f)
	{
		m_timeUntilEnemySpawn = enemySpawnInterval;
		std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
		sf::Vector2f scale = m_scaleFactors;
		if (m_timeUntilBigEnemySpawn < 0.0f && m_enemyLvlUps < 3)
		{
			m_enemyLvlUps++;
			if (m_enemyLvlUps % 2)
				m_scaleFactors = sf::Vector2f(m_scaleFactors.x, m_scaleFactors.y + 2.5f);
			else
				m_scaleFactors = sf::Vector2f(m_scaleFactors.x + 2.5f, m_scaleFactors.y);
			m_timeUntilBigEnemySpawn = enemyLvlInterval;
		}
		if (pEnemy->init(scale))
		{
			sf::FloatRect bounds = pEnemy->getGlobalBounds();
			float yOffset = m_groundAmplitude * std::sin(m_waveOffsetGround * m_groundFrequency);
			float yPos = Ground_height + bounds.size.y / 2.0f - yOffset - bounds.size.y;
			pEnemy->setPosition(sf::Vector2f(ScreenWidth, yPos));
			m_enemies.push_back(std::move(pEnemy));
		}
	}

	// Spawn ground and roof tiles
	if (m_timeUntilGroundSpawn < 0.0f)
	{
		m_timeUntilGroundSpawn = groundSpawnInterval;
		std::unique_ptr<Ground> pGround = std::make_unique<Ground>();
		if (pGround->init())
		{
			sf::FloatRect bounds = pGround->getGlobalBounds();

			float horizontalDistancePerTile = 2.0f * groundSpawnInterval;
			if (m_totalPassedTime > 10)
				horizontalDistancePerTile = 8.0f * groundSpawnInterval;
			if (m_totalPassedTime > 20)
				horizontalDistancePerTile = 15.0f * groundSpawnInterval;
			m_waveOffsetGround += horizontalDistancePerTile;

			float yOffset = m_groundAmplitude * std::sin(m_waveOffsetGround * m_groundFrequency);
			float yPos = Ground_height + bounds.size.y / 2.0f - yOffset;

			pGround->setPosition(sf::Vector2f(ScreenWidth, yPos));
			m_grounds.push_back(std::move(pGround));
		}

		if (m_totalPassedTime > 0)
		{
			std::unique_ptr<Ground> pRoof = std::make_unique<Ground>();
			if (pRoof->init())
			{
				float horizontalDistancePerTile = 5.0f * groundSpawnInterval;
				m_waveOffsetRoof += horizontalDistancePerTile;
				float ySinOffset = m_roofAmplitude * std::sin(m_waveOffsetRoof * m_groundFrequency);
				float yPos = 50 - ySinOffset + m_yOffset;

				pRoof->setPosition(sf::Vector2f(ScreenWidth, yPos));
				m_roofs.push_back(std::move(pRoof));
			}
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
		pEnemy->update(dt);

	for (const std::unique_ptr<Ground>& pGround : m_grounds)
		pGround->update(dt);

	for (const std::unique_ptr<Ground>& pRoof : m_roofs)
		pRoof->update(dt);

	updateCollisionsAndDead();
}

void StatePlaying::updateCollisionsAndDead() {
	// Detect player collisions with enemies
	for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
	{
		if (pEnemy.get()->isCollided(m_pPlayer.get()))
		{
			m_pPlayer->m_isDead = true;
			break;
		}
	}

	// Detect player collisions with ground
	for (const std::unique_ptr<Ground>& pGround : m_grounds)
	{
		if (pGround.get()->isCollided(m_pPlayer.get()))
		{
			m_pPlayer->m_isDead = true;
			break;
		}
	}

	// Detect player collisions with roof
	for (const std::unique_ptr<Ground>& pRoof : m_roofs)
	{
		if (pRoof.get()->isCollided(m_pPlayer.get()))
		{
			m_pPlayer->m_isDead = true;
			break;
		}
	}

	// Remove ground out of scope
	long unsigned int i = 0;
	while (i < m_grounds.size())
	{
		if (m_grounds[i]->isKilled())
		{
			std::swap(m_grounds[i], m_grounds.back());
			m_grounds.pop_back();
			continue;
		}
		i++;
	}

	// Remove enemies out of scope
	i = 0;
	while (i < m_enemies.size())
	{
		if (m_enemies[i]->isKilled())
		{
			std::swap(m_enemies[i], m_enemies.back());
			m_enemies.pop_back();
			continue;
		}
		i++;
	}

	// End Playing State on player death
	if (m_pPlayer->m_isDead)
	{
		m_totalPassedTime = 0;
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
	for (const std::unique_ptr<Ground>& pRoof : m_roofs)
		pRoof->render(target);
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
