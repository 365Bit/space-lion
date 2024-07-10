#include "ParticlesComponentManager.hpp"

void EngineCore::Graphics::ParticlesComponentManager::deleteComponent(Entity entity)
{
}

size_t EngineCore::Graphics::ParticlesComponentManager::getComponentCount() const
{
    return size_t();
}

bool EngineCore::Graphics::ParticlesComponentManager::checkComponent(size_t index)
{
    return false;
}

//EngineCore::Graphics::ParticlesComponentManager::Data const& EngineCore::Graphics::ParticlesComponentManager::getComponent(size_t index) const
//{
//    // TODO: insert return statement here
//}

//EngineCore::Graphics::ParticlesComponentManager::Data& EngineCore::Graphics::ParticlesComponentManager::getComponent(size_t index)
//{
//    // TODO: insert return statement here
//    return data_.getComponentCopy(data_.getIndices(index));
//}
