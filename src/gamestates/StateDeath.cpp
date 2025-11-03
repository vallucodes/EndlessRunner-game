#include "StateDeath.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

StateDeath::StateDeath(StateStack& stateStack)
    : m_stateStack(stateStack)
{

}

bool StateDeath::init()
{
    m_pPrevState = m_stateStack.getCurrentState();
	m_timer.restart();
    return true;
}

void StateDeath::update(float dt)
{
    (void)dt;
	if (m_timer.getElapsedTime().asSeconds() >= m_deathDelayTime)
	{
		m_stateStack.popDeferred();
		m_stateStack.popDeferred();
	}
}

void StateDeath::render(sf::RenderTarget& target) const
{
	if (m_pPrevState != nullptr)
		m_pPrevState->render(target);
}
