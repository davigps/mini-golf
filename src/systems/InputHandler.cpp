#include "InputHandler.hpp"
#include "../utils/Entity.hpp"

InputHandler::InputHandler(sf::RenderWindow& window) 
    : window(window) {
}

bool InputHandler::processEvents(std::vector<std::unique_ptr<Entity>>& entities) {
    while (std::optional<sf::Event> event = window.pollEvent()) {
        // Handle window close
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return false; // Signal to stop the game
        }
        
        // Handle window resize
        if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            // Window was resized
            unsigned int width = resized->size.x;
            unsigned int height = resized->size.y;
            // Handle resize if needed
        }
        
        // Handle mouse press
        if (event->is<sf::Event::MouseButtonPressed>() && 
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = mapPixelToCoords(mousePixelPos);
            
            for (auto& entity : entities) {
                if (entity->handleMousePress(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
        
        // Handle mouse release
        if (event->is<sf::Event::MouseButtonReleased>()) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = mapPixelToCoords(mousePixelPos);
            
            for (auto& entity : entities) {
                if (entity->handleMouseRelease(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
        
        // Handle mouse move
        if (event->is<sf::Event::MouseMoved>()) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = mapPixelToCoords(mousePixelPos);
            
            for (auto& entity : entities) {
                if (entity->handleMouseMove(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
    }
    
    return true;
}

sf::Vector2f InputHandler::mapPixelToCoords(const sf::Vector2i& pixelPos) const {
    // Use the current game view from the window to convert coordinates
    return window.mapPixelToCoords(pixelPos, window.getView());
} 