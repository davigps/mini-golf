#include <SFML/Graphics.hpp>
#include <memory>
#include "Game.hpp"
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
        sf::Color(139, 69, 19) // Light brown
    ));
    
    // Top wall
    game.addEntity(std::make_unique<Obstacle>(
        sf::Vector2f(300.f, 50.f),
        sf::Vector2f(500.f, 20.f),
        sf::Color(50, 150, 50) // Green
    ));
    
    // Bottom wall
    game.addEntity(std::make_unique<Obstacle>(
        sf::Vector2f(300.f, 550.f),
        sf::Vector2f(500.f, 20.f),
        sf::Color(50, 150, 50) // Green
    ));
    
    // Left wall
    game.addEntity(std::make_unique<Obstacle>(
        sf::Vector2f(50.f, 300.f),
        sf::Vector2f(20.f, 500.f),
        sf::Color(50, 150, 50) // Green
    ));
    
    // Right wall
    game.addEntity(std::make_unique<Obstacle>(
        sf::Vector2f(550.f, 300.f),
        sf::Vector2f(20.f, 500.f),
        sf::Color(50, 150, 50) // Green
    ));
    
    // Diagonal obstacle
    game.addEntity(std::make_unique<Obstacle>(
        sf::Vector2f(200.f, 200.f),
        sf::Vector2f(150.f, 30.f),
        sf::Color(120, 60, 12) // Dark brown
    ));
    
    // Run the game
    game.run();
    
    return 0;
}
