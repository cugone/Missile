#include "Game/Missile.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"

#include "Engine/Services/ServiceLocator.hpp"

Missile::Missile(Vector2 startPosition, Vector2 target) noexcept
    : Missile{ startPosition, target, TimeUtils::Frames{1} }
{
    /* DO NOTHING */
}

Missile::Missile(Vector2 startPosition, Vector2 target, TimeUtils::FPSeconds timeToTarget) noexcept
    : m_position{ startPosition }
    , m_target{ target }
    , m_startPosition{ startPosition }
    , m_timeToTarget{timeToTarget}
{
    /* DO NOTHING */
}

void Missile::BeginFrame() noexcept {
    /* DO NOTHING */
}

void Missile::Update([[maybe_unused]] TimeUtils::FPSeconds deltaTime) noexcept {
    if(TimeUtils::FPSeconds::zero() < m_timeToTarget) {
        auto direction = (m_target - m_position);
        const auto distanceInPixels = direction.Normalize();
        const auto pixelsPerSecond = distanceInPixels / m_timeToTarget.count();
        m_position += direction * pixelsPerSecond * deltaTime.count();
        m_timeToTarget -= deltaTime;
        if(m_timeToTarget <= TimeUtils::FPSeconds::zero()) {
            m_timeToTarget = TimeUtils::FPSeconds::zero();
            m_position = m_target;
        }
    }
}

void Missile::Render() const {
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    g_theRenderer->SetModelMatrix();
    g_theRenderer->DrawLine2D(m_startPosition, m_position, Rgba::Red);
}

void Missile::EndFrame() noexcept {
    /* DO NOTHING */
}

void Missile::SetTarget(Vector2 newTarget) noexcept {
    m_target = newTarget;
}

Vector2 Missile::GetTarget() const noexcept {
    return m_target;
}

bool Missile::ReachedTarget() const noexcept {
    return MathUtils::IsEquivalentToZero((m_target - m_position).CalcLengthSquared());
}

