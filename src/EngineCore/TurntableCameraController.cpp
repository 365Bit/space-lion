#include "TurntableCameraController.hpp"

#include "../EngineCore/types.hpp"
#include "../EngineCore/CameraComponent.hpp"
#include "../EngineCore/TransformComponentManager.hpp"

Editor::Controls::TurntableCameraController::TurntableCameraController(EngineCore::WorldState& world_state)
    : world_state_(world_state),
    yaw_(0.0f),
    pitch_(0.0f),
    pitch_limit_(glm::half_pi<float>() - 0.01f)
{
    EngineCore::Common::Input::StateDrivenAction gamepad_state_action = {
        {
            {EngineCore::Common::Input::Device::GAMEPAD_AXES,EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_LEFT_X},
            {EngineCore::Common::Input::Device::GAMEPAD_AXES,EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_LEFT_Y},
            {EngineCore::Common::Input::Device::GAMEPAD_AXES,EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_RIGHT_X},
            {EngineCore::Common::Input::Device::GAMEPAD_AXES,EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_RIGHT_Y},
            {EngineCore::Common::Input::Device::GAMEPAD_AXES,EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_LEFT_TRIGGER},
            {EngineCore::Common::Input::Device::GAMEPAD_AXES,EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_RIGHT_TRIGGER}
        },
        std::bind(&TurntableCameraController::controlCameraGamepadAction, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    };

    gamepad_input_action_context_ = { "editor_gamepad_cam_controls", true, {}, {gamepad_state_action} };
}

EngineCore::Common::Input::InputActionContext const& Editor::Controls::TurntableCameraController::getGamepadInputActionContext()
{
    return gamepad_input_action_context_;
}

void Editor::Controls::TurntableCameraController::resetValues(Vec3 rot_center, float radius)
{
    yaw_ = 0.0f;
    pitch_ = 0.0f;

    radius_ = radius;
    rot_center_ = rot_center;
}

void Editor::Controls::TurntableCameraController::setRadius(float radius)
{
    radius_ = radius;
}

void Editor::Controls::TurntableCameraController::setRotCenter(Vec3 rot_center)
{
    rot_center_ = rot_center;
}

void Editor::Controls::TurntableCameraController::controlCameraGamepadAction(
    EngineCore::Common::Input::HardwareStateQuery const& input_hardware,
    std::vector<EngineCore::Common::Input::HardwareState> states,
    double dt)
{
    auto& camera_mngr = world_state_.get<EngineCore::Graphics::CameraComponentManager>();
    auto& transform_mngr = world_state_.get<EngineCore::Common::TransformComponentManager>();

    Entity camera_entity = camera_mngr.getActiveCamera();

    if (camera_entity == EntityManager::invalidEntity())
    {
        return;
    }

    size_t camera_transform_idx = transform_mngr.getIndex(camera_entity);

    Vec3 cam_position = transform_mngr.getPosition(camera_transform_idx);
    //auto view_to_world = transform_mngr.getWorldTransformation(camera_transform_idx);
    //auto view_to_world_vec = glm::mat3x3(view_to_world);
    //auto world_to_view_vec = glm::inverse(view_to_world_vec);
    //view_to_world_vec = glm::inverse(view_to_world_vec);
    //view_to_world_vec = glm::transpose(view_to_world_vec);
    //Vec3 cam_forward = view_to_world_vec * Vec3(0.0f, 0.0f, -1.0f);
    //Vec3 cam_right = view_to_world_vec * Vec3(1.0f, 0.0f, 0.0f);
    //Vec3 cam_up = view_to_world_vec * Vec3(0.0f, 1.0f, 0.0f);

    //Vec3 world_up_vs = world_to_view_vec * Vec3(0.0f, 1.0f, 0.0f);

    //Vec3 movement = Vec3(0.0f, 0.0f, 0.0f);

    float dead_zone = 0.05f;
    float thumbstick_dead_zone = 0.15f;

    float dt_f = static_cast<float>(dt);

    // first hardware part is left x axis
    if (std::sqrt(std::pow(std::abs(states[0]), 2.0f) + std::pow(std::abs(states[1]), 2.0f)) > thumbstick_dead_zone)
    {
        // Move rotation center
        float sign_flip_x = std::signbit(states[0]) ? -1.0f : 1.0f;
        float sign_flip_y = std::signbit(states[1]) ? -1.0f : 1.0f;

        //float remapped_state_x = (states[0] - dead_zone) / (1.0f - dead_zone);
        //float remapped_state_y = (states[1] - dead_zone) / (1.0f - dead_zone);

        //float factor = 100.0f;

        //cam_position += cam_right * factor * remapped_state_x * static_cast<float>(dt);
        //cam_position += cam_up * factor * remapped_state_y * static_cast<float>(dt);
        //transform_mngr.setPosition(camera_transform_idx, cam_position);

        float remapped_state_x = (states[0] - sign_flip_x * dead_zone) / (1.0f - dead_zone);
        float remapped_state_y = (states[1] - sign_flip_y * dead_zone) / (1.0f - dead_zone);

        float factor = 0.5f;

        glm::vec3 forward = glm::normalize(rot_center_ - cam_position);
        glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
        glm::vec3 up = glm::cross(forward, right);

        rot_center_ -= right * remapped_state_x * factor * radius_ * dt_f;
        rot_center_ += up * remapped_state_y * factor * radius_ * dt_f;
    }

    if (std::sqrt(std::pow(std::abs(states[2]), 2.0f) + std::pow(std::abs(states[3]), 2.0f)) > thumbstick_dead_zone)
    {
        // Rotate camera around rotation center
        float sign_flip_x = std::signbit(states[2]) ? -1.0f : 1.0f;
        float sign_flip_y = std::signbit(states[3]) ? -1.0f : 1.0f;

        float remapped_state_x = (states[2] - sign_flip_x * dead_zone) / (1.0f - dead_zone);
        float remapped_state_y = (states[3] - sign_flip_y * dead_zone) / (1.0f - dead_zone);

        //float factor_x = 90.0f;
        //float factor_y = 90.0f;

        //if (dot(cam_forward, Vec3(0.0, 1.0, 0.0)) < -0.99f) {
        //    remapped_state_y = std::min(remapped_state_y, 0.0f);
        //}
        //else if (dot(cam_forward, Vec3(0.0, 1.0, 0.0)) > 0.99f) {
        //    remapped_state_y = std::max(remapped_state_y, 0.0f);
        //}

        //// rotate horizontally
        //auto rot_lon = glm::angleAxis(factor_x * remapped_state_x * static_cast<float>(dt) * (3.14159265f / 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //cam_right = glm::rotate(rot_lon, cam_right);
        //cam_forward = glm::rotate(rot_lon, cam_forward);
        //cam_up = glm::rotate(rot_lon, cam_up);

        //// rotate vertically
        //auto rot_lat = glm::angleAxis(factor_y * remapped_state_y * static_cast<float>(dt) * (3.14159265f / 180.0f), -cam_right);
        //cam_forward = glm::rotate(rot_lat, cam_forward);
        //cam_up = glm::rotate(rot_lat, cam_up);

        //// transform s.t. rotation center is origin
        //auto shifted_pos = cam_position - rot_center_;
        //shifted_pos = glm::rotate(rot_lon, shifted_pos);
        //shifted_pos = glm::rotate(rot_lat, shifted_pos);

        //// transform back
        //cam_position = shifted_pos + glm::vec3(rot_center_);

        //transform_mngr.setPosition(camera_transform_idx, cam_position);
        //transform_mngr.rotate(camera_transform_idx, rot_lon * rot_lat);

        constexpr float factor_x = glm::radians(90.0f);
        constexpr float factor_y = glm::radians(90.0f);

        yaw_ += remapped_state_x * factor_x * dt_f;
        pitch_ += remapped_state_y * factor_y * dt_f;
        pitch_ = std::clamp(pitch_, -pitch_limit_, pitch_limit_);
    }

    if (std::sqrt(std::pow(std::abs(states[4]), 2.0f) + std::pow(std::abs(states[5]), 2.0f)) > dead_zone)
    {
        // Move camera closer or further away from rotation center
        float dz = states[4] - states[5];

        //auto v = glm::normalize(rot_center_ - cam_position);

        auto altitude = glm::length(rot_center_ - cam_position);

        //if (altitude > 4000.0)
        //{
        //    dz = std::min(dz, 0.0f);
        //}

        //cam_position = cam_position - (v * dz * (altitude / 50.0f));
        //transform_mngr.setPosition(camera_transform_idx, cam_position);

        radius_ += dz * (altitude / 50.0f);
        radius_ = glm::clamp(radius_, 0.0f, 4000.0f);
    }

    // Recompute camera position and orientation
    cam_position.x = rot_center_.x + radius_ * std::cos(pitch_) * std::sin(yaw_);
    cam_position.y = rot_center_.y + radius_ * std::sin(pitch_);
    cam_position.z = rot_center_.z + radius_ * std::cos(pitch_) * std::cos(yaw_);

    Vec3 cam_forward = glm::normalize(cam_position - rot_center_);
    Vec3 cam_right = glm::normalize(glm::cross(Vec3(0.0f, 1.0f, 0.0f), cam_forward));
    Vec3 cam_up = glm::cross(cam_forward, cam_right);

    glm::mat3 rot_matrix;
    rot_matrix[0] = cam_right;
    rot_matrix[1] = cam_up;
    rot_matrix[2] = cam_forward;

    transform_mngr.setPositionOrientation(camera_transform_idx, cam_position, glm::quat_cast(rot_matrix));
}
