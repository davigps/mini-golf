#include "ParticleSystem.hpp"
#include <chrono>
#include <cmath>

ParticleSystem::ParticleSystem() {
    // Initialize random number generator with time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
}

void ParticleSystem::createCollisionParticles(const sf::Vector2f& position, const sf::Vector2f& normal) {
    // Create 8-12 particles at the collision point
    std::uniform_int_distribution<int> countDist(8, 12);
    int particleCount = countDist(rng);
    
    // Random distributions for particle properties
    std::uniform_real_distribution<float> speedDist(50.f, 150.f);
    std::uniform_real_distribution<float> lifetimeDist(0.3f, 0.7f);
    std::uniform_real_distribution<float> sizeDist(1.5f, 3.5f);
    
    // Reflect normal slightly to spread particles in the bounce direction
    sf::Vector2f baseDirection = normal;
    
    for (int i = 0; i < particleCount; ++i) {
        // Generate random properties
        float speed = speedDist(rng);
        float lifetime = lifetimeDist(rng);
        float size = sizeDist(rng);
        
        // Generate a direction within a cone 
        sf::Vector2f direction = randomDirectionInCone(baseDirection, 60.f); // 60 degree spread
        sf::Vector2f velocity = direction * speed;
        
        // Create and add the particle
        auto particle = std::make_unique<Particle>(position, velocity, lifetime, size);
        particle->setColor(sf::Color::White);
        particles.push_back(std::move(particle));
    }
}

void ParticleSystem::createMovementParticles(const sf::Vector2f& position, const sf::Vector2f& direction) {
    // Create 15-20 green particles at the ball's position
    std::uniform_int_distribution<int> countDist(15, 20);
    int particleCount = countDist(rng);
    
    // Random distributions for particle properties
    std::uniform_real_distribution<float> speedDist(20.f, 80.f);
    std::uniform_real_distribution<float> lifetimeDist(0.4f, 0.8f);
    std::uniform_real_distribution<float> sizeDist(2.f, 4.f);
    
    // Create particles in the opposite direction of movement (trailing effect)
    sf::Vector2f baseDirection = -direction; // Opposite direction to create a trail
    
    // Create green particles with different shades
    std::uniform_int_distribution<int> greenShadeDist(150, 255);
    
    for (int i = 0; i < particleCount; ++i) {
        // Generate random properties
        float speed = speedDist(rng);
        float lifetime = lifetimeDist(rng);
        float size = sizeDist(rng);
        
        // Generate a direction within a wider cone for more spread
        sf::Vector2f particleDir = randomDirectionInCone(baseDirection, 90.f); // 90 degree spread
        sf::Vector2f velocity = particleDir * speed;
        
        // Create and add the particle with a random shade of green
        auto particle = std::make_unique<Particle>(position, velocity, lifetime, size);
        
        // Create a random shade of green
        int greenShade = greenShadeDist(rng);
        sf::Color greenColor(0, greenShade, 0);
        particle->setColor(greenColor);
        
        particles.push_back(std::move(particle));
    }
}

void ParticleSystem::createTrailParticles(const sf::Vector2f& position, const sf::Vector2f& direction, float speed) {
    // Create fewer particles for continuous trails to avoid overwhelming the system
    int particleCount = 2 + static_cast<int>(speed / 50.0f); // More particles for faster speed
    particleCount = std::min(particleCount, 5); // Cap at 5 particles per update
    
    // Random distributions for particle properties
    std::uniform_real_distribution<float> offsetDist(-5.f, 5.f);
    std::uniform_real_distribution<float> speedDist(10.f, 30.f);
    std::uniform_real_distribution<float> lifetimeDist(0.2f, 0.5f);
    std::uniform_real_distribution<float> sizeDist(1.5f, 3.f);
    
    // Create particles in the opposite direction of movement (trailing effect)
    sf::Vector2f baseDirection = -direction; // Opposite direction to create a trail
    
    // Create green particles with different shades
    std::uniform_int_distribution<int> greenShadeDist(150, 255);
    
    for (int i = 0; i < particleCount; ++i) {
        // Generate random properties
        float particleSpeed = speedDist(rng);
        float lifetime = lifetimeDist(rng);
        float size = sizeDist(rng);
        
        // Add a slight random offset to the position to create a wider trail
        sf::Vector2f offset(offsetDist(rng), offsetDist(rng));
        sf::Vector2f particlePos = position + offset;
        
        // Generate a direction with a narrow cone for more focused trail
        sf::Vector2f particleDir = randomDirectionInCone(baseDirection, 30.f); // 30 degree spread
        sf::Vector2f velocity = particleDir * particleSpeed;
        
        // Create and add the particle with a random shade of green
        auto particle = std::make_unique<Particle>(particlePos, velocity, lifetime, size);
        
        // Create a random shade of green
        int greenShade = greenShadeDist(rng);
        sf::Color greenColor(0, greenShade, 0, 200); // Slightly transparent
        particle->setColor(greenColor);
        
        particles.push_back(std::move(particle));
    }
}

void ParticleSystem::update(float deltaTime) {
    // Update all particles
    for (auto& particle : particles) {
        particle->update(deltaTime);
    }
    
    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const std::unique_ptr<Particle>& p) { return !p->isAlive(); }),
        particles.end()
    );
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    // Draw all particles
    for (auto& particle : particles) {
        particle->draw(window);
    }
}

sf::Vector2f ParticleSystem::randomDirectionInCone(const sf::Vector2f& baseDirection, float spreadAngle) {
    // Convert spread angle to radians
    float spreadRadians = spreadAngle * 3.14159f / 180.f;
    
    // Generate a random angle within the spread
    std::uniform_real_distribution<float> angleDist(-spreadRadians/2, spreadRadians/2);
    float angle = angleDist(rng);
    
    // Normalize base direction
    float length = std::sqrt(baseDirection.x * baseDirection.x + baseDirection.y * baseDirection.y);
    sf::Vector2f normalizedBase = length > 0 ? baseDirection / length : sf::Vector2f(0, -1);
    
    // Calculate direction using rotation matrix
    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);
    
    sf::Vector2f result;
    result.x = normalizedBase.x * cos_angle - normalizedBase.y * sin_angle;
    result.y = normalizedBase.x * sin_angle + normalizedBase.y * cos_angle;
    
    return result;
} 