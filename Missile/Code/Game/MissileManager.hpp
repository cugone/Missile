#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Mesh.hpp"

#include "Game/GameCommon.hpp"
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
    void DebugRender() const noexcept;
    void EndFrame() noexcept;

    bool LaunchMissile(Vector2 position, Direction direction, TimeUtils::FPSeconds timeToTarget, Faction faction, Rgba color) noexcept;
    bool LaunchMissile(Vector2 position, Target target, TimeUtils::FPSeconds timeToTarget, Faction faction, Rgba color) noexcept;

    std::size_t ActiveMissileCount() const noexcept;
    std::vector<Vector2> GetMissilePositions() const noexcept;
    void KillMissile(std::size_t idx) noexcept;

protected:
private:
    Vector2 m_position{};
    std::vector<Missile> m_missiles{};
    std::vector<std::size_t> m_deadMissiles{};
    mutable Mesh::Builder m_builder{};
};
