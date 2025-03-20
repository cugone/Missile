#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Game/EnemyWaveState.hpp"

class EnemyWaveStatePrewave : public EnemyWaveState {
public:
    EnemyWaveStatePrewave() = default;
    EnemyWaveStatePrewave(const EnemyWaveStatePrewave& other) = default;
    EnemyWaveStatePrewave(EnemyWaveStatePrewave&& other) = default;
    EnemyWaveStatePrewave& operator=(const EnemyWaveStatePrewave& other) = default;
    EnemyWaveStatePrewave& operator=(EnemyWaveStatePrewave&& other) = default;
    virtual ~EnemyWaveStatePrewave() = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

protected:
private:
    
};