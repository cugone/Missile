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
    m_timeToFire.SetSeconds(TimeUtils::FPSeconds{MathUtils::GetRandomInRange(2.0f, 15.0f)});
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
        const auto* g = GetGameAs<Game>();
        const auto& targets = g->GetValidTargets();
        const auto& target = targets[MathUtils::GetRandomLessThan(targets.size())];
        m_parentWave->GetMissileManager().LaunchMissile(m_position, target, TimeUtils::FPSeconds{ 5.0f }, Faction::Enemy, m_parentWave->GetObjectColor());
    }
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
