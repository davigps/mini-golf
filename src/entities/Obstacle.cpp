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
    // Draw the actual obstacle
    window.draw(shape);
}

void Obstacle::drawShadow(sf::RenderWindow& window) {
    // Draw shadow (slightly offset and semi-transparent black)
    sf::RectangleShape shadow = shape;
    shadow.setPosition({shape.getPosition().x + 5.f, shape.getPosition().y + 5.f});
    shadow.setFillColor(sf::Color(0, 0, 0, 70));  // Semi-transparent black
    window.draw(shadow);
}

sf::FloatRect Obstacle::getBounds() const {
    return shape.getGlobalBounds();
} 