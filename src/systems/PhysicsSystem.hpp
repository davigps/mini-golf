#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class Ball;
class Obstacle;
class Entity;

// Physics system responsible for handling collisions and physics-related behavior
class PhysicsSystem {
public:
    PhysicsSystem();
    ~PhysicsSystem() = default;
    
    // Update physics for all entities
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float deltaTime);
    
    // Handle specific collision between ball and obstacles
    void checkCollisions(Ball* ball, const std::vector<Obstacle*>& obstacles);
    
private:
    // Physics parameters
    const float gravity = 0.0f;
    const float friction = 0.99f;
}; 