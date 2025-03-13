#include "Game/City.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/Game.hpp"

#include <utility>

City::City(Vector2 position) noexcept
    : m_position{position}
{
    /* DO NOTHING */
}

void City::SetPosition(Vector2 position) noexcept {
    m_position = position;
}

void City::BeginFrame() noexcept {
    /* DO NOTHING */
}

void City::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void City::Render() const noexcept {
    auto* mat = g_theRenderer->GetMaterial("city");
    auto* tex = mat->GetTexture(Material::TextureID::Diffuse);
    const auto&& [x, y, _] = tex->GetDimensions().GetXYZ();
    const auto dims = IntVector2{ x, y };
    const auto S = Matrix4::CreateScaleMatrix(Vector2{ dims });
    const auto R = Matrix4::I;
    const auto T = Matrix4::CreateTranslationMatrix(m_position);
    const auto M = Matrix4::MakeSRT(S, R, T);
    g_theRenderer->SetMaterial(mat);
    g_theRenderer->DrawQuad2D(M, GetCityColor());

}

void City::DebugRender() const noexcept {
    g_theRenderer->SetModelMatrix();
    g_theRenderer->SetMaterial("__2D");
    g_theRenderer->DrawAABB2(GetCollisionMesh(), Rgba::Green, Rgba::NoAlpha);

}

void City::EndFrame() noexcept {
    /* DO NOTHING */
}

Rgba City::GetCityColor() const noexcept {
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    return IsDead() ? state->GetGroundColor() : state->GetPlayerColor();
}

AABB2 City::GetCollisionMesh() const noexcept {
    return AABB2{m_position, 33.0f, 24.0f };
}

bool City::IsDead() const noexcept {
    return m_health < 1;
}

void City::Kill() noexcept {
    m_health = 0;
}
