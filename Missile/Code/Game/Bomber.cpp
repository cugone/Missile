#include "Game/Bomber.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

Bomber::Bomber(Vector2 position) noexcept
    : m_position{ position }
{
    g_theAudioSystem->Play(GameConstants::game_audio_bomber_path, AudioSystem::SoundDesc{});
}

void Bomber::BeginFrame() noexcept {
    if (IsDead()) {
        return;
    }
}

void Bomber::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(IsDead()) {
        return;
    }
    m_position += Vector2::X_Axis * m_speed * deltaSeconds.count();
}

void Bomber::Render() const noexcept {
    if(IsDead()) {
        return;
    }
    {
        auto* mat = g_theRenderer->GetMaterial("bomber");
        auto* tex = mat->GetTexture(Material::TextureID::Diffuse);
        const auto&& [x, y, _] = tex->GetDimensions().GetXYZ();
        const auto dims = IntVector2{x, y};
        const auto S = Matrix4::CreateScaleMatrix(Vector2{dims});
        const auto R = Matrix4::I;
        const auto T = Matrix4::CreateTranslationMatrix(m_position);
        const auto M = Matrix4::MakeSRT(S, R, T);
        g_theRenderer->SetMaterial(mat);
        g_theRenderer->DrawQuad2D(M, m_color);
    }
}

void Bomber::EndFrame() noexcept {
    if(IsDead()) {
        auto* g = GetGameAs<Game>();
        g->CreateExplosionAt(m_position, Faction::Enemy);
    }
}

void Bomber::Kill() noexcept {
    m_health = 0;
}

bool Bomber::IsDead() const noexcept {
    return m_health < 1;
}

Disc2 Bomber::GetCollisionMesh() const noexcept {
    return Disc2{m_position, 15.0f};
}

Vector2 Bomber::GetPosition() const noexcept {
    return m_position;
}

void Bomber::SetColor(Rgba newColor) noexcept {
    m_color = newColor;
}

