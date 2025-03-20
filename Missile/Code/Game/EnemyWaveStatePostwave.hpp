#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Game/EnemyWaveState.hpp"

class EnemyWaveStatePostwave : public EnemyWaveState {
public:
    EnemyWaveStatePostwave() = default;
    EnemyWaveStatePostwave(const EnemyWaveStatePostwave& other) = default;
    EnemyWaveStatePostwave(EnemyWaveStatePostwave&& other) = default;
    EnemyWaveStatePostwave& operator=(const EnemyWaveStatePostwave& other) = default;
    EnemyWaveStatePostwave& operator=(EnemyWaveStatePostwave&& other) = default;
    virtual ~EnemyWaveStatePostwave() = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

protected:
private:
    
};