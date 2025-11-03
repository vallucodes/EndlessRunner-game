#pragma once

#include "IState.h"
#include <memory>
#include <SFML/System/Clock.hpp>

namespace sf { class Text; };

class StateDeath : public IState
{
	public:
		StateDeath(StateStack& stateStack);
		~StateDeath() = default;

		bool init() override;
		void update(float dt) override;
		void render(sf::RenderTarget& target) const override;

	public:

		StateStack& m_stateStack;
		sf::Clock m_timer;
		const float m_deathDelayTime = 1.0f;
		const IState* m_pPrevState = nullptr;
};
