#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

template <typename Resource>
class ResourceManager 
{
public:
    void load(std::string const& t_id, std::string const& t_filename)
    {
        auto resource = std::make_unique<Resource>();
        bool success = false;

        // Compile-time check: Is this a Font?
        if constexpr (std::is_same_v<Resource, sf::Font>) 
        {
            // Font uses openFromFile
            success = resource->openFromFile(t_filename);
        }
        // Otherwise, assume it's a Texture/SoundBuffer
        else 
        {
            // Other resources use loadFromFile
            success = resource->loadFromFile(t_filename);
        }

        if (!success) 
        {
            throw std::runtime_error("Failed to load resource: " + t_filename);
        }

        m_resources[t_id] = std::move(resource);
    }

    Resource const& get(std::string const& t_id) const
    {
        // Look up the resource by id, if it exists, the map iterator will point to it, 
        // otherwise the map iterator will be equal to the map end iterator.
        std::map<std::string, std::unique_ptr<Resource>>::const_iterator found = m_resources.find(t_id);
        if (found == m_resources.end())
        {
            throw std::runtime_error("Resource ID not found: " + t_id);
        }
        // return the resource (first is the name, second is the resource itself)
        return *found->second;
    }

    void remove(std::string const& t_id) 
    {
        int removed = m_resources.erase(t_id);
        if (removed == 0) 
        {
            throw std::runtime_error("Could not delete Resource ID : " + t_id);
        }
    }


private:
    std::map<std::string, std::unique_ptr<Resource>> m_resources;
};