#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <memory>

class Entity;
class Ball;
class Obstacle;

// ObstacleGenerator responsible for generating random obstacles
class ObstacleGenerator {
public:
    ObstacleGenerator();
    ~ObstacleGenerator() = default;
    
    // Generate obstacles based on ball position
    void generateObstacles(const sf::Vector2f& ballPosition, 
                          std::vector<std::unique_ptr<Entity>>& entities,
                          const std::vector<Obstacle*>& existingObstacles);
    
    // Check if a position is valid for a new obstacle
    bool isValidObstaclePosition(const sf::Vector2f& pos, 
                                const sf::Vector2f& size, 
                                const sf::Vector2f& ballPosition,
                                float ballRadius,
                                const std::vector<Obstacle*>& obstacles);
    
    // Track the last generation position to avoid generating too frequently
    void updateLastGenerationPosition(const sf::Vector2f& position);
    
    // Check if new obstacles should be generated based on distance moved
    bool shouldGenerateObstacles(const sf::Vector2f& currentPosition) const;
    
private:
    std::mt19937 rng;
    sf::Vector2f lastGenerationPos;
    
    // Configuration parameters
    float obstacleGenerationDistance;
    float minObstacleDistance;
    int maxObstacleCount;
}; 