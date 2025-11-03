#include "ResourceManager.h"
#include "gamestates/StateStack.h"
#include "gamestates/StatePaused.h"
#include "gamestates/StateDeath.h"
#include "gamestates/IState.h"
#include "gamestates/StateMenu.h"
#include "Constants.h"
#include <memory>
#include <stack>
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

int main(int argc, char* argv[])
{
	(void)argc;

	// ResourceManager must be instantiated here -- DO NOT CHANGE
	ResourceManager::init(argv[0]);

	sf::RenderWindow window(sf::VideoMode({ScreenWidth, ScreenHeight}), "Runner");
	window.setKeyRepeatEnabled(false);

	StateStack gamestates;
	if (!gamestates.push<StateMenu>())
		return -1;

	float timeScale = 1.0f;
	float totalElapsedTime = 0;

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();

		IState* pState = gamestates.getCurrentState();
		if (!pState) return -1;

		bool isPaused = (dynamic_cast<StatePaused*>(pState) != nullptr);
		bool isMenu = (dynamic_cast<StateMenu*>(pState) != nullptr);
		if (!isPaused && !isMenu)
		{
			totalElapsedTime += elapsedTime.asSeconds();;
			if (totalElapsedTime > 20.0f)
			{
				timeScale += 0.2f;
				totalElapsedTime = 0.0f;
			}
		}

		bool isDeath = (dynamic_cast<StateDeath*>(pState) != nullptr);
		if (isDeath)
		{
			timeScale = 1.0f;
			totalElapsedTime = 0.0f;
		}
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				return 0;
			}
		}
		float scaledTime = elapsedTime.asSeconds() * timeScale;
		pState->update(scaledTime);
		window.clear(sf::Color(20, 20, 20));
		pState->render(window);

		window.display();

		gamestates.performDeferredPops();
	}

	return 0;
}
