#include <SFML/Graphics.hpp>
#include "GameWorld.h"
#include "GameRender.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Zombie Chase Game");
    window.setFramerateLimit(60);

    GameWorld world;
    GameRender renderer;

    world.init();

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();

        world.update(dt, window);

        window.clear(sf::Color::Black);
        world.render(window); // ✅ Updated to pass window
        window.display();
    }

    return 0;
}
