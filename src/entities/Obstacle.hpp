#pragma once

#include <SFML/Graphics.hpp>
#include "../Entity.hpp"

class Obstacle : public Entity {
public:
    Obstacle(const sf::Vector2f& position, const sf::Vector2f& size, 
             const sf::Color& color = sf::Color(100, 100, 100));
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    
    // Optional: Add collision methods
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
}; 