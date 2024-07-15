#ifndef ParticlesComponentManager_hpp
#define ParticlesComponentManager_hpp

#include "BaseResourceManager.hpp"
#include "BaseMultiInstanceComponentManager.hpp"
#include "ComponentStorage.hpp"

namespace EngineCore {
    namespace Graphics {

        namespace RenderTaskTags {
            struct Particles {};
        }

        template<typename ResourceManagerType>
        class ParticlesComponentManager : public BaseMultiInstanceComponentManager
        {
        private:
            struct Data
            {
                Entity     entity;         ///< entity that owns the component
                size_t     particle_count;
                ResourceID particle_data;
            };

            Utility::ComponentStorage<Data, 1000, 1000> data_;

            ResourceManagerType* resource_mngr_;

        public:
            ParticlesComponentManager() = default;
            ~ParticlesComponentManager() = default;

            template<typename ParticleType>
            size_t addComponent(Entity entity, std::shared_ptr<std::vector<ParticleType>> particle_data);

            void deleteComponent(Entity entity);

            size_t getComponentCount() const;

            bool checkComponent(size_t index);

            Data const& getComponent(size_t index) const;

            //Data& getComponent(size_t index);
        };

        template<typename ResourceManagerType>
        template<typename ParticleType>
        inline size_t ParticlesComponentManager<ResourceManagerType>::addComponent(Entity entity, std::shared_ptr<std::vector<ParticleType>> particle_data)
        {
            auto idx_query = getIndex(entity);

            auto rsrc_id = resource_mngr_->createStructuredBufferAsync(
                std::to_string(entity.id()) + "_particles_" + std::to_string(idx_query.size()),
                particle_data
            );

            auto index = data_.addComponent(
                {
                    entity,
                    particle_data->size(),
                    rsrc_id
                }
            );

            addIndex(entity.id(), index);

            auto [page_idx, idx_in_page] = data_.getIndices(index);

            return index;
        }

        template<typename ResourceManagerType>
        inline void ParticlesComponentManager<ResourceManagerType>::deleteComponent(Entity entity)
        {
            //TODO
        }

        template<typename ResourceManagerType>
        inline size_t ParticlesComponentManager<ResourceManagerType>::getComponentCount() const
        {
            return data_.getComponentCount();
        }

        template<typename ResourceManagerType>
        inline bool ParticlesComponentManager<ResourceManagerType>::checkComponent(size_t index)
        {
            auto indices = data_.getIndices(index);
            return data_.checkComponent(indices.first, indices.second);
        }

        template<typename ResourceManagerType>
        inline ParticlesComponentManager<ResourceManagerType>::Data const& ParticlesComponentManager<ResourceManagerType>::getComponent(size_t index) const
        {
            auto [page_idx, idx_in_page] = data_.getIndices(index);

            return data_(page_idx, idx_in_page);
        }

        //EngineCore::Graphics::ParticlesComponentManager::Data& EngineCore::Graphics::ParticlesComponentManager::getComponent(size_t index)
        //{
        //    // TODO: insert return statement here
        //    return data_.getComponentCopy(data_.getIndices(index));
        //}


    }
}

#endif // !ParticlesComponentManager_hpp
