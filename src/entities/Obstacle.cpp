#include "Obstacle.hpp"

Obstacle::Obstacle(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Color& color) {
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(color);
    shape.setOrigin({size.x / 2.f, size.y / 2.f});
}

void Obstacle::update(float deltaTime) {
    // Obstacles are static, so no update logic needed
}

void Obstacle::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Obstacle::getBounds() const {
    return shape.getGlobalBounds();
} 