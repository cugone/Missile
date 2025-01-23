#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Game/MissileManager.hpp"

class MissileBase {
public:

    MissileBase() = default;
    MissileBase(const MissileBase& other) = default;
    MissileBase(MissileBase&& other) = default;
    MissileBase& operator=(const MissileBase& other) = default;
    MissileBase& operator=(MissileBase&& other) = default;
    ~MissileBase() = default;

    explicit MissileBase(Vector2 position) noexcept;

    void SetPosition(Vector2 position) noexcept;
    void SetTimeToTarget(TimeUtils::FPSeconds newTimeToTarget) noexcept;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    void Fire(MissileManager::Target target) noexcept;
    void RemoveAllMissiles() noexcept;
    bool HasMissilesRemaining() const noexcept;
    bool OutOfMissiles() const noexcept;
    void DecrementMissiles() noexcept;
    void IncrementMissiles() noexcept;

    const MissileManager& GetMissileManager() const noexcept;
    MissileManager& GetMissileManager() noexcept;

    Vector2 GetMissileLauncherPosition() const noexcept;

    AABB2 GetCollisionMesh() const noexcept;
protected:
private:

    void RenderRemainingMissiles() const noexcept;
    Rgba GetMissileColor() const noexcept;

    Vector2 m_position{};
    MissileManager m_missileManager{};
    TimeUtils::FPSeconds m_timeToTarget{ 1.0f };
    int m_maxMissiles{10};
    int m_missilesRemaining{m_maxMissiles};
};
