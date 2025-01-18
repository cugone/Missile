#include "Game/EnemyWave.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/Game.hpp"

void EnemyWave::BeginFrame() noexcept {
    m_missiles.BeginFrame();
    if(m_bomber) {
        m_bomber->BeginFrame();
    }
    if(m_satellite) {
        m_satellite->BeginFrame();
    }
}

void EnemyWave::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    m_missiles.Update(deltaSeconds);
    UpdateBomber(deltaSeconds);
    UpdateSatellite(deltaSeconds);
}

void EnemyWave::UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(!m_satellite) {
        return;
    }
    m_satellite->Update(deltaSeconds);
    Disc2 satellite_visible{ m_satellite->GetPosition(), 50.0f };
    const auto satellite_right = satellite_visible.center.x + satellite_visible.radius;
    auto* g = GetGameAs<Game>();
    AABB2 bounds = g->GetWorldBounds();
    const auto bounds_left = bounds.mins.x;
    if (satellite_right < bounds_left) {
        m_satellite->Kill();
    }
}

void EnemyWave::UpdateBomber(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(!m_bomber) {
        return;
    }
    m_bomber->Update(deltaSeconds);
    Disc2 bomber_visible{ m_bomber->GetPosition(), 50.0f };
    const auto bomber_left = bomber_visible.center.x - bomber_visible.radius;
    auto* g = GetGameAs<Game>();
    AABB2 bounds = g->GetWorldBounds();
    const auto bounds_right = bounds.maxs.x;
    if(bounds_right < bomber_left) {
        m_bomber->Kill();
    }
}

void EnemyWave::Render() const noexcept {
    m_missiles.Render();
    if(m_bomber) {
        m_bomber->Render();
    }
    if(m_satellite) {
        m_satellite->Render();
    }
}

void EnemyWave::EndFrame() noexcept {
    m_missiles.EndFrame();
    if(m_bomber) {
        m_bomber->EndFrame();
        if(m_bomber->IsDead()) {
            m_bomber.reset();
        }
    }
    if(m_satellite) {
        m_satellite->EndFrame();
        if(m_satellite->IsDead()) {
            m_satellite.reset();
        }
    }
}

const MissileManager& EnemyWave::GetMissileManager() const noexcept {
    return m_missiles;
}

MissileManager& EnemyWave::GetMissileManager() noexcept {
    return m_missiles;
}

void EnemyWave::IncrementWave() noexcept {
    m_waveId += 1;
}

int EnemyWave::GetScoreMultiplier() const noexcept {
    return m_waveId < GameConstants::wave_score_multiplier_lookup.size() ? GameConstants::wave_score_multiplier_lookup[m_waveId] : GameConstants::max_score_multiplier;
}

void EnemyWave::SpawnBomber() noexcept {
    if(m_bomber) {
        return;
    }
    auto* g = GetGameAs<Game>();
    AABB2 bomber_spawn_area = g->GetWorldBounds();
    bomber_spawn_area.Translate(Vector2::X_Axis * -100.0f);
    bomber_spawn_area.AddPaddingToSides(0.0f, -100.0f);
    bomber_spawn_area.maxs.x = g->GetWorldBounds().mins.x;
    m_bomber = std::make_unique<Bomber>(MathUtils::GetRandomPointInside(bomber_spawn_area));

}

void EnemyWave::SpawnSatellite() noexcept {
    if (m_satellite) {
        return;
    }
    auto* g = GetGameAs<Game>();
    AABB2 satellite_spawn_area = g->GetWorldBounds();
    satellite_spawn_area.Translate(Vector2::X_Axis * 100.0f);
    satellite_spawn_area.AddPaddingToSides(0.0f, -100.0f);
    satellite_spawn_area.mins.x = g->GetWorldBounds().maxs.x;
    m_satellite = std::make_unique<Satellite>(MathUtils::GetRandomPointInside(satellite_spawn_area));

}

void EnemyWave::SpawnMissile() noexcept {
    auto* g = GetGameAs<Game>();
    AABB2 missile_spawn_area = g->GetWorldBounds();
    missile_spawn_area.Translate(Vector2::Y_Axis * -100.0f);
    missile_spawn_area.AddPaddingToSides(-100.0f, 0.0f);
    missile_spawn_area.maxs.y = g->GetWorldBounds().mins.y;
    Vector2 pos = MathUtils::GetRandomPointInside(missile_spawn_area);
    MissileManager::Target target{g->BaseLocationCenter()};
    m_missiles.LaunchMissile(pos, target, TimeUtils::FPSeconds{}, Faction::Enemy);
}

Bomber* const EnemyWave::GetBomber() const noexcept {
    return m_bomber.get();
}

Satellite* const EnemyWave::GetSatellite() const noexcept {
    return m_satellite.get();
}
