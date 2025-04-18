#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include "Entity.hpp"

class Ball;
class Obstacle;

class Game {
public:
    Game(unsigned int width = 600, unsigned int height = 600);
    ~Game() = default;
    
    void run();
    void addEntity(std::unique_ptr<Entity> entity);
    
private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void checkCollisions();
    void handleResize(unsigned int width, unsigned int height);
    sf::Vector2f mapPixelToCoords(const sf::Vector2i& pixelPos) const;
    void drawBackground();
    
    Ball* findBall();
    std::vector<Obstacle*> findObstacles();
    
    sf::RenderWindow window;
    sf::View gameView;
    sf::Vector2f originalSize;
    sf::Clock clock;
    std::vector<std::unique_ptr<Entity>> entities;
    bool running;
    
    // Background properties
    sf::RectangleShape tileShape;
    float tileSize;
    sf::Color lightGreenColor;
    sf::Color darkGreenColor;
}; 