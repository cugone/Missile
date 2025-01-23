#pragma once

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
    void EndFrame() noexcept;

    const MissileManager& GetMissileManager() const noexcept;
    MissileManager& GetMissileManager() noexcept;

    std::size_t GetWaveId() const noexcept;
    void IncrementWave() noexcept;

    int GetScoreMultiplier() const noexcept;
    Rgba GetObjectColor() const noexcept;
    Rgba GetBackgroundColor() const noexcept;

    void SpawnBomber() noexcept;
    void SpawnSatellite() noexcept;
    void SpawnMissile() noexcept;

    Bomber* const GetBomber() const noexcept;
    Satellite* const GetSatellite() const noexcept;

protected:
private:
    void UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateBomber(TimeUtils::FPSeconds deltaSeconds) noexcept;

    MissileManager m_missiles{};
    std::unique_ptr<Bomber> m_bomber{};
    std::unique_ptr<Satellite> m_satellite{};
    std::size_t m_waveId{ 0 };
};
