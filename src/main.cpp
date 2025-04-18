#include <SFML/Graphics.hpp>
#include <memory>
#include "Game.hpp"
#include "Colors.hpp"
#include "entities/Ball.hpp"
#include "entities/Obstacle.hpp"

int main()
{
    Game game;
    
    // Add a ball to the game
    auto ball = std::make_unique<Ball>();
    game.addEntity(std::move(ball));
    
    // Create a simple mini-golf course with obstacles
    
    // Central obstacle
    game.addEntity(std::make_unique<Obstacle>(
        sf::Vector2f(400.f, 300.f),
        sf::Vector2f(100.f, 50.f),
        Colors::LightBrown
    ));
    
    // Diagonal obstacle
    game.addEntity(std::make_unique<Obstacle>(
        sf::Vector2f(200.f, 200.f),
        sf::Vector2f(150.f, 30.f),
        Colors::LightBrown
    ));
    
    // Run the game
    game.run();
    
    return 0;
}
