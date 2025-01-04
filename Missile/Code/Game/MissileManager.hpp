#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Mesh.hpp"

#include "Game/Missile.hpp"

#include <vector>

class MissileManager {
public:

    struct Direction {
        Vector2 value;
    };
    struct Target {
        Vector2 value;
    };

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    void LaunchMissile(Vector2 position, Direction direction, TimeUtils::FPSeconds timeToTarget) noexcept;
    void LaunchMissile(Vector2 position, Target target, TimeUtils::FPSeconds timeToTarget) noexcept;
    void FireMissile() noexcept;
protected:
private:
    Vector2 m_position{};
    std::vector<Missile> m_missiles{};
    mutable Mesh::Builder m_builder{};
    bool m_missileFired{false};
};
