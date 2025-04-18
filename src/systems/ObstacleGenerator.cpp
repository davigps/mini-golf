#include "ObstacleGenerator.hpp"
#include "../entities/Obstacle.hpp"
#include "../utils/Colors.hpp"
#include <chrono>
#include <cmath>

ObstacleGenerator::ObstacleGenerator()
    : lastGenerationPos(0.f, 0.f)
    , obstacleGenerationDistance(500.f)
    , minObstacleDistance(100.f)
    , maxObstacleCount(30)
{
    // Initialize random number generator with time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
}

void ObstacleGenerator::generateObstacles(const sf::Vector2f& ballPosition, 
                                        std::vector<std::unique_ptr<Entity>>& entities,
                                        const std::vector<Obstacle*>& existingObstacles) {
    // Don't generate if we already have the maximum number of obstacles
    if (existingObstacles.size() >= maxObstacleCount) return;
    
    // Define the generation area around the ball (focus on generating ahead)
    float genLeft = ballPosition.x + 300.f;  // Generate ahead of the ball
    float genTop = ballPosition.y - 600.f;
    float genWidth = 1200.f;
    float genHeight = 1200.f;
    
    // Random distributions
    std::uniform_real_distribution<float> xDist(genLeft, genLeft + genWidth);
    std::uniform_real_distribution<float> yDist(genTop, genTop + genHeight);
    std::uniform_real_distribution<float> sizeDist(40.f, 120.f);
    std::uniform_int_distribution<int> colorDist(0, 2);
    std::uniform_int_distribution<int> typeDist(0, 1);  // 0 for horizontal, 1 for vertical
    
    // Try to create 1-3 new obstacles
    std::uniform_int_distribution<int> countDist(1, 3);
    int newObstacleCount = countDist(rng);
    
    // Colors for obstacles
    sf::Color obstacleColors[] = {
        Colors::LightBrown,
        Colors::DarkBrown,
        Colors::Gray
    };
    
    // Keep track of the ball radius for collision checking
    float ballRadius = 20.f; // Default value, should be obtained from the actual ball
    
    for (int i = 0; i < newObstacleCount; i++) {
        // Generate random properties
        float x = xDist(rng);
        float y = yDist(rng);
        float width, height;
        
        if (typeDist(rng) == 0) { // Horizontal obstacle
            width = sizeDist(rng);
            height = sizeDist(rng) / 2.f;
        } else { // Vertical obstacle
            width = sizeDist(rng) / 2.f;
            height = sizeDist(rng);
        }
        
        sf::Vector2f position(x, y);
        sf::Vector2f size(width, height);
        
        // Only add if position is valid
        if (isValidObstaclePosition(position, size, ballPosition, ballRadius, existingObstacles)) {
            sf::Color color = obstacleColors[colorDist(rng)];
            entities.push_back(std::make_unique<Obstacle>(position, size, color));
        }
    }
    
    // Update the last generation position
    updateLastGenerationPosition(ballPosition);
}

bool ObstacleGenerator::isValidObstaclePosition(const sf::Vector2f& pos, 
                                              const sf::Vector2f& size, 
                                              const sf::Vector2f& ballPosition,
                                              float ballRadius,
                                              const std::vector<Obstacle*>& obstacles) {
    // Don't generate too close to the ball
    float distToBall = std::hypot(pos.x - ballPosition.x, pos.y - ballPosition.y);
    if (distToBall < minObstacleDistance + ballRadius) {
        return false;
    }
    
    // Check overlap with existing obstacles
    sf::FloatRect newObstacleBounds(
        {pos.x - size.x / 2.f, pos.y - size.y / 2.f}, 
        size
    );
    
    for (auto obstacle : obstacles) {
        sf::FloatRect existingBounds = obstacle->getBounds();
        
        // Add some padding to avoid obstacles being too close
        existingBounds.position.x -= 20.f;
        existingBounds.position.y -= 20.f;
        existingBounds.size.x += 40.f;
        existingBounds.size.y += 40.f;
        
        // Check if rectangles intersect
        bool doesIntersect = !(
            existingBounds.position.x + existingBounds.size.x < newObstacleBounds.position.x ||
            existingBounds.position.y + existingBounds.size.y < newObstacleBounds.position.y ||
            existingBounds.position.x > newObstacleBounds.position.x + newObstacleBounds.size.x ||
            existingBounds.position.y > newObstacleBounds.position.y + newObstacleBounds.size.y
        );
        
        if (doesIntersect) {
            return false;
        }
    }
    
    return true;
}

void ObstacleGenerator::updateLastGenerationPosition(const sf::Vector2f& position) {
    lastGenerationPos = position;
}

bool ObstacleGenerator::shouldGenerateObstacles(const sf::Vector2f& currentPosition) const {
    // Generate obstacles if we've moved far enough from the last generation point
    float distance = std::hypot(currentPosition.x - lastGenerationPos.x, 
                              currentPosition.y - lastGenerationPos.y);
    return distance > obstacleGenerationDistance;
} 