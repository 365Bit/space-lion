#ifndef TriggerSystems_hpp
#define TriggerSystems_hpp

#include "CooldownTriggerComponentManager.hpp"
#include "ProximityTriggerComponentManager.hpp"
#include "TaskScheduler.hpp"
#include "TransformComponentManager.hpp"

namespace EngineCore {
namespace Common {
    void checkProximityTriggers(
        EngineCore::Common::TransformComponentManager& transform_mngr,
        EngineCore::Common::ProximityTriggerComponentManager& proximity_trigger_mngr,
        double dt,
        Utility::TaskScheduler& task_scheduler);

    void updateCooldownTriggers(
        EngineCore::Common::CooldownTriggerComponentManager& cooldown_trigger_mngr,
        double dt,
        Utility::TaskScheduler& task_scheduler);
}
}

#endif // !TriggerSystems_hpp
