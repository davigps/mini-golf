#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <optional>

class Entity;

// Input handler responsible for processing all user input
class InputHandler {
public:
    InputHandler(sf::RenderWindow& window);
    ~InputHandler() = default;
    
    // Process all input events
    bool processEvents(std::vector<std::unique_ptr<Entity>>& entities);
    
    // Map pixel coords to world coordinates
    sf::Vector2f mapPixelToCoords(const sf::Vector2i& pixelPos) const;
    
private:
    sf::RenderWindow& window;
}; 