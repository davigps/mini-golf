#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <memory>
#include <queue>

class Entity;
class Ball;
class Obstacle;

// Path segment structure
struct PathSegment {
    sf::Vector2f start;
    sf::Vector2f end;
    float width;
};

// ObstacleGenerator responsible for generating random path walls
class ObstacleGenerator {
public:
    ObstacleGenerator();
    ~ObstacleGenerator() = default;
    
    // Generate obstacles based on ball position to form a path
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
    // Generate a new path segment ahead of the ball
    std::vector<PathSegment> generatePathSegments(const sf::Vector2f& ballPosition);
    
    // Create wall obstacles from path segments
    void createWallsFromPath(const std::vector<PathSegment>& segments, 
                             std::vector<std::unique_ptr<Entity>>& entities,
                             const sf::Vector2f& ballPosition,
                             const std::vector<Obstacle*>& existingObstacles);
    
    std::mt19937 rng;
    sf::Vector2f lastGenerationPos;
    
    // Path state
    std::queue<PathSegment> pathQueue; // Store future path segments
    sf::Vector2f currentPathEnd;       // End point of the last generated path
    sf::Vector2f currentPathDirection; // Current direction of the path
    float currentPathWidth;            // Current width of the path
    
    // Configuration parameters
    float obstacleGenerationDistance;
    float minObstacleDistance;
    int maxObstacleCount;
    float maxPathTurnAngle;
    float minPathSegmentLength;
    float maxPathSegmentLength;
}; 