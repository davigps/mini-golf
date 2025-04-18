#pragma once

#include <SFML/Graphics.hpp>
#include "../utils/Entity.hpp"

class Particle : public Entity {
public:
    Particle(const sf::Vector2f& position, const sf::Vector2f& velocity, float lifetime = 0.5f, float size = 3.f);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    void drawShadow(sf::RenderWindow& window) override;
    
    bool isAlive() const { return remainingLifetime > 0.f; }
    
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float remainingLifetime;
    float initialLifetime;
    float size;
}; 