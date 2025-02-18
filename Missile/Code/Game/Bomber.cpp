#include "Game/Bomber.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/EnemyWave.hpp"

Bomber::Bomber(EnemyWave* parent, Vector2 position) noexcept
    : m_position{ position }
    , m_parentWave{parent}
{
    g_theAudioSystem->Play(GameConstants::game_audio_bomber_path, AudioSystem::SoundDesc{});
    m_timeToFire.SetSeconds(TimeUtils::FPFrames{GetFireRate()});
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
    if(m_timeToFire.CheckAndReset()) {
        m_parentWave->LaunchMissileFrom(m_position);
    }
}

float Bomber::GetFireRate() const noexcept {
    return m_parentWave->GetWaveId() < GameConstants::wave_flier_firerate_lookup.size() ? GameConstants::wave_flier_firerate_lookup[m_parentWave->GetWaveId()] : GameConstants::min_bomber_firerate;
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
        g_theRenderer->DrawQuad2D(M, m_parentWave->GetObjectColor());
    }
}

void Bomber::DebugRender() const noexcept {
    if(IsDead()) {
        return;
    }
    g_theRenderer->SetMaterial("__2D");
    g_theRenderer->SetModelMatrix();
    g_theRenderer->DrawCircle2D(GetCollisionMesh(), Rgba::Orange);
}

void Bomber::EndFrame() noexcept {
    if(IsDead()) {
        auto* g = GetGameAs<Game>();
        g->CreateExplosionAt(m_position, Faction::Player);
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
