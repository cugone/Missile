#pragma once

#include "Game/MissileManager.hpp"

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

    void IncrementWave() noexcept;
    constexpr const int GetScoreMultiplier() const noexcept;
protected:
private:
    MissileManager m_missiles{};
    std::size_t m_waveId{ 0 };

};
