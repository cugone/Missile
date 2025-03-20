#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Game/EnemyWaveState.hpp"

class EnemyWaveStateActive : public EnemyWaveState {
public:
    EnemyWaveStateActive() = default;
    EnemyWaveStateActive(const EnemyWaveStateActive& other) = default;
    EnemyWaveStateActive(EnemyWaveStateActive&& other) = default;
    EnemyWaveStateActive& operator=(const EnemyWaveStateActive& other) = default;
    EnemyWaveStateActive& operator=(EnemyWaveStateActive&& other) = default;
    virtual ~EnemyWaveStateActive() = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

protected:
private:
    
};