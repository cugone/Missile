#include "Game/SmartBomb.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Game/EnemyWave.hpp"
#include "Game/ExplosionManager.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameStateMain.hpp"

#include <utility>
#include <vector>

SmartBomb::SmartBomb(ExplosionManager* explosionManager, Vector2 startPosition, Vector2 target) noexcept
    : m_startPosition{startPosition}
    , m_position{startPosition}
    , m_target{target}
    , m_color{Rgba::Random()}
    , m_explosionManager{explosionManager}
{
    m_velocity = (m_target - m_position).GetNormalize() * m_speed;
}

void SmartBomb::BeginFrame() noexcept {
    if(IsDead()) {
        return;
    }
    m_color = Rgba::Random();
}

void SmartBomb::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(IsDead()) {
        return;
    }
    if (const auto new_direction = AvoidExplosions(); new_direction.y < 0.0f) {
        //Only choice is "up".
        Kill();
    } else {
        m_velocity = new_direction * m_speed * deltaSeconds.count();
        m_position += m_velocity;
    }
}

void SmartBomb::Render() const noexcept {
    if(IsDead()) {
        return;
    }
    g_theRenderer->SetMaterial("__2D");
    g_theRenderer->SetModelMatrix();
    g_theRenderer->DrawFilledCircle2D(GetCollisionMesh(), m_color);
}

void SmartBomb::DebugRender() const noexcept {
    if(IsDead()) {
        return;
    }
    g_theRenderer->SetMaterial("__2D");
    g_theRenderer->SetModelMatrix();
    g_theRenderer->DrawCircle2D(GetCollisionMesh(), Rgba::Orange);
}

void SmartBomb::EndFrame() noexcept {
    if (IsDead()) {
        auto* g = GetGameAs<Game>();
        auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
        state->CreateExplosionAt(m_position, Faction::Player);
    }
}

void SmartBomb::Kill() noexcept {
    m_health = 0;
}

bool SmartBomb::IsDead() const noexcept {
    return m_health < 1;
}

Disc2 SmartBomb::GetCollisionMesh() const noexcept {
    return Disc2(m_position, m_radius);
}

Vector2 SmartBomb::GetPosition() const noexcept {
    return m_position;
}

Vector2 SmartBomb::AvoidExplosions() noexcept {
    const auto dot_products_to_explosions = [&]() -> std::vector<std::pair<Disc2, float>> {
        std::vector<std::pair<Disc2, float>> result;
        result.reserve(m_explosionManager->ActiveExplosionCount());
        for (const auto& e : m_explosionManager->GetExplosionCollisionMeshes()) {
            result.emplace_back(std::make_pair(e, MathUtils::DotProduct((m_target - e.center).GetNormalize(), m_velocity.GetNormalize())));
        }
        return result;
        }();
    Vector2 new_direction{ m_velocity.GetNormalize() };
    for (const auto& edp : dot_products_to_explosions) {
        //Explosion practically missed or is behind us.
        if (edp.second < 0.0f) {
            continue;
        }
        //Direct hit
        if (MathUtils::CalcDistanceSquared(edp.first.center, m_position) < m_radius * m_radius + edp.first.radius * edp.first.radius) {
            Kill();
            break;
        }
        //Explosion is "directly" in front.
        const auto direction_from_explosion = (m_position - edp.first.center).GetNormalize();
        auto dp = MathUtils::DotProduct(direction_from_explosion, m_velocity.GetNormalize());
        if (dp > 0.75f) {
            if (MathUtils::DoDiscsOverlap(edp.first, Disc2{ m_position, m_sensorRadius })) {
                //Avoid explosion by turning in the opposite direction
                new_direction.RotateRadians(-std::acos(dp));
                break;
            }
        }

    }
    return new_direction;
}

