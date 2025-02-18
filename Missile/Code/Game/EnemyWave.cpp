#include "Game/EnemyWave.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Game/Game.hpp"

#include <utility>

void EnemyWave::BeginFrame() noexcept {
    switch (m_state) {
    case State::Inactive:
        BeginFrame_Inactive();
        break;
    case State::Prewave:
        BeginFrame_Prewave();
        break;
    case State::Active:
        BeginFrame_Active();
        break;
    case State::Postwave:
        BeginFrame_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    switch (m_state) {
    case State::Inactive:
        Update_Inactive(deltaSeconds);
        break;
    case State::Prewave:
        Update_Prewave(deltaSeconds);
        break;
    case State::Active:
        Update_Active(deltaSeconds);
        break;
    case State::Postwave:
        Update_Postwave(deltaSeconds);
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::Render() const noexcept {
    switch (m_state) {
    case State::Inactive:
        Render_Inactive();
        break;
    case State::Prewave:
        Render_Prewave();
        break;
    case State::Active:
        Render_Active();
        break;
    case State::Postwave:
        Render_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::EndFrame() noexcept {
    switch (m_state) {
    case State::Inactive:
        EndFrame_Inactive();
        break;
    case State::Prewave:
        EndFrame_Prewave();
        break;
    case State::Active:
        EndFrame_Active();
        break;
    case State::Postwave:
        EndFrame_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

/************************************************************************/
/*                         BEGIN FRAME                                  */
/************************************************************************/

void EnemyWave::BeginFrame_Inactive() noexcept {
    /* DO NOTHING */
}

void EnemyWave::BeginFrame_Prewave() noexcept {
    /* DO NOTHING */
}

void EnemyWave::BeginFrame_Active() noexcept {
    m_missiles.BeginFrame();
    if (m_bomber) {
        m_bomber->BeginFrame();
    }
    if (m_satellite) {
        m_satellite->BeginFrame();
    }
}

void EnemyWave::BeginFrame_Postwave() noexcept {
    /* DO NOTHING */
}

/************************************************************************/
/*                         UPDATE                                       */
/************************************************************************/


void EnemyWave::Update_Inactive([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {

}

void EnemyWave::Update_Prewave([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {

}

void EnemyWave::Update_Active(TimeUtils::FPSeconds deltaSeconds) noexcept {
    UpdateMissiles(deltaSeconds);
    UpdateBomber(deltaSeconds);
    UpdateSatellite(deltaSeconds);
}

void EnemyWave::Update_Postwave([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {

}

void EnemyWave::UpdateMissiles(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(IsWaveActive() && m_missileCount) {
        if(CanSpawnMissile()) {
            if(m_missileSpawnRate.CheckAndReset()) {
                SpawnMissile();
            }
        }
    }
    m_missiles.Update(deltaSeconds);
}

bool EnemyWave::CanSpawnMissile() const noexcept {
    return m_missileCount != 0 && m_missiles.ActiveMissileCount() < GameConstants::max_missles_on_screen;
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
        m_satellite.reset();
        m_flierSpawnRate.Reset();
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
        m_bomber.reset();
        m_flierSpawnRate.Reset();
    }
}

bool EnemyWave::LaunchMissileFrom(Vector2 position) noexcept {
    if(CanSpawnMissile()) {
        const auto* g = GetGameAs<Game>();
        const auto& targets = g->GetValidTargets();
        const auto& target = targets[MathUtils::GetRandomLessThan(targets.size())];
        m_missileCount = (std::max)(0, m_missileCount - 1);
        return m_missiles.LaunchMissile(position, target, GetMissileImpactTime(), Faction::Enemy, GetObjectColor());
    }
    return false;
}

/************************************************************************/
/*                         RENDER                                       */
/************************************************************************/

void EnemyWave::Render_Inactive() const noexcept {

}

void EnemyWave::Render_Prewave() const noexcept {

}

void EnemyWave::Render_Active() const noexcept {
    m_missiles.Render();
    if (m_bomber) {
        m_bomber->Render();
    }
    if (m_satellite) {
        m_satellite->Render();
    }
}

void EnemyWave::Render_Postwave() const noexcept {

}

void EnemyWave::DebugRender() const noexcept {
    switch (m_state) {
    case State::Inactive:
        DebugRender_Inactive();
        break;
    case State::Prewave:
        DebugRender_Prewave();
        break;
    case State::Active:
        DebugRender_Active();
        break;
    case State::Postwave:
        DebugRender_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::DebugRender_Inactive() const noexcept {

}

void EnemyWave::DebugRender_Prewave() const noexcept {

}

void EnemyWave::DebugRender_Active() const noexcept {
    m_missiles.DebugRender();
    if (m_bomber) {
        m_bomber->DebugRender();
    }
    if (m_satellite) {
        m_satellite->DebugRender();
    }
}

void EnemyWave::DebugRender_Postwave() const noexcept {

}

/************************************************************************/
/*                         END FRAME                                    */
/************************************************************************/

void EnemyWave::EndFrame_Inactive() noexcept {

}

void EnemyWave::EndFrame_Prewave() noexcept {

}

void EnemyWave::EndFrame_Active() noexcept {
    m_missiles.EndFrame();
    if (m_bomber) {
        m_bomber->EndFrame();
        if (m_bomber->IsDead()) {
            m_bomber.reset();
        }
    }
    if (m_satellite) {
        m_satellite->EndFrame();
        if (m_satellite->IsDead()) {
            m_satellite.reset();
        }
    }
    if (CanSpawnFlier()) {
        if (const auto is_bomber = MathUtils::GetRandomBool(); is_bomber) {
            if (!m_bomber) {
                SpawnBomber();
                m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ GetFlierCooldown() });
                m_flierSpawnRate.Reset();
            }
        } else {
            if (!m_satellite) {
                SpawnSatellite();
                m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ GetFlierCooldown() });
                m_flierSpawnRate.Reset();
            }
        }
    }
    if (IsWaveOver()) {
        AdvanceToNextWave();
    }
}

void EnemyWave::EndFrame_Postwave() noexcept {

}

void EnemyWave::AdvanceToNextWave() noexcept {
    DeactivateWave();
    m_bomber.reset();
    m_satellite.reset();
    IncrementWave();
    SetMissileCount(GetMissileCount());
    m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ GetFlierCooldown() });
    m_flierSpawnRate.Reset();
    if (auto* g = GetGameAs<Game>(); g != nullptr) {
        g->ResetMissileCount();
    }
    ActivateWave();
}

bool EnemyWave::CanSpawnFlier() const noexcept {
    return IsWaveActive() && m_waveId > 0 && m_missileCount && (!m_bomber || !m_satellite) && m_flierSpawnRate.Check();
}

bool EnemyWave::IsWaveOver() const noexcept {
    if (auto* g = GetGameAs<Game>(); g != nullptr) {
        const auto all_explosions_finished = g->GetExplosionManager().ActiveExplosionCount() == 0;
        const auto no_missiles_in_flight = g->GetMissileManager().ActiveMissileCount() == 0;
        const auto player_has_no_missiles_remaining = !g->HasMissilesRemaining();
        const auto wave_has_no_missiles_remaining = m_missileCount == 0;
        const auto cant_score_points = player_has_no_missiles_remaining && no_missiles_in_flight && all_explosions_finished;
        const auto everything_dead = wave_has_no_missiles_remaining && !m_bomber && !m_satellite && all_explosions_finished;
        if (cant_score_points || everything_dead) {
            return true;
        }
    }
    return false;
}

float EnemyWave::GetFlierCooldown() const noexcept {
    return m_waveId < GameConstants::wave_flier_cooldown_lookup.size() ? GameConstants::wave_flier_cooldown_lookup[m_waveId] : GameConstants::min_bomber_cooldown;
}

const MissileManager& EnemyWave::GetMissileManager() const noexcept {
    return m_missiles;
}

MissileManager& EnemyWave::GetMissileManager() noexcept {
    return m_missiles;
}

std::size_t EnemyWave::GetWaveId() const noexcept {
    return m_waveId;
}

void EnemyWave::IncrementWave() noexcept {
    m_waveId += 1;
}

int EnemyWave::GetScoreMultiplier() const noexcept {
    return m_waveId < GameConstants::wave_score_multiplier_lookup.size() ? GameConstants::wave_score_multiplier_lookup[m_waveId] : GameConstants::max_score_multiplier;
}

Rgba EnemyWave::GetObjectColor() const noexcept {
    return Rgba(GameConstants::wave_object_color_lookup[m_waveId % GameConstants::wave_array_size]);
}

Rgba EnemyWave::GetBackgroundColor() const noexcept {
    return Rgba(GameConstants::wave_background_color_lookup[m_waveId % GameConstants::wave_array_size]);
}

TimeUtils::FPSeconds EnemyWave::GetMissileImpactTime() const noexcept {
    return TimeUtils::FPSeconds{m_waveId < GameConstants::wave_missile_impact_time.size() ? GameConstants::wave_missile_impact_time[m_waveId] : GameConstants::min_missile_impact_time};
}

int EnemyWave::GetMissileCount() const noexcept {
    return m_waveId < GameConstants::wave_missile_count_lookup.size() ? GameConstants::wave_missile_count_lookup[m_waveId] : GameConstants::max_missile_count;
}

void EnemyWave::SetMissileSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept {
    m_missileSpawnRate.SetSeconds(secondsBetween);
}

void EnemyWave::SetBomberSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept {
    m_flierSpawnRate.SetSeconds(secondsBetween);
}

void EnemyWave::SetMissileCount(int newMissileCount) noexcept {
    m_missileCount = newMissileCount;
}

int EnemyWave::GetRemainingMissiles() const noexcept {
    return m_missileCount;
}

bool EnemyWave::IsWaveActive() const noexcept {
    return m_state == State::Active;
}

void EnemyWave::ActivateWave() noexcept {
    SetState(State::Active);
}

void EnemyWave::DeactivateWave() noexcept {
    SetState(State::Inactive);
}

void EnemyWave::SetState(State newState) noexcept {
    m_state = newState;
}

void EnemyWave::SpawnBomber() noexcept {
    if(m_bomber) {
        return;
    }
    auto* g = GetGameAs<Game>();
    AABB2 bomber_spawn_area = g->GetWorldBounds();
    bomber_spawn_area.Translate(Vector2::X_Axis * -100.0f);
    bomber_spawn_area.AddPaddingToSides(0.0f, -GameConstants::radar_line_distance);
    bomber_spawn_area.maxs.x = g->GetWorldBounds().mins.x;
    m_bomber = std::make_unique<Bomber>(this, MathUtils::GetRandomPointInside(bomber_spawn_area));
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
    m_satellite = std::make_unique<Satellite>(this, MathUtils::GetRandomPointInside(satellite_spawn_area));
}

void EnemyWave::SpawnMissile() noexcept {
    auto* g = GetGameAs<Game>();
    AABB2 missile_spawn_area = g->GetWorldBounds();
    missile_spawn_area.Translate(Vector2::Y_Axis * -100.0f);
    missile_spawn_area.AddPaddingToSides(-100.0f, 0.0f);
    missile_spawn_area.maxs.y = g->GetWorldBounds().mins.y;
    Vector2 pos = MathUtils::GetRandomPointInside(missile_spawn_area);

    LaunchMissileFrom(pos);
}

Bomber* const EnemyWave::GetBomber() const noexcept {
    return m_bomber.get();
}

Satellite* const EnemyWave::GetSatellite() const noexcept {
    return m_satellite.get();
}
