#pragma once

#include "Engine/Core/Stopwatch.hpp"

#include "Game/MissileManager.hpp"
#include "Game/Bomber.hpp"
#include "Game/Satellite.hpp"

#include <memory>

class EnemyWave {
public:
    EnemyWave() = default;
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

    const MissileManager& GetMissileManager() const noexcept;
    MissileManager& GetMissileManager() noexcept;

    std::size_t GetWaveId() const noexcept;
    void IncrementWave() noexcept;

    int GetScoreMultiplier() const noexcept;
    Rgba GetObjectColor() const noexcept;
    Rgba GetBackgroundColor() const noexcept;
    TimeUtils::FPSeconds GetMissileImpactTime() const noexcept;

    void SpawnBomber() noexcept;
    void SpawnSatellite() noexcept;
    void SpawnMissile() noexcept;

    Bomber* const GetBomber() const noexcept;
    Satellite* const GetSatellite() const noexcept;

    void SetMissileCount(int newMissileCount) noexcept;
    
    bool IsWaveActive() const noexcept;
    void ActivateWave() noexcept;
    void DeactivateWave() noexcept;

    void SetMissileSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept;

protected:
private:
    void UpdateMissiles(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateBomber(TimeUtils::FPSeconds deltaSeconds) noexcept;

    MissileManager m_missiles{};
    std::unique_ptr<Bomber> m_bomber{};
    std::unique_ptr<Satellite> m_satellite{};
    Stopwatch m_missileSpawnRate{};
    std::size_t m_waveId{ 0 };
    int m_missileCount{};
    bool m_waveActive{false};
};
