#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

// Singleton ResourceManager for efficiently managing textures, sounds, fonts, etc.
class ResourceManager {
public:
    // Get singleton instance
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    // Load a texture from file (or get it from cache if already loaded)
    sf::Texture& getTexture(const std::string& filename) {
        auto it = textures.find(filename);
        if (it != textures.end()) {
            return *it->second;
        }
        
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load texture: " + filename);
        }
        
        sf::Texture& result = *texture;
        textures[filename] = std::move(texture);
        return result;
    }
    
    // Load a font from file (or get it from cache if already loaded)
    sf::Font& getFont(const std::string& filename) {
        auto it = fonts.find(filename);
        if (it != fonts.end()) {
            return *it->second;
        }
        
        auto font = std::make_unique<sf::Font>();
        if (!font->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load font: " + filename);
        }
        
        sf::Font& result = *font;
        fonts[filename] = std::move(font);
        return result;
    }
    
    // Load a sound buffer from file (or get it from cache if already loaded)
    sf::SoundBuffer& getSoundBuffer(const std::string& filename) {
        auto it = soundBuffers.find(filename);
        if (it != soundBuffers.end()) {
            return *it->second;
        }
        
        auto soundBuffer = std::make_unique<sf::SoundBuffer>();
        if (!soundBuffer->loadFromFile(filename)) {
            throw std::runtime_error("Failed to load sound buffer: " + filename);
        }
        
        sf::SoundBuffer& result = *soundBuffer;
        soundBuffers[filename] = std::move(soundBuffer);
        return result;
    }
    
    // Clear all cached resources
    void clear() {
        textures.clear();
        fonts.clear();
        soundBuffers.clear();
    }
    
private:
    // Private constructor for singleton
    ResourceManager() = default;
    
    // Delete copy and move constructors and assignment operators
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    
    // Resource caches
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
}; 