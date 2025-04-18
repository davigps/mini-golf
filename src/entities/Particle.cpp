#include "Particle.hpp"
#include <cmath>

Particle::Particle(const sf::Vector2f& position, const sf::Vector2f& velocity, float lifetime, float size)
    : position(position)
    , velocity(velocity)
    , remainingLifetime(lifetime)
    , initialLifetime(lifetime)
    , size(size)
{
    shape.setRadius(size);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin({size, size});
    shape.setPosition(position);
}

void Particle::setColor(const sf::Color& color) {
    shape.setFillColor(color);
}

void Particle::update(float deltaTime) {
    // Update lifetime
    remainingLifetime -= deltaTime;
    
    // Update position based on velocity
    position += velocity * deltaTime;
    
    // Apply a little gravity and drag
    velocity.y += 50.f * deltaTime;  // Slight downward acceleration
    velocity *= 0.98f; // Air drag
    
    // Update visual properties
    shape.setPosition(position);
    
    // Fade out as lifetime decreases
    float alpha = (remainingLifetime / initialLifetime) * 255.f;
    sf::Color color = shape.getFillColor();
    color.a = static_cast<uint8_t>(alpha);
    shape.setFillColor(color);
    
    // Shrink slightly as lifetime decreases
    float scale = 0.8f + (remainingLifetime / initialLifetime) * 0.2f;
    shape.setRadius(size * scale);
    shape.setOrigin({size * scale, size * scale});
}

void Particle::draw(sf::RenderWindow& window) {
    if (isAlive()) {
        window.draw(shape);
    }
}

void Particle::drawShadow(sf::RenderWindow& window) {
    // Particles are small and short-lived, no need for shadows
} 