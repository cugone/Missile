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
    if (IsDead()) {
        return;
    }
}

void Satellite::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if (IsDead()) {
        return;
    }
    m_position += -Vector2::X_Axis * m_speed * deltaSeconds.count();
}

void Satellite::Render() const noexcept {
    if (IsDead()) {
        return;
    }
    {
        auto* mat = g_theRenderer->GetMaterial("satellite");
        auto* tex = mat->GetTexture(Material::TextureID::Diffuse);
        const auto&& [x, y, _] = tex->GetDimensions().GetXYZ();
        const auto dims = IntVector2{ x, y };
        const auto S = Matrix4::CreateScaleMatrix(Vector2{ dims });
        const auto R = Matrix4::I;
        const auto T = Matrix4::CreateTranslationMatrix(m_position);
        const auto M = Matrix4::MakeSRT(S, R, T);
        g_theRenderer->SetMaterial("__2D");
        g_theRenderer->DrawQuad2D(M, Rgba::Red);
    }
}

void Satellite::EndFrame() noexcept {
    if (IsDead()) {
        auto* g = GetGameAs<Game>();
        g->CreateExplosionAt(m_position, Faction::Enemy);
    }
}

void Satellite::Kill() noexcept {
    m_health = 0;
}

bool Satellite::IsDead() const noexcept {
    return m_health < 1;
}

Disc2 Satellite::GetCollisionMesh() const noexcept {
    return Disc2{ m_position, 50.0f};
}

Vector2 Satellite::GetPosition() const noexcept {
    return m_position;
}
