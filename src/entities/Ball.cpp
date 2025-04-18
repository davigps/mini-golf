#include "Ball.hpp"
#include "Obstacle.hpp"
#include "../Colors.hpp"
#include <cmath>

Ball::Ball(float radius) 
    : position(300.f, 300.f)
    , velocity(0.f, 0.f)
    , isDragging(false)
    , friction(0.99f)
{
    shape.setRadius(radius);
    shape.setFillColor(Colors::BallColor);
    shape.setOrigin(sf::Vector2f(radius, radius));
    shape.setPosition(position);
    
    // Initialize line for drag visualization
    line[0] = {position, Colors::DragLineColor};
    line[1] = {position, Colors::DragLineColor};
    
    // Initialize arrow head
    for (int i = 0; i < 3; i++) {
        arrowHead[i].color = Colors::DragLineColor;
    }
}

void Ball::update(float deltaTime) {
    if (!isDragging) {
        // Apply friction to slow down the ball
        velocity *= friction;
        
        // Stop the ball if it's moving very slowly
        if (std::abs(velocity.x) < 1.0f && std::abs(velocity.y) < 1.0f) {
            velocity = sf::Vector2f(0.f, 0.f);
        }
        
        // Update position based on velocity
        position += velocity * deltaTime;
    }
    
    // Update the ball's shape position
    shape.setPosition(position);
    
    // Update line start position to follow ball
    line[0].position = position;
}

void Ball::draw(sf::RenderWindow& window) {
    // Draw the actual ball
    window.draw(shape);
    
    // Draw the drag line when dragging
    if (isDragging) {
        window.draw(line, 2, sf::PrimitiveType::Lines);
        window.draw(arrowHead, 3, sf::PrimitiveType::Triangles);
    }
}

void Ball::drawShadow(sf::RenderWindow& window) {
    // Draw shadow (slightly larger, offset, and semi-transparent black)
    sf::CircleShape shadow = shape;
    shadow.setPosition({position.x + 6.f, position.y + 6.f});  // Offset shadow
    shadow.setFillColor(sf::Color(0, 0, 0, 70));  // Semi-transparent black
    shadow.setRadius(shape.getRadius() * 1.1f);   // Slightly smaller shadow
    window.draw(shadow);
}

bool Ball::handleMousePress(const sf::Vector2f& mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        isDragging = true;
        startDragPos = position; // Start from ball position, not mouse position
        currentDragPos = mousePos;
        velocity = sf::Vector2f(0.f, 0.f);
        
        // Update line positions
        line[0].position = position;
        line[1].position = mousePos;
        
        // Initialize arrowhead
        updateArrowHead();
        
        return true;
    }
    return false;
}

bool Ball::handleMouseRelease(const sf::Vector2f& mousePos) {
    if (isDragging) {
        isDragging = false;
        // Calculate velocity based on drag distance and direction
        // The direction is reversed (position - mousePos instead of mousePos - position)
        // Intensity is proportional to the distance
        sf::Vector2f dragVector = position - mousePos;
        float distance = std::sqrt(dragVector.x * dragVector.x + dragVector.y * dragVector.y);
        
        // Apply a scaling factor to convert distance to appropriate velocity
        float factor = 2.5f;
        velocity = dragVector * factor;
        return true;
    }
    return false;
}

bool Ball::handleMouseMove(const sf::Vector2f& mousePos) {
    if (isDragging) {
        currentDragPos = mousePos;
        
        // Update the line end point to show drag direction and strength
        line[1].position = mousePos;
        
        // Update arrow head
        updateArrowHead();
        return true;
    }
    return false;
}

void Ball::updateArrowHead() {
    // The direction the ball will travel is from ball to mouse, but inverted
    // We're using mouse to ball direction because that's the direction the ball will travel
    sf::Vector2f direction = line[0].position - line[1].position;
    
    // Normalize direction vector
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length < 1.0f) return; // Avoid division by zero
    
    sf::Vector2f unitDirection = direction / length;
    
    // Calculate perpendicular vector
    sf::Vector2f perpendicular(-unitDirection.y, unitDirection.x);
    
    // Size of arrow head
    float arrowSize = 15.0f;
    
    // Position of arrow tip - now at the ball's position since that's where ball will go
    sf::Vector2f arrowTip = line[0].position;
    
    // Calculate the two points at the base of the arrow head
    sf::Vector2f baseLeft = arrowTip - (unitDirection * arrowSize) + (perpendicular * arrowSize * 0.5f);
    sf::Vector2f baseRight = arrowTip - (unitDirection * arrowSize) - (perpendicular * arrowSize * 0.5f);
    
    // Set arrowhead positions
    arrowHead[0].position = arrowTip;
    arrowHead[1].position = baseLeft;
    arrowHead[2].position = baseRight;
}

sf::FloatRect Ball::getBounds() const {
    return shape.getGlobalBounds();
}

void Ball::checkCollision(const Obstacle& obstacle) {
    // Skip collision check if not moving
    if (velocity.x == 0 && velocity.y == 0) {
        return;
    }
    
    // Get obstacle bounds
    sf::FloatRect obstacleBounds = obstacle.getBounds();
    
    // Calculate the center point of the ball
    float ballRadius = shape.getRadius();
    sf::Vector2f ballCenter = position;
    
    // Find the closest point on the obstacle to the ball center
    sf::Vector2f closestPoint;
    
    // Clamp ball center to obstacle bounds for closest point
    auto obstacleWidth = obstacleBounds.size.x;
    auto obstacleLeft = obstacleBounds.position.x;
    auto obstacleHeight = obstacleBounds.size.y;
    auto obstacleTop = obstacleBounds.position.y;

    closestPoint.x = std::max(obstacleLeft, std::min(ballCenter.x, obstacleLeft + obstacleWidth));
    closestPoint.y = std::max(obstacleTop, std::min(ballCenter.y, obstacleTop + obstacleHeight));
    
    // Calculate the distance between the ball center and closest point
    sf::Vector2f ballToClosest = ballCenter - closestPoint;
    float distanceSquared = ballToClosest.x * ballToClosest.x + ballToClosest.y * ballToClosest.y;
    
    // Check if collision occurred (distance is less than ball radius)
    if (distanceSquared <= ballRadius * ballRadius) {
        // Normalize the direction vector
        float distance = std::sqrt(distanceSquared);
        sf::Vector2f direction = distance > 0 ? ballToClosest / distance : sf::Vector2f(0, -1);
        
        // Move the ball outside the obstacle along collision normal
        float overlap = ballRadius - distance;
        position += direction * overlap;
        
        // Reflect velocity based on collision normal (with some energy loss)
        float speedLoss = 0.8f; // Ball loses some energy on collision
        
        // Calculate dot product of velocity and normal
        float dotProduct = velocity.x * direction.x + velocity.y * direction.y;
        
        // Apply reflection formula: v' = v - 2(v·n)n
        velocity -= 2.0f * dotProduct * direction * speedLoss;
        
        // Update ball position
        shape.setPosition(position);
        line[0].position = position;
    }
} 