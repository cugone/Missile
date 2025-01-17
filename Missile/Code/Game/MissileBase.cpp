#include "Game/MissileBase.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <utility>

MissileBase::MissileBase(Vector2 position) noexcept
    : m_position{position}
    , m_missilesRemaining{m_maxMissiles}
{
    /* DO NOTHING */
}

void MissileBase::SetPosition(Vector2 position) noexcept {
    m_position = position;
}

void MissileBase::SetTimeToTarget(TimeUtils::FPSeconds newTimeToTarget) noexcept {
    m_timeToTarget = newTimeToTarget;
}

void MissileBase::BeginFrame() noexcept {
    m_missileManager.BeginFrame();
}

void MissileBase::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    m_missileManager.Update(deltaSeconds);
}

void MissileBase::Render() const noexcept {
    g_theRenderer->SetModelMatrix();
    g_theRenderer->SetMaterial("__2D");
    {
        const auto S = Matrix4::CreateScaleMatrix(Vector2{66.0f, 48.0f});
        const auto R = Matrix4::I;
        const auto T = Matrix4::CreateTranslationMatrix(m_position);
        const auto M = Matrix4::MakeSRT(S, R, T);
        g_theRenderer->DrawQuad2D(M, Rgba::Orange);
    }
    m_missileManager.Render();

    if(HasMissilesRemaining()) {
        RenderRemainingMissiles();
    }


    if(OutOfMissiles()) {
        const auto* font = g_theRenderer->GetFont("System32");
        const auto S = Matrix4::I;
        const auto R = Matrix4::I;
        const auto T = Matrix4::CreateTranslationMatrix(m_position + Vector2::X_Axis * -0.5f * font->CalculateTextWidth("OUT"));
        const auto M = Matrix4::MakeSRT(S, R, T);
        g_theRenderer->DrawTextLine(M, font, "OUT");
    }
}

void MissileBase::EndFrame() noexcept {
    m_missileManager.EndFrame();
}

void MissileBase::Fire(MissileManager::Target target) noexcept {
    if(HasMissilesRemaining()) {
        if(m_missileManager.LaunchMissile(GetMissileLauncherPosition(), target, m_timeToTarget, Faction::Player)) {
            DecrementMissiles();
        }
    } else {
        g_theAudioSystem->Play(GameConstants::game_audio_folder / std::filesystem::path{ "NoMissiles.wav" }, AudioSystem::SoundDesc{});
    }
}

bool MissileBase::HasMissilesRemaining() const noexcept {
    return m_missilesRemaining != 0;
}

bool MissileBase::OutOfMissiles() const noexcept {
    return !HasMissilesRemaining();
}

void MissileBase::DecrementMissiles() noexcept {
    m_missilesRemaining = (std::max)(0, m_missilesRemaining - 1);
}

void MissileBase::IncrementMissiles() noexcept {
    m_missilesRemaining = (std::min)(m_maxMissiles, m_missilesRemaining + 1);
}

const MissileManager& MissileBase::GetMissileManager() const noexcept {
    return m_missileManager;
}

MissileManager& MissileBase::GetMissileManager() noexcept {
    return m_missileManager;
}

void MissileBase::RemoveAllMissiles() noexcept {
    m_missilesRemaining = 0;
}

void MissileBase::RenderRemainingMissiles() const noexcept {
    auto* mat = g_theRenderer->GetMaterial("missile");
    const auto* tex = mat->GetTexture(Material::TextureID::Diffuse);
    const auto dims = Vector2{ IntVector2{tex->GetDimensions()} };
    const auto S = Matrix4::CreateScaleMatrix(dims);
    const auto R = Matrix4::I;
    const auto missilePositions = std::vector<Vector2>{
        m_position + Vector2{ 2.0f * dims.x, dims.y }
        ,m_position + Vector2{ dims.x, dims.y }
        ,m_position + Vector2{ -dims.x, dims.y }
        ,m_position + Vector2{ -2.0f * dims.x, dims.y }
        ,m_position + Vector2::X_Axis * dims.x
        ,m_position
        ,m_position - Vector2::X_Axis * dims.x
        ,m_position - Vector2{ dims.x * -0.5f, dims.y }
        ,m_position - Vector2{ dims.x * 0.5f, dims.y }
        ,m_position - (Vector2::Y_Axis * dims.y * 2.0f)
    };
    g_theRenderer->SetMaterial(mat);
    const auto s = missilePositions.size();
    const auto idx = s - m_missilesRemaining;
    for (std::size_t i = idx; i < s; ++i) {
        const auto T = Matrix4::CreateTranslationMatrix(missilePositions[i]);
        const auto M = Matrix4::MakeSRT(S, R, T);
        g_theRenderer->DrawQuad2D(M);
    }
}

Vector2 MissileBase::GetMissileLauncherPosition() noexcept {
    return m_position;
}
