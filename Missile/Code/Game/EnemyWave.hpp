#pragma once

#include "Engine/Core/Stopwatch.hpp"

#include "Game/MissileManager.hpp"
#include "Game/Bomber.hpp"
#include "Game/Satellite.hpp"

#include <cstdint>
#include <memory>

class EnemyWave {
public:
    enum class State : uint8_t {
        Inactive
        ,Prewave
        ,Active
        ,Postwave
    };
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
    int GetMissileCount() const noexcept;
    int GetRemainingMissiles() const noexcept;

    bool IsWaveActive() const noexcept;
    void ActivateWave() noexcept;
    void DeactivateWave() noexcept;
    bool IsWaveOver() const noexcept;

    void SetMissileSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept;
    void SetBomberSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept;
    float GetFlierCooldown() const noexcept;

    bool CanSpawnMissile() const noexcept;
    bool LaunchMissileFrom(Vector2 position) noexcept;

    void ChangeState(State newState) noexcept;
protected:
private:

    void ClayPrewave() noexcept;

    bool CanSpawnFlier() const noexcept;
    void AdvanceToNextWave() noexcept;

    void BeginFrame_Inactive() noexcept;
    void BeginFrame_Prewave() noexcept;
    void BeginFrame_Active() noexcept;
    void BeginFrame_Postwave() noexcept;

    void Update_Inactive(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Update_Prewave (TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Update_Active  (TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Update_Postwave(TimeUtils::FPSeconds deltaSeconds) noexcept;

    void Render_Inactive() const noexcept;
    void Render_Prewave() const noexcept;
    void Render_Active() const noexcept;
    void Render_Postwave() const noexcept;
    
    void DebugRender_Inactive() const noexcept;
    void DebugRender_Prewave() const noexcept;
    void DebugRender_Active() const noexcept;
    void DebugRender_Postwave() const noexcept;

    void EndFrame_Inactive() noexcept;
    void EndFrame_Prewave() noexcept;
    void EndFrame_Active() noexcept;
    void EndFrame_Postwave() noexcept;

    void UpdateMissiles(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void UpdateBomber(TimeUtils::FPSeconds deltaSeconds) noexcept;

    MissileManager m_missiles{};
    std::unique_ptr<Bomber> m_bomber{};
    std::unique_ptr<Satellite> m_satellite{};
    Stopwatch m_missileSpawnRate{};
    Stopwatch m_flierSpawnRate{};
    Stopwatch m_preWaveTimer{5.0f};
    Stopwatch m_postWaveIncrementRate{0.33f};
    Stopwatch m_postWaveTimer{5.0f};
    std::size_t m_waveId{ 0 };
    std::size_t m_postWaveCityCount{6};
    int m_missileCount{};
    State m_currentState{State::Inactive};
    State m_nextState{State::Inactive};
    bool m_isActive{false};
};
