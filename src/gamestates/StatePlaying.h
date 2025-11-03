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
		static constexpr const float enemySpawnInterval = 11.0f;
		static constexpr const float groundSpawnInterval = 0.1f;

		static constexpr float m_groundAmplitude = 100.0f;
		static constexpr float m_groundFrequency = 0.05f;

		sf::Clock m_deathTimer;

		float m_waveOffset = 0.0f;

		float m_timeUntilEnemySpawn = enemySpawnInterval;
		float m_timeUntilGroundSpawn = groundSpawnInterval;

		StateStack& m_stateStack;

		std::unique_ptr<Player> m_pPlayer;
		std::vector<std::unique_ptr<Enemy>> m_enemies;
		std::vector<std::unique_ptr<Ground>> m_grounds;

		sf::RectangleShape m_ground;
		bool m_hasPauseKeyBeenReleased = true;

		void updateCollisions();
};
