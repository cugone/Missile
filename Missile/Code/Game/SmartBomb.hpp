#pragma once

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/TimeUtils.hpp"

class EnemyWave;
class ExplosionManager;

class SmartBomb {
public:
    SmartBomb() = default;
    SmartBomb(const SmartBomb& other) = default;
    SmartBomb(SmartBomb&& other) = default;
    SmartBomb& operator=(const SmartBomb& other) = default;
    SmartBomb& operator=(SmartBomb&& other) = default;
    ~SmartBomb() = default;

    explicit SmartBomb(ExplosionManager* explosionManager, Vector2 startPosition, Vector2 target) noexcept;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void DebugRender() const noexcept;
    void EndFrame() noexcept;

    void Kill() noexcept;
    bool IsDead() const noexcept;

    Disc2 GetCollisionMesh() const noexcept;

    Vector2 GetPosition() const noexcept;

protected:
private:
    Vector2 AvoidExplosions() noexcept;

    Vector2 m_startPosition{};
    Vector2 m_position{};
    Vector2 m_velocity{};
    Vector2 m_target{};
    Rgba m_color{};
    int m_health{ 1 };
    float m_radius{ 16.0f };
    float m_sensorRadius{ 32.0f };
    float m_speed{40.0f};
    ExplosionManager* m_explosionManager{};
};
