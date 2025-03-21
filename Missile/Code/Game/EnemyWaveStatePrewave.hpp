#pragma once

#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Game/EnemyWaveState.hpp"

class EnemyWave;

class EnemyWaveStatePrewave : public EnemyWaveState {
public:
    explicit EnemyWaveStatePrewave(EnemyWave* context) noexcept;
    EnemyWaveStatePrewave(const EnemyWaveStatePrewave& other) noexcept = default;
    EnemyWaveStatePrewave(EnemyWaveStatePrewave&& other) noexcept = default;
    EnemyWaveStatePrewave& operator=(const EnemyWaveStatePrewave& other) noexcept = default;
    EnemyWaveStatePrewave& operator=(EnemyWaveStatePrewave&& other) noexcept = default;
    virtual ~EnemyWaveStatePrewave() noexcept = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void DebugRender() const noexcept override;
    void EndFrame() noexcept override;

protected:
private:
    void ClayPrewave() noexcept;

    void RenderScoreElement() const noexcept;
    void RenderScoreMultiplierElement() const noexcept;

    Stopwatch m_preWaveTimer{ 5.0f };
    EnemyWave* m_context{ nullptr };
};
