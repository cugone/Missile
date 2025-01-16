#include "Game/Missile.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Audio/AudioSystem.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

Missile::Missile(Vector2 startPosition, Vector2 target, Faction faction) noexcept
    : Missile{ startPosition, target, TimeUtils::FPSeconds{1.0f}, faction }
{
    /* DO NOTHING */
}

Missile::Missile(Vector2 startPosition, Vector2 target, TimeUtils::FPSeconds timeToTarget, Faction faction) noexcept
    : m_position{ startPosition }
    , m_target{ target }
    , m_startPosition{ startPosition }
    , m_timeToTarget{timeToTarget}
    , m_speed{(target - startPosition).CalcLength() / timeToTarget.count()}
    , m_faction{ faction }
{
    g_theAudioSystem->Play(GameConstants::game_audio_folder / std::filesystem::path{ std::format("LaunchMissile{}.wav", idx) }, AudioSystem::SoundDesc{});
    idx = (idx + 1) % GameConstants::max_launch_sounds;
}

void Missile::BeginFrame() noexcept {
    /* DO NOTHING */
}

void Missile::Update([[maybe_unused]] TimeUtils::FPSeconds deltaTime) noexcept {
    if(IsDead()) {
        return;
    }
    if(TimeUtils::FPSeconds::zero() < m_timeToTarget) {
        auto direction = (m_target - m_position).GetNormalize();
        m_position += direction * m_speed * deltaTime.count();
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

    if (m_faction == Faction::Player) {
        constexpr const float target_x_scale{ 5.0f };
        builder.Begin(PrimitiveType::Lines);
        builder.SetColor(Rgba::Random());
        builder.AddVertex(m_target - Vector2::One * target_x_scale);
        builder.AddVertex(m_target + Vector2::One * target_x_scale);
        builder.AddIndicies(Mesh::Builder::Primitive::Line);

        builder.AddVertex(m_target + Vector2{-1.0f, 1.0f} * target_x_scale);
        builder.AddVertex(m_target + Vector2{1.0f, -1.0f} * target_x_scale);
        builder.AddIndicies(Mesh::Builder::Primitive::Line);
        builder.End(g_theRenderer->GetMaterial("__2D"));
    }

    builder.Begin(PrimitiveType::Lines);
    builder.SetColor(m_color);
    builder.AddVertex(m_startPosition);
    builder.AddVertex(m_position);
    builder.AddIndicies(Mesh::Builder::Primitive::Line);
    builder.End(g_theRenderer->GetMaterial("__2D"));

    builder.Begin(PrimitiveType::Points);
    builder.SetColor(Rgba::White);
    builder.AddVertex(m_position);
    builder.AddIndicies(Mesh::Builder::Primitive::Point);
    builder.End(g_theRenderer->GetMaterial("__2D"));


}

void Missile::EndFrame() noexcept {
    if(IsDead()) {
        auto* g = GetGameAs<Game>();
        g->CreateExplosionAt(m_position, m_faction);
    }
}

Vector2 Missile::GetPosition() const noexcept {
    return m_position;
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

void Missile::SetFaction(Faction newFaction) noexcept {
    m_faction = newFaction;
}

Faction Missile::GetFaction() const noexcept {
    return m_faction;
}

int Missile::GetScore() noexcept {
    return GameConstants::enemy_missile_value;
}

void Missile::Kill() noexcept {
    m_health = 0;
}
