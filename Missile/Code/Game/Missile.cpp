#include "Game/Missile.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Audio/AudioSystem.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

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
    g_theAudioSystem->Play(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{ "Audio" } / std::filesystem::path{ std::format("LaunchMissile{}.wav", idx) }, AudioSystem::SoundDesc{});
    idx = (idx + 1) % max_launch_sounds;
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
    if(ReachedTarget()) {
        Kill();
    }
}

void Missile::AppendToMesh(Mesh::Builder& builder) noexcept {
    builder.Begin(PrimitiveType::Lines);
    builder.SetColor(m_color);

    builder.AddVertex(m_startPosition);
    builder.AddVertex(m_position);
    builder.AddIndicies(Mesh::Builder::Primitive::Line);

    builder.End(g_theRenderer->GetMaterial("__2D"));
}

void Missile::EndFrame() noexcept {
    if(IsDead()) {
        GetGameAs<Game>()->CreateExplosionAt(m_position);
    }
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

void Missile::SetColor(Rgba newColor) noexcept {
    m_color = newColor;
}

bool Missile::IsDead() const noexcept {
    return m_health <= 0;
}

void Missile::Kill() noexcept {
    m_health = 0;
}
