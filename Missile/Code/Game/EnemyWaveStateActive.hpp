#pragma once

#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Game/EnemyWaveState.hpp"

#include "Game/Bomber.hpp"
#include "Game/MissileManager.hpp"
#include "Game/Satellite.hpp"

#include <memory>

class EnemyWave;

class EnemyWaveStateActive : public EnemyWaveState {
public:
    explicit EnemyWaveStateActive(EnemyWave* context) noexcept;
    EnemyWaveStateActive(const EnemyWaveStateActive& other) noexcept = default;
    EnemyWaveStateActive(EnemyWaveStateActive&& other) noexcept = default;
    EnemyWaveStateActive& operator=(const EnemyWaveStateActive& other) noexcept = default;
    EnemyWaveStateActive& operator=(EnemyWaveStateActive&& other) noexcept = default;
    virtual ~EnemyWaveStateActive() noexcept = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void DebugRender() const noexcept override;
    void EndFrame() noexcept override;

    const MissileManager* GetMissileManager() const noexcept;
    MissileManager* GetMissileManager() noexcept;

    Bomber* const GetBomber() const noexcept;

    Satellite* const GetSatellite() const noexcept;

    bool CanSpawnMissile() const noexcept;
    bool LaunchMissileFrom(Vector2 position) noexcept;
protected:
private:


    bool CanSpawnFlier() const noexcept;

    bool IsWaveOver() const noexcept;

    void SpawnBomber() noexcept;

    void SpawnSatellite() noexcept;

    void SpawnMissile() noexcept;

    void UpdateMissiles(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateBomber(TimeUtils::FPSeconds deltaSeconds) noexcept;

    void AdvanceToNextWave() noexcept;

    void ClayActive() noexcept;
    void RenderScoreElement() const noexcept;
    void RenderScoreMultiplierElement() const noexcept;

    EnemyWave* m_context{nullptr};
    MissileManager m_missiles{};
    std::unique_ptr<Bomber> m_bomber{};
    std::unique_ptr<Satellite> m_satellite{};
    Stopwatch m_missileSpawnRate{};
    Stopwatch m_flierSpawnRate{};

};
