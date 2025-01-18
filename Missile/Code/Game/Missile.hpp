#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Mesh.hpp"

#include "Game/GameCommon.hpp"

class Missile {
public:

    Missile() = default;
    Missile(const Missile& other) = default;
    Missile(Missile&& other) = default;
    Missile& operator=(const Missile& other) = default;
    Missile& operator=(Missile&& other) = default;
    ~Missile() = default;

    Missile(Vector2 startPosition, Vector2 target, Faction faction) noexcept;
    Missile(Vector2 startPosition, Vector2 target, TimeUtils::FPSeconds timeToTarget, Faction faction) noexcept;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaTime) noexcept;
    void AppendToMesh(Mesh::Builder& builder) noexcept;
    void EndFrame() noexcept;

    Vector2 GetPosition() const noexcept;

    void SetTarget(Vector2 newTarget) noexcept;
    Vector2 GetTarget() const noexcept;
    bool ReachedTarget() const noexcept;

    void SetColor(Rgba newColor) noexcept;
    void Kill() noexcept;
    bool IsDead() const noexcept;

    void SetFaction(Faction newFaction) noexcept;
    Faction GetFaction() const noexcept;

protected:
private:

    Vector2 m_position{};
    Vector2 m_target{};
    Vector2 m_startPosition{};
    Rgba m_color{Rgba::Red};
    TimeUtils::FPSeconds m_timeToTarget{ TimeUtils::FPSeconds{1.0f} };
    float m_speed{};
    int m_health{1};
    static inline int idx{0};
    Faction m_faction{Faction::None};

};
