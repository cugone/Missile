#include "Game/EnemyWave.hpp"

#include "Engine/Audio/AudioSystem.hpp"

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

#include <algorithm>
#include <format>
#include <utility>

EnemyWave::EnemyWave() noexcept {
    m_currentState = std::move(std::make_unique<EnemyWaveStatePrewave>(this));
}

void EnemyWave::BeginFrame() noexcept {
    if (m_nextState) {
        m_currentState->OnExit();
        m_currentState = std::move(m_nextState);
        m_currentState->OnEnter();
        m_nextState.reset(nullptr);
    }
    m_currentState->BeginFrame();
}

void EnemyWave::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    m_currentState->Update(deltaSeconds);
}

void EnemyWave::Render() const noexcept {
    m_currentState->Render();
}

void EnemyWave::DebugRender() const noexcept {
    m_currentState->DebugRender();
}

void EnemyWave::EndFrame() noexcept {
    m_currentState->EndFrame();
}

const EnemyWaveState* EnemyWave::GetCurrentState() const noexcept {
    return m_currentState.get();
}

EnemyWaveState* EnemyWave::GetCurrentState() noexcept {
    return m_currentState.get();
}

void EnemyWave::ChangeState(std::unique_ptr<EnemyWaveState> newState) noexcept {
    m_nextState = std::move(newState);
}

float EnemyWave::GetFlierCooldown() const noexcept {
    return m_waveId < GameConstants::wave_flier_cooldown_lookup.size() ? GameConstants::wave_flier_cooldown_lookup[m_waveId] : GameConstants::min_bomber_cooldown;
}

bool EnemyWave::CanSpawnMissile() const noexcept {
    auto* state = GetCurrentState();
    if (auto* active_state = dynamic_cast<const EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->CanSpawnMissile();
    }
    return false;
}

bool EnemyWave::LaunchMissileFrom(Vector2 position) noexcept {
    auto* state = GetCurrentState();
    if (auto* active_state = dynamic_cast<EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->LaunchMissileFrom(position);
    }
    return false;
}

const MissileManager* EnemyWave::GetMissileManager() const noexcept {
    const auto* state = GetCurrentState();
    if (const auto* main_state = dynamic_cast<const EnemyWaveStateActive*>(state); main_state != nullptr) {
        return main_state->GetMissileManager();
    }
    return nullptr;
}

MissileManager* EnemyWave::GetMissileManager() noexcept {
    auto* state = GetCurrentState();
    if (auto* main_state = dynamic_cast<EnemyWaveStateActive*>(state); main_state != nullptr) {
        return main_state->GetMissileManager();
    }
    return nullptr;

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

Bomber* const EnemyWave::GetBomber() const noexcept {
    const auto* state = GetCurrentState();
    if (const auto* active_state = dynamic_cast<const EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->GetBomber();
    }
    return nullptr;
}

Satellite* const EnemyWave::GetSatellite() const noexcept {
    const auto* state = GetCurrentState();
    if (const auto* active_state = dynamic_cast<const EnemyWaveStateActive*>(state); active_state != nullptr) {
        return active_state->GetSatellite();
    }
    return nullptr;
}

void EnemyWave::DecrementMissileCount() noexcept {
    m_missileCount = (std::max)(0, m_missileCount - 1);
}

int EnemyWave::GetMissileCountForWave() const noexcept {
    return m_waveId < GameConstants::wave_missile_count_lookup.size() ? GameConstants::wave_missile_count_lookup[m_waveId] : GameConstants::max_enemy_missile_count;
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
    return m_isActive;
}

void EnemyWave::ActivateWave() noexcept {
    m_isActive = true;
}

void EnemyWave::DeactivateWave() noexcept {
    m_isActive = false;
}
