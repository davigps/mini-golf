#include <SFML/Graphics.hpp>
#include <memory>
#include "core/Game.hpp"
#include "utils/Colors.hpp"
#include "entities/Ball.hpp"
#include "entities/Obstacle.hpp"

int main()
{
    Game game;
    
    // Add a ball to the game
    auto ball = std::make_unique<Ball>();
    game.addEntity(std::move(ball));
    
    // Run the game - obstacles will be generated dynamically
    game.run();
    
    return 0;
}
