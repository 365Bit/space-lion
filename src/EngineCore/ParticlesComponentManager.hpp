#ifndef ParticlesComponentManager_hpp
#define ParticlesComponentManager_hpp

#include "BaseResourceManager.hpp"
#include "BaseMultiInstanceComponentManager.hpp"
#include "ComponentStorage.hpp"

namespace EngineCore {
    namespace Graphics {

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

        public:
            ParticlesComponentManager() = default;
            ~ParticlesComponentManager() = default;

            template<typename ParticleType>
            size_t addComponent(Entity entity, std::shared_ptr<std::vector<ParticleType>> particle_data);

            void deleteComponent(Entity entity);

            size_t getComponentCount() const;

            bool checkComponent(size_t index);

            //Data const& getComponent(size_t index) const;

            //Data& getComponent(size_t index);
        };

        template<typename ParticleType>
        inline size_t ParticlesComponentManager::addComponent(Entity entity, std::shared_ptr<std::vector<ParticleType>> particle_data)
        {
            return size_t();
        }

    }
}

#endif // !ParticlesComponentManager_hpp
