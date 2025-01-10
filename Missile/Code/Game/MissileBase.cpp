#include "Game/MissileBase.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

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
        if(m_missileManager.LaunchMissile(GetMissileLauncherPosition(), target, TimeUtils::FPSeconds{ 1.0f })) {
            DecrementMissiles();
        }
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
    /* DO NOTHING */
}

Vector2 MissileBase::GetMissileLauncherPosition() noexcept {
    return m_position;
}
