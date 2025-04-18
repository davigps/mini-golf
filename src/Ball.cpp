#include "Ball.hpp"

Ball::Ball(float radius) 
    : position(300.f, 300.f)
    , velocity(0.f, 0.f)
    , isDragging(false)
    , friction(0.99f)
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(sf::Vector2f(radius, radius));
    shape.setPosition(position);
}

void Ball::update(float deltaTime) {
    if (!isDragging) {
        velocity *= friction;
        position += velocity * deltaTime;
        
        // Keep ball within window bounds
        float radius = shape.getRadius();
        if (position.x < radius) {
            position.x = radius;
            velocity.x *= -0.5f;
        }
        if (position.x > 600 - radius) {
            position.x = 600 - radius;
            velocity.x *= -0.5f;
        }
        if (position.y < radius) {
            position.y = radius;
            velocity.y *= -0.5f;
        }
        if (position.y > 600 - radius) {
            position.y = 600 - radius;
            velocity.y *= -0.5f;
        }
    }
    
    shape.setPosition(position);
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Ball::handleMousePress(const sf::Vector2f& mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        isDragging = true;
        startDragPos = mousePos;
        velocity = sf::Vector2f(0.f, 0.f);
    }
}

void Ball::handleMouseRelease(const sf::Vector2f& mousePos) {
    if (isDragging) {
        isDragging = false;
        // Calculate velocity based on drag distance
        velocity = (startDragPos - mousePos) * 0.1f;
    }
}

void Ball::handleMouseMove(const sf::Vector2f& mousePos) {
    if (isDragging) {
        position = mousePos;
    }
} 