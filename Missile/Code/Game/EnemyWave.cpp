#include "Game/EnemyWave.hpp"

void EnemyWave::BeginFrame() noexcept {
    m_missiles.BeginFrame();
}

void EnemyWave::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    m_missiles.Update(deltaSeconds);
}

void EnemyWave::Render() const noexcept {
    m_missiles.Render();
}

void EnemyWave::EndFrame() noexcept {
    m_missiles.EndFrame();
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

constexpr const int EnemyWave::GetScoreMultiplier() const noexcept {
    return m_waveId < GameConstants::wave_score_multiplier_lookup.size() ? GameConstants::wave_score_multiplier_lookup[m_waveId] : GameConstants::max_score_multiplier;
}