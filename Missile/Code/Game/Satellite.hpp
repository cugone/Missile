#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Mesh.hpp"

class EnemyWave;

class Satellite {
public:
    Satellite() = default;
    Satellite(const Satellite& other) = default;
    Satellite(Satellite&& other) = default;
    Satellite& operator=(const Satellite& other) = default;
    Satellite& operator=(Satellite&& other) = default;
    ~Satellite() = default;

    explicit Satellite(EnemyWave* parent, Vector2 position) noexcept;

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

    float GetFireRate() const noexcept;

    Mesh::Builder m_builder{};
    Vector2 m_position{};
    float m_speed{ 30.0f };
    float m_radius{ 25.0f };
    Stopwatch m_timeToFire{};
    EnemyWave* m_parentWave{};
    int m_health{ 1 };
};
