#include <SFML/Graphics.hpp>
#include "Ball.hpp"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({600u, 600u}), "Mini Golf");
    window.setFramerateLimit(144);

    Ball ball;
    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::MouseButtonPressed>()) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    ball.handleMousePress(sf::Vector2f(
                        static_cast<float>(sf::Mouse::getPosition(window).x),
                        static_cast<float>(sf::Mouse::getPosition(window).y)
                    ));
                }
            }
            if (event->is<sf::Event::MouseButtonReleased>()) {
                ball.handleMouseRelease(sf::Vector2f(
                    static_cast<float>(sf::Mouse::getPosition(window).x),
                    static_cast<float>(sf::Mouse::getPosition(window).y)
                ));
            }
            if (event->is<sf::Event::MouseMoved>()) {
                ball.handleMouseMove(sf::Vector2f(
                    static_cast<float>(sf::Mouse::getPosition(window).x),
                    static_cast<float>(sf::Mouse::getPosition(window).y)
                ));
            }
        }

        auto deltaTime = clock.restart().asSeconds();
        ball.update(deltaTime);

        window.clear();
        ball.draw(window);
        window.display();
    }
}
