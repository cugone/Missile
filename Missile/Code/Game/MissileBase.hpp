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

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    void Fire(MissileManager::Target target) noexcept;
    void RemoveAllMissiles() noexcept;
    bool HasMissilesRemaining() const noexcept;
    void DecrementMissiles() noexcept;
    void IncrementMissiles() noexcept;

    const MissileManager& GetMissileManager() const noexcept;
    MissileManager& GetMissileManager() noexcept;

protected:
private:

    void RenderRemainingMissiles() const noexcept;

    Vector2 GetMissileLauncherPosition() noexcept;

    Vector2 m_position{};
    MissileManager m_missileManager{};
    int m_maxMissiles{10};
    int m_missilesRemaining{};
};
