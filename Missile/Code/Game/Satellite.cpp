#include "Game/Satellite.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Services/IAudioService.hpp"
#include "Engine/Services/ServiceLocator.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

Satellite::Satellite(Vector2 position) noexcept
    : m_position{position}
{
    g_theAudioSystem->Play(GameConstants::game_audio_satellite_path, AudioSystem::SoundDesc{});
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
    m_builder.Begin(PrimitiveType::Lines);

    m_builder.SetColor(m_color);

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
        g_theRenderer->DrawFilledCircle2D(GetCollisionMesh(), m_color);
    }
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

void Satellite::SetColor(Rgba newColor) noexcept {
    m_color = newColor;
}
