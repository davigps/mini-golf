#pragma once

#include <SFML/Graphics.hpp>

class Entity {
public:
    virtual ~Entity() = default;
    
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    
    virtual bool handleMousePress(const sf::Vector2f& mousePos) { return false; }
    virtual bool handleMouseRelease(const sf::Vector2f& mousePos) { return false; }
    virtual bool handleMouseMove(const sf::Vector2f& mousePos) { return false; }
}; 