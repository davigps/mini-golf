#pragma once

#include <SFML/Graphics.hpp>

namespace Colors {
    // Course colors
    const sf::Color LightGreen = sf::Color(40, 130, 40);    // Green grass/walls
    const sf::Color DarkGreen = sf::Color(30, 100, 30);     // Dark green for checkerboard pattern
    
    // Obstacle colors
    const sf::Color LightBrown = sf::Color(139, 69, 19);    // Light brown obstacles
    const sf::Color DarkBrown = sf::Color(120, 60, 12);     // Dark brown obstacles
    const sf::Color Gray = sf::Color(100, 100, 100);        // Default obstacle color
    
    // Ball colors
    const sf::Color BallColor = sf::Color::White;           // Ball color
    const sf::Color DragLineColor = sf::Color::Red;         // Color for the drag line
} 