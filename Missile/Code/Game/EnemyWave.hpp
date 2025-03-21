#pragma once

#include "Engine/Core/Stopwatch.hpp"

#include "Game/EnemyWaveState.hpp"

#include "Game/MissileManager.hpp"
#include "Game/Bomber.hpp"
#include "Game/Satellite.hpp"

#include <cstdint>
#include <memory>

class EnemyWave {
public:
    EnemyWave() noexcept;
    EnemyWave(const EnemyWave& other) = default;
    EnemyWave(EnemyWave&& other) = default;
    EnemyWave& operator=(const EnemyWave& other) = default;
    EnemyWave& operator=(EnemyWave&& other) = default;
    ~EnemyWave() = default;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void DebugRender() const noexcept;
    void EndFrame() noexcept;

    const MissileManager* GetMissileManager() const noexcept;
    MissileManager* GetMissileManager() noexcept;

    std::size_t GetWaveId() const noexcept;
    void IncrementWave() noexcept;

    int GetScoreMultiplier() const noexcept;
    Rgba GetObjectColor() const noexcept;
    Rgba GetBackgroundColor() const noexcept;
    TimeUtils::FPSeconds GetMissileImpactTime() const noexcept;

    Bomber* const GetBomber() const noexcept;
    Satellite* const GetSatellite() const noexcept;

    void DecrementMissileCount() noexcept;
    void SetMissileCount(int newMissileCount) noexcept;
    int GetMissileCountForWave() const noexcept;
    int GetRemainingMissiles() const noexcept;

    bool IsWaveActive() const noexcept;
    void ActivateWave() noexcept;
    void DeactivateWave() noexcept;

    void SetMissileSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept;
    void SetBomberSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept;
    float GetFlierCooldown() const noexcept;

    bool CanSpawnMissile() const noexcept;
    bool LaunchMissileFrom(Vector2 position) noexcept;

    const EnemyWaveState* GetCurrentState() const noexcept;
    EnemyWaveState* GetCurrentState() noexcept;

    void ChangeState(std::unique_ptr<EnemyWaveState> newState) noexcept;
protected:
private:
    std::size_t m_waveId{ 0 };
    std::unique_ptr<EnemyWaveState> m_currentState{};
    std::unique_ptr<EnemyWaveState> m_nextState{};
    Stopwatch m_missileSpawnRate{};
    Stopwatch m_flierSpawnRate{};
    int m_missileCount{0};
    bool m_isActive{false};
};
