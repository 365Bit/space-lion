#ifndef CooldownTriggerComponentManager_hpp
#define CooldownTriggerComponentManager_hpp

#include "functional"

#include "BaseMultiInstanceComponentManager.hpp"
#include "ComponentStorage.hpp"

namespace EngineCore {
    namespace Common {

        class CooldownTriggerComponentManager : public BaseMultiInstanceComponentManager
        {
        private:
            struct Data
            {
                Entity                entity;            ///< entity that owns the component
                bool                  is_active;         ///< flag denoting if cooldown actively counts down
                float                 remaining_time;    ///< remaining cooldown time
                float                 reset_time;        ///< value that the cooldown is reset to
                std::function<void()> cooldown_callback; ///< function that is called when the cooldown time has passed
            };

            Utility::ComponentStorage<Data, 1000, 1000> data_;

        public:
            CooldownTriggerComponentManager() = default;
            ~CooldownTriggerComponentManager() = default;

            size_t addComponent(Entity entity, float reset_time, std::function<void()> cooldown_callback, bool start_on_creation = true);

            void deleteComponent(Entity entity);

            void resetCooldown(Entity entity);

            void resetCooldown(size_t index);

            size_t getComponentCount() const;

            bool checkComponent(size_t index);

            Data const& getComponent(size_t index) const;

            Data& getComponent(size_t index);
        };

    }
}

#endif // !CooldownTriggerComponentManager_hpp
