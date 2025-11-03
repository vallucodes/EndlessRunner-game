#pragma once

#include "IState.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Ground.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

class StatePlaying : public IState
{
	public:
		StatePlaying(StateStack& stateStack);
		~StatePlaying() = default;

		bool init() override;
		void update(float dt) override;
		void render(sf::RenderTarget& target) const override;


		std::vector<Ground*> getGrounds() const;

		// Player* getPlayer();

	private:
		static constexpr const float enemySpawnInterval = 3.0f;
		static constexpr const float groundSpawnInterval = 0.1f;
		static constexpr const float roofLvlInterval = 3.0f;

		static constexpr float m_groundAmplitude = 100.0f;
		float m_roofAmplitude = 100.0f;
		static constexpr float m_groundFrequency = 0.05f;

		sf::Clock m_deathTimer;
		float m_totalPassedTime;

		float m_waveOffsetGround = 100.0f;
		float m_waveOffsetRoof = 300.0f;

		float m_timeUntilEnemySpawn = 5.0f;
		float m_timeUntilGroundSpawn = groundSpawnInterval;
		float m_timeUntilBigEnemySpawn = 15.0f;
		float m_timeUntilRoofLvlUp = 5.0f;

		StateStack& m_stateStack;

		std::unique_ptr<Player> m_pPlayer;
		std::vector<std::unique_ptr<Enemy>> m_enemies;
		std::vector<std::unique_ptr<Ground>> m_grounds;
		std::vector<std::unique_ptr<Ground>> m_roofs;

		sf::RectangleShape m_ground;
		bool m_hasPauseKeyBeenReleased = true;

		void updateCollisions();
};
