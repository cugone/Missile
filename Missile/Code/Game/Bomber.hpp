#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Stopwatch.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"

class EnemyWave;

class Bomber {
public:
    Bomber() = default;
    Bomber(const Bomber& other) = default;
    Bomber(Bomber&& other) = default;
    Bomber& operator=(const Bomber& other) = default;
    Bomber& operator=(Bomber&& other) = default;
    ~Bomber() = default;

    explicit Bomber(EnemyWave* parent, Vector2 position) noexcept;

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
    Vector2 m_position{};
    Stopwatch m_timeToFire{};
    float m_speed{100.0f};
    int m_health{1};
    EnemyWave* m_parentWave{};
};
