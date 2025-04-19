#include "Obstacle.hpp"
#include <cmath>
#include <array>

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
    shadow.setRotation(shape.getRotation());  // Match the rotation of the original shape
    window.draw(shadow);
}

sf::FloatRect Obstacle::getBounds() const {
    return shape.getGlobalBounds();
}

void Obstacle::setRotation(float angle) {
    shape.setRotation(sf::degrees(angle));
}

sf::Vector2f Obstacle::getPosition() const {
    return shape.getPosition();
}

float Obstacle::getRotation() const {
    return shape.getRotation().asDegrees();
}

sf::Vector2f Obstacle::getSize() const {
    return shape.getSize();
}

std::array<sf::Vector2f, 4> Obstacle::getCorners() const {
    // Get basic information about the rectangle
    sf::Vector2f position = shape.getPosition();
    sf::Vector2f size = shape.getSize();
    float angle = shape.getRotation().asDegrees() * 3.14159f / 180.f; // Convert to radians
    
    // Calculate half-width and half-height
    float halfWidth = size.x / 2.f;
    float halfHeight = size.y / 2.f;
    
    // Calculate sine and cosine of the rotation angle
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);
    
    // Calculate the four corners relative to the center
    std::array<sf::Vector2f, 4> corners;
    
    // Top-left corner
    corners[0].x = position.x - halfWidth * cosA + halfHeight * sinA;
    corners[0].y = position.y - halfWidth * sinA - halfHeight * cosA;
    
    // Top-right corner
    corners[1].x = position.x + halfWidth * cosA + halfHeight * sinA;
    corners[1].y = position.y + halfWidth * sinA - halfHeight * cosA;
    
    // Bottom-right corner
    corners[2].x = position.x + halfWidth * cosA - halfHeight * sinA;
    corners[2].y = position.y + halfWidth * sinA + halfHeight * cosA;
    
    // Bottom-left corner
    corners[3].x = position.x - halfWidth * cosA - halfHeight * sinA;
    corners[3].y = position.y - halfWidth * sinA + halfHeight * cosA;
    
    return corners;
}

float Obstacle::distancePointLineSegment(const sf::Vector2f& point, 
                                        const sf::Vector2f& lineStart, 
                                        const sf::Vector2f& lineEnd,
                                        sf::Vector2f& closestPoint) const {
    // Calculate the line vector and length
    sf::Vector2f lineVec = lineEnd - lineStart;
    float lineLength = std::sqrt(lineVec.x * lineVec.x + lineVec.y * lineVec.y);
    
    // Normalize line vector
    if (lineLength > 0) {
        lineVec = lineVec / lineLength;
    }
    
    // Calculate vector from line start to point
    sf::Vector2f pointVec = point - lineStart;
    
    // Project pointVec onto lineVec to find the closest point
    float projection = pointVec.x * lineVec.x + pointVec.y * lineVec.y;
    
    // Clamp projection to the line segment
    projection = std::max(0.f, std::min(projection, lineLength));
    
    // Calculate the closest point on the line segment
    closestPoint = lineStart + lineVec * projection;
    
    // Calculate distance from point to closest point
    float dx = point.x - closestPoint.x;
    float dy = point.y - closestPoint.y;
    
    return std::sqrt(dx * dx + dy * dy);
}

bool Obstacle::checkCircleCollision(const sf::Vector2f& circleCenter, float radius, 
                                   sf::Vector2f& collisionPoint, sf::Vector2f& collisionNormal) const {
    // Get the corners of the obstacle
    auto corners = getCorners();
    
    // Find the minimum distance from the circle center to any edge of the rectangle
    float minDistance = std::numeric_limits<float>::max();
    sf::Vector2f closestPoint;
    
    for (int i = 0; i < 4; i++) {
        // Get current edge (from this corner to the next)
        sf::Vector2f currentCorner = corners[i];
        sf::Vector2f nextCorner = corners[(i + 1) % 4];
        
        // Find the closest point on this edge to the circle center
        sf::Vector2f tempClosestPoint;
        float distance = distancePointLineSegment(circleCenter, currentCorner, nextCorner, tempClosestPoint);
        
        // Update minimum distance if this edge is closer
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = tempClosestPoint;
        }
    }
    
    // Check if the circle is colliding with the rectangle
    if (minDistance <= radius) {
        // Calculate collision point (on the edge of the obstacle)
        collisionPoint = closestPoint;
        
        // Calculate collision normal (from closest point to circle center, normalized)
        sf::Vector2f normalVec = circleCenter - closestPoint;
        float normalLength = std::sqrt(normalVec.x * normalVec.x + normalVec.y * normalVec.y);
        
        if (normalLength > 0) {
            collisionNormal = normalVec / normalLength;
        } else {
            collisionNormal = sf::Vector2f(0, -1); // Default normal if circle center is exactly on the edge
        }
        
        return true;
    }
    
    return false;
} 