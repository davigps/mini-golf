#include "PhysicsSystem.hpp"
#include "../entities/Ball.hpp"
#include "../entities/Obstacle.hpp"
#include <memory>

PhysicsSystem::PhysicsSystem() {
    // Initialize physics system
}

void PhysicsSystem::update(const std::vector<std::unique_ptr<Entity>>& entities, float deltaTime) {
    // Physics update for each entity
    for (const auto& entity : entities) {
        entity->update(deltaTime);
    }
}

void PhysicsSystem::checkCollisions(Ball* ball, const std::vector<Obstacle*>& obstacles) {
    if (!ball) return;
    
    // Check ball collision against all obstacles
    for (auto obstacle : obstacles) {
        ball->checkCollision(*obstacle);
    }
} 