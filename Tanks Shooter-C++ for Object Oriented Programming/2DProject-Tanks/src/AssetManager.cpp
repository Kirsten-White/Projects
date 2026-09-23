#include "AssetManager.h"

void AssetManager::loadTexture(std::string const& t_id, std::string const& t_path)
{
    m_textures.load(t_id, t_path);
}
void AssetManager::loadFont(std::string const& t_id, std::string const& t_path)
{
    m_fonts.load(t_id, t_path);
}

const sf::Texture& AssetManager::getTexture(std::string const& t_id) const
{
    return m_textures.get(t_id);
}
const sf::Font& AssetManager::getFont(std::string const& t_id) const
{
    return m_fonts.get(t_id);
}

void AssetManager::releaseTexture(std::string const& t_id) 
{
    m_textures.remove(t_id);
}

void AssetManager::releaseFont(std::string const& t_id) 
{
    m_fonts.remove(t_id);
}