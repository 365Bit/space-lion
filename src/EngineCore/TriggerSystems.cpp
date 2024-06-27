#include "TriggerSystems.hpp"

void EngineCore::Common::checkProximityTriggers(
    EngineCore::Common::TransformComponentManager& transform_mngr,
    EngineCore::Common::ProximityTriggerComponentManager& proximity_trigger_mngr,
    double dt,
    Utility::TaskScheduler& task_scheduler)
{
    size_t component_cnt = proximity_trigger_mngr.getComponentCount();

    std::vector<std::pair<size_t, size_t>> from_to_pairs;
    size_t bucket_cnt = 6;
    for (size_t i = 0; i < bucket_cnt; ++i) {
        from_to_pairs.push_back({ component_cnt * (float(i) / float(bucket_cnt)), component_cnt * (float(i + 1) / float(bucket_cnt)) });
    }

    for (auto from_to : from_to_pairs) {
        task_scheduler.submitTask(
            [&transform_mngr, &proximity_trigger_mngr, from_to, dt]() {
                for (size_t i = from_to.first; i < from_to.second; ++i)
                {
                    if (proximity_trigger_mngr.checkComponent(i)) {
                        auto& cmp = proximity_trigger_mngr.getComponent(i);

                        auto entity_transform_idx = transform_mngr.getIndex(cmp.entity);
                        auto target_transform_idx = transform_mngr.getIndex(cmp.target);

                        float distance = glm::length(transform_mngr.getWorldPosition(entity_transform_idx) - transform_mngr.getWorldPosition(target_transform_idx));

                        if (distance < cmp.trigger_distance && !cmp.in_proximity) {
                            cmp.enter_callback();
                            cmp.in_proximity = true;
                        }
                        else if (distance > cmp.trigger_distance && cmp.in_proximity) {
                            cmp.leave_callback();
                            cmp.in_proximity = false;
                        }
                    }
                }
            }
        );
    }

    task_scheduler.waitWhileBusy();
}
