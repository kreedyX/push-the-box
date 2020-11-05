#include "AssetsManager.h"
#include "Logger.h"

std::unordered_map<std::string, sf::Texture*> AssetsManager::m_Textures;

bool AssetsManager::load_texture(const std::string& file_name, const std::string& asset_id, bool smooth)
{
    sf::Texture* texture = new sf::Texture();
    if (texture->loadFromFile("assets/" + file_name)) {
        if (smooth)
            texture->setSmooth(true);
        m_Textures[asset_id] = texture;
        LOG_INFO("AssetsManager: loaded texture '", '\b' + asset_id, "\b'");
        return true;
    }
    else return false;
}

sf::Texture* AssetsManager::get_texture(const std::string& asset_id)
{
    return m_Textures.at(asset_id);
}

void AssetsManager::free_memory()
{
    for (auto& t : m_Textures)
        delete t.second;
    m_Textures.clear();
}