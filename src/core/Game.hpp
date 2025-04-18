#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include <random>
#include "../utils/Entity.hpp"
#include "../utils/Colors.hpp"

// Forward declarations
class Ball;
class Obstacle;
class PhysicsSystem;
class InputHandler;
class ObstacleGenerator;
class ParticleSystem;

class Game {
public:
    Game(unsigned int width = 600, unsigned int height = 600);
    ~Game();
    
    void run();
    void addEntity(std::unique_ptr<Entity> entity);
    
    // Entity access methods
    Ball* findBall();
    std::vector<Obstacle*> findObstacles();
    
private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void handleResize(unsigned int width, unsigned int height);
    void drawBackground();
    
    // Helper method to find the closest standard aspect ratio
    float findClosestAspectRatio(float targetRatio);
    
    // Systems
    std::unique_ptr<PhysicsSystem> physicsSystem;
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<ObstacleGenerator> obstacleGenerator;
    std::unique_ptr<ParticleSystem> particleSystem;
    
    sf::RenderWindow window;
    sf::View gameView;
    sf::Vector2f originalSize;
    sf::Vector2f gameViewSize;  // Stores the aspect ratio view dimensions
    sf::Clock clock;
    std::vector<std::unique_ptr<Entity>> entities;
    bool running;
    
    // Background tiles
    sf::RectangleShape tileShape;
    float tileSize;
    float closestRatio;
}; 