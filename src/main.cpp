#include "ResourceManager.h"
#include "gamestates/StateStack.h"
#include "gamestates/IState.h"
#include "gamestates/StateMenu.h"
#include "Constants.h"
#include <memory>
#include <stack>
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

// --- NEW FUNCTION: DRAW GRID ---
void drawGrid(sf::RenderWindow& window, float spacing)
{
    const sf::Vector2u windowSize = window.getSize();
    const float thickness = 1.0f; // Thickness of the grid lines
    const sf::Color color = sf::Color(100, 100, 100, 150); // Gray with some transparency

    // 1. Draw Vertical Lines
    for (float x = spacing; x < windowSize.x; x += spacing)
    {
        sf::RectangleShape line({thickness, (float)windowSize.y});
        line.setPosition({x, 0.0f});
        line.setFillColor(color);
        window.draw(line);
    }

    // 2. Draw Horizontal Lines
    for (float y = spacing; y < windowSize.y; y += spacing)
    {
        sf::RectangleShape line({(float)windowSize.x, thickness});
        line.setPosition({0.0f, y});
        line.setFillColor(color);
        window.draw(line);
    }
}
// --- END NEW FUNCTION ---


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

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsedTime = clock.restart();

        IState* pState = gamestates.getCurrentState();
        if (!pState) return -1;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                return 0;
            }
        }

        pState->update(elapsedTime.asSeconds());
        window.clear(sf::Color::Black);
        pState->render(window);

        // --- INTEGRATE GRID DRAWING HERE ---
        // Draw the grid with a 50-pixel spacing
        drawGrid(window, 50.0f);
        // ------------------------------------

        window.display();

        gamestates.performDeferredPops();
    }

    return 0;
}
