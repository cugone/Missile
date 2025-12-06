#include "Game/EnemyWave.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IRendererService.hpp"
#include "Engine/Services/IInputService.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"
#include "Game/EnemyWaveStatePrewave.hpp"
#include "Game/EnemyWaveStateActive.hpp"

#ifdef PROFILE_BUILD
#include <Thirdparty/Tracy/tracy/Tracy.hpp>
#endif

#include <algorithm>
#include <format>
#include <utility>

EnemyWave::EnemyWave() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_currentState = std::move(std::make_unique<EnemyWaveStatePrewave>(this));
}

void EnemyWave::BeginFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    if (m_nextState) {
        m_currentState->OnExit();
        m_currentState = std::move(m_nextState);
        m_currentState->OnEnter();
        m_nextState.reset(nullptr);
    }
    m_currentState->BeginFrame();
}

void EnemyWave::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_currentState->Update(deltaSeconds);
}

void EnemyWave::Render() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_currentState->Render();
}

void EnemyWave::DebugRender() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_currentState->DebugRender();
}

void EnemyWave::EndFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_currentState->EndFrame();
}

const EnemyWaveState* EnemyWave::GetCurrentState() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_currentState.get();
}

EnemyWaveState* EnemyWave::GetCurrentState() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_currentState.get();
}

void EnemyWave::ChangeState(std::unique_ptr<EnemyWaveState> newState) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_nextState = std::move(newState);
}

float EnemyWave::GetFlierCooldown() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_waveId < GameConstants::wave_flier_cooldown_lookup.size() ? GameConstants::wave_flier_cooldown_lookup[m_waveId] : GameConstants::min_bomber_cooldown;
}

bool EnemyWave::CanSpawnMissile() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    auto* state = GetCurrentState();
    if (auto* active_state = dynamic_cast<const EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->CanSpawnMissile();
    }
    return false;
}

bool EnemyWave::LaunchMissileFrom(Vector2 position) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    auto* state = GetCurrentState();
    if (auto* active_state = dynamic_cast<EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->LaunchMissileFrom(position);
    }
    return false;
}

int EnemyWave::GetSmartBombCountForWave() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_waveId < GameConstants::wave_smartbomb_count_lookup.size() ? GameConstants::wave_smartbomb_count_lookup[m_waveId] : GameConstants::max_smartbomb_count;
}

void EnemyWave::DecrementSmartBombCount() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_smartBombCount = (std::max)(0, m_smartBombCount - 1);
}

int EnemyWave::GetRemainingSmartBombs() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_smartBombCount;
}

bool EnemyWave::CanSpawnSmartBomb() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    auto* state = GetCurrentState();
    if (auto* active_state = dynamic_cast<const EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->CanSpawnSmartBomb();
    }
    return false;
}

bool EnemyWave::LaunchSmartBombFrom(Vector2 position) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    auto* state = GetCurrentState();
    if(auto* active_state = dynamic_cast<EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->LaunchSmartBombFrom(position);
    }
    return false;
}

const MissileManager* EnemyWave::GetMissileManager() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    const auto* state = GetCurrentState();
    if (const auto* main_state = dynamic_cast<const EnemyWaveStateActive*>(state); main_state != nullptr) {
        return main_state->GetMissileManager();
    }
    return nullptr;
}

MissileManager* EnemyWave::GetMissileManager() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    auto* state = GetCurrentState();
    if (auto* main_state = dynamic_cast<EnemyWaveStateActive*>(state); main_state != nullptr) {
        return main_state->GetMissileManager();
    }
    return nullptr;

}

std::size_t EnemyWave::GetWaveId() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_waveId;
}

void EnemyWave::IncrementWave() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_waveId += 1;
}

int EnemyWave::GetScoreMultiplier() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_waveId < GameConstants::wave_score_multiplier_lookup.size() ? GameConstants::wave_score_multiplier_lookup[m_waveId] : GameConstants::max_score_multiplier;
}

Rgba EnemyWave::GetObjectColor() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return Rgba(GameConstants::wave_object_color_lookup[m_waveId % GameConstants::wave_array_size]);
}

Rgba EnemyWave::GetBackgroundColor() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return Rgba(GameConstants::wave_background_color_lookup[m_waveId % GameConstants::wave_array_size]);
}

TimeUtils::FPSeconds EnemyWave::GetMissileImpactTime() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return TimeUtils::FPSeconds{m_waveId < GameConstants::wave_missile_impact_time.size() ? GameConstants::wave_missile_impact_time[m_waveId] : GameConstants::min_missile_impact_time};
}

Bomber* const EnemyWave::GetBomber() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    const auto* state = GetCurrentState();
    if (const auto* active_state = dynamic_cast<const EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->GetBomber();
    }
    return nullptr;
}

Satellite* const EnemyWave::GetSatellite() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    const auto* state = GetCurrentState();
    if (const auto* active_state = dynamic_cast<const EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->GetSatellite();
    }
    return nullptr;
}

void EnemyWave::DecrementMissileCount() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_missileCount = (std::max)(0, m_missileCount - 1);
}

int EnemyWave::GetMissileCountForWave() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_waveId < GameConstants::wave_missile_count_lookup.size() ? GameConstants::wave_missile_count_lookup[m_waveId] : GameConstants::max_enemy_missile_count;
}

void EnemyWave::SetMissileSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_missileSpawnRate.SetSeconds(secondsBetween);
}

void EnemyWave::SetBomberSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_flierSpawnRate.SetSeconds(secondsBetween);
}

void EnemyWave::SetMissileCount(int newMissileCount) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_missileCount = newMissileCount;
}

void EnemyWave::SetSmartBombCount(int newSmartBombCount) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_smartBombCount = newSmartBombCount;
}

int EnemyWave::GetRemainingMissiles() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_missileCount;
}

bool EnemyWave::IsWaveActive() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_isActive;
}

void EnemyWave::ActivateWave() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_isActive = true;
}

void EnemyWave::DeactivateWave() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_isActive = false;
}
