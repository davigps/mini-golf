#include "ObstacleGenerator.hpp"
#include "../entities/Obstacle.hpp"
#include "../utils/Colors.hpp"
#include <chrono>
#include <cmath>
#include <algorithm>

ObstacleGenerator::ObstacleGenerator()
    : lastGenerationPos(0.f, 0.f)
    , currentPathEnd(300.f, 300.f)
    , currentPathDirection(1.f, 0.f)  // Initial direction: right
    , currentPathWidth(200.f)         // Initial path width
    , obstacleGenerationDistance(300.f)
    , minObstacleDistance(100.f)
    , maxObstacleCount(100)
    , maxPathTurnAngle(45.f)
    , minPathSegmentLength(200.f)
    , maxPathSegmentLength(500.f)
{
    // Initialize random number generator with time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
}

void ObstacleGenerator::generateObstacles(const sf::Vector2f& ballPosition, 
                                        std::vector<std::unique_ptr<Entity>>& entities,
                                        const std::vector<Obstacle*>& existingObstacles) {
    // Don't generate if we already have too many obstacles
    if (existingObstacles.size() >= maxObstacleCount) return;
    
    // Initialize the path start if this is the first generation
    if (lastGenerationPos == sf::Vector2f(0.f, 0.f)) {
        currentPathEnd = ballPosition + sf::Vector2f(200.f, 0.f);
        currentPathDirection = sf::Vector2f(1.f, 0.f);
    }
    
    // Generate new path segments ahead of the ball
    std::vector<PathSegment> newSegments = generatePathSegments(ballPosition);
    
    // Create wall obstacles from path segments
    createWallsFromPath(newSegments, entities, ballPosition, existingObstacles);
    
    // Update the last generation position
    updateLastGenerationPosition(ballPosition);
}

std::vector<PathSegment> ObstacleGenerator::generatePathSegments(const sf::Vector2f& ballPosition) {
    std::vector<PathSegment> segments;
    
    // Random distributions for path properties
    std::uniform_real_distribution<float> angleDist(-maxPathTurnAngle, maxPathTurnAngle);
    std::uniform_real_distribution<float> lengthDist(minPathSegmentLength, maxPathSegmentLength);
    std::uniform_real_distribution<float> widthDist(180.f, 250.f);
    
    // Add 2-3 new path segments
    int segmentCount = 3;
    sf::Vector2f segmentStart = currentPathEnd;
    sf::Vector2f currentDir = currentPathDirection;
    
    for (int i = 0; i < segmentCount; i++) {
        // Determine segment properties
        float segmentLength = lengthDist(rng);
        float turnAngle = angleDist(rng);
        float pathWidth = widthDist(rng);
        
        // Apply rotation to direction vector using a rotation matrix
        float angleRad = turnAngle * 3.14159f / 180.f;
        float cos_a = std::cos(angleRad);
        float sin_a = std::sin(angleRad);
        
        sf::Vector2f newDir;
        newDir.x = currentDir.x * cos_a - currentDir.y * sin_a;
        newDir.y = currentDir.x * sin_a + currentDir.y * cos_a;
        
        // Normalize direction vector
        float length = std::sqrt(newDir.x * newDir.x + newDir.y * newDir.y);
        if (length > 0) {
            newDir = newDir / length;
        }
        
        // Calculate segment end point
        sf::Vector2f segmentEnd = segmentStart + newDir * segmentLength;
        
        // Create the path segment
        PathSegment segment;
        segment.start = segmentStart;
        segment.end = segmentEnd;
        segment.width = pathWidth;
        
        // Add the segment to the result
        segments.push_back(segment);
        
        // Prepare for the next segment
        segmentStart = segmentEnd;
        currentDir = newDir;
    }
    
    // Update the path state
    currentPathEnd = segmentStart;
    currentPathDirection = currentDir;
    currentPathWidth = segments.back().width;
    
    return segments;
}

void ObstacleGenerator::createWallsFromPath(const std::vector<PathSegment>& segments, 
                                           std::vector<std::unique_ptr<Entity>>& entities,
                                           const sf::Vector2f& ballPosition,
                                           const std::vector<Obstacle*>& existingObstacles) {
    // Colors for obstacles
    sf::Color wallColors[] = {
        Colors::LightBrown,
        Colors::DarkBrown,
        Colors::Gray
    };
    std::uniform_int_distribution<int> colorDist(0, 2);
    
    // Keep track of the ball radius for collision checking
    float ballRadius = 20.f;
    
    for (const auto& segment : segments) {
        // Calculate the normalized direction vector of the segment
        sf::Vector2f segmentDir = segment.end - segment.start;
        float segmentLength = std::sqrt(segmentDir.x * segmentDir.x + segmentDir.y * segmentDir.y);
        segmentDir = segmentDir / segmentLength;
        
        // Calculate the perpendicular vector
        sf::Vector2f perpDir(-segmentDir.y, segmentDir.x);
        
        // Calculate wall offset (half the path width)
        float halfWidth = segment.width / 2.f;
        
        // Calculate the four corners of the path segment
        sf::Vector2f leftStart = segment.start + perpDir * halfWidth;
        sf::Vector2f rightStart = segment.start - perpDir * halfWidth;
        sf::Vector2f leftEnd = segment.end + perpDir * halfWidth;
        sf::Vector2f rightEnd = segment.end - perpDir * halfWidth;
        
        // Create wall thickness
        float wallThickness = 20.f;
        
        // Create the left wall
        sf::Vector2f leftWallPos = (leftStart + leftEnd) / 2.f;
        sf::Vector2f leftWallSize(segmentLength, wallThickness);
        
        // Rotate the left wall
        float angle = std::atan2(segmentDir.y, segmentDir.x) * 180.f / 3.14159f;
        
        // Pick a random color
        sf::Color leftColor = wallColors[colorDist(rng)];
        
        // Create the obstacle entity
        auto leftWall = std::make_unique<Obstacle>(leftWallPos, leftWallSize, leftColor);
        
        // Set the rotation of the left wall
        leftWall->setRotation(angle);
        
        // Create the right wall
        sf::Vector2f rightWallPos = (rightStart + rightEnd) / 2.f;
        sf::Vector2f rightWallSize(segmentLength, wallThickness);
        
        // Pick a random color
        sf::Color rightColor = wallColors[colorDist(rng)];
        
        // Create the obstacle entity
        auto rightWall = std::make_unique<Obstacle>(rightWallPos, rightWallSize, rightColor);
        
        // Set the rotation of the right wall
        rightWall->setRotation(angle);
        
        // Add obstacles to the entities list
        if (isValidObstaclePosition(leftWallPos, leftWallSize, ballPosition, ballRadius, existingObstacles)) {
            entities.push_back(std::move(leftWall));
        }
        
        if (isValidObstaclePosition(rightWallPos, rightWallSize, ballPosition, ballRadius, existingObstacles)) {
            entities.push_back(std::move(rightWall));
        }
    }
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
    
    // Check overlap with existing obstacles (simplified check)
    // Instead of detailed rect intersection, just check center distance for performance
    for (auto obstacle : obstacles) {
        sf::FloatRect existingBounds = obstacle->getBounds();
        sf::Vector2f existingCenter = {
            existingBounds.position.x + existingBounds.size.x / 2.f,
            existingBounds.position.y + existingBounds.size.y / 2.f
        };
        
        float distToObstacle = std::hypot(pos.x - existingCenter.x, pos.y - existingCenter.y);
        float minDist = (size.x + size.y + existingBounds.size.x + existingBounds.size.y) / 4.f;
        
        if (distToObstacle < minDist) {
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