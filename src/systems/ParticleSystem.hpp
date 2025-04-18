#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../entities/Particle.hpp"
#include <random>

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem() = default;
    
    // Create particles at collision point
    void createCollisionParticles(const sf::Vector2f& position, const sf::Vector2f& normal);
    
    // Update and draw particles
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
private:
    std::vector<std::unique_ptr<Particle>> particles;
    std::mt19937 rng;
    
    // Generate a random vector within a cone
    sf::Vector2f randomDirectionInCone(const sf::Vector2f& baseDirection, float spreadAngle);
}; 