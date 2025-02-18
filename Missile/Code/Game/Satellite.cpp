#include "Game/Satellite.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Services/IAudioService.hpp"
#include "Engine/Services/ServiceLocator.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include "Game/EnemyWave.hpp"

Satellite::Satellite(EnemyWave* parent, Vector2 position) noexcept
    : m_position{position}
    , m_parentWave{parent}
{
    g_theAudioSystem->Play(GameConstants::game_audio_satellite_path, AudioSystem::SoundDesc{});
    m_timeToFire.SetSeconds(TimeUtils::FPFrames{GetFireRate()});
}

void Satellite::BeginFrame() noexcept {
    m_builder.Clear();
    if (IsDead()) {
        return;
    }
}

void Satellite::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if (IsDead()) {
        return;
    }
    m_position += -Vector2::X_Axis * m_speed * deltaSeconds.count();
    if(m_timeToFire.CheckAndReset()) {
        if(m_parentWave->CanSpawnMissile()) {
            m_parentWave->LaunchMissileFrom(m_position);
        }
    }

    m_builder.Begin(PrimitiveType::Lines);

    m_builder.SetColor(m_parentWave->GetObjectColor());

    m_builder.AddVertex(m_position + Vector2{ -1.5f, -1.5f } * m_radius);
    m_builder.AddVertex(m_position + Vector2{ +1.5f, +1.5f } * m_radius);
    m_builder.AddIndicies(Mesh::Builder::Primitive::Line);

    m_builder.AddVertex(m_position + Vector2{ +1.5f, -1.5f } * m_radius);
    m_builder.AddVertex(m_position + Vector2{ -1.5f, +1.5f } * m_radius);
    m_builder.AddIndicies(Mesh::Builder::Primitive::Line);
    m_builder.End(g_theRenderer->GetMaterial("__2D"));

    m_builder.Begin(PrimitiveType::Points);
    
    m_builder.SetColor(Rgba::Random());
    
    m_builder.AddVertex(m_position + Vector2{ -1.5f, -1.5f } * m_radius);
    m_builder.AddIndicies(Mesh::Builder::Primitive::Point);
    
    m_builder.AddVertex(m_position + Vector2{ +1.5f, -1.5f } * m_radius);
    m_builder.AddIndicies(Mesh::Builder::Primitive::Point);
    
    m_builder.AddVertex(m_position + Vector2{ -1.5f, +1.5f } * m_radius);
    m_builder.AddIndicies(Mesh::Builder::Primitive::Point);
    
    m_builder.AddVertex(m_position + Vector2{ +1.5f, +1.5f } * m_radius);
    m_builder.AddIndicies(Mesh::Builder::Primitive::Point);

    m_builder.End(g_theRenderer->GetMaterial("__2D"));
}

void Satellite::Render() const noexcept {
    if (IsDead()) {
        return;
    }
    {
        g_theRenderer->SetModelMatrix();
        Mesh::Render(m_builder);
        g_theRenderer->DrawFilledCircle2D(GetCollisionMesh(), m_parentWave->GetObjectColor());
    }
}

void Satellite::DebugRender() const noexcept {
    if (IsDead()) {
        return;
    }
    g_theRenderer->SetMaterial("__2D");
    g_theRenderer->SetModelMatrix();
    g_theRenderer->DrawCircle2D(GetCollisionMesh(), Rgba::Orange);
}

void Satellite::EndFrame() noexcept {
    if (IsDead()) {
        auto* g = GetGameAs<Game>();
        g->CreateExplosionAt(m_position, Faction::Player);
    }
}

void Satellite::Kill() noexcept {
    m_health = 0;
}

bool Satellite::IsDead() const noexcept {
    return m_health < 1;
}

Disc2 Satellite::GetCollisionMesh() const noexcept {
    return Disc2{ m_position, m_radius};
}

Vector2 Satellite::GetPosition() const noexcept {
    return m_position;
}

float Satellite::GetFireRate() const noexcept {
    return m_parentWave->GetWaveId() < GameConstants::wave_flier_firerate_lookup.size() ? GameConstants::wave_flier_firerate_lookup[m_parentWave->GetWaveId()] : GameConstants::min_bomber_firerate;
}
