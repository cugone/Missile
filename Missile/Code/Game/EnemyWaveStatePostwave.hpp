#pragma once

#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/EnemyWaveState.hpp"

class EnemyWave;

class EnemyWaveStatePostwave : public EnemyWaveState {
public:
    explicit EnemyWaveStatePostwave(EnemyWave* context) noexcept;
    EnemyWaveStatePostwave(const EnemyWaveStatePostwave& other) noexcept = default;
    EnemyWaveStatePostwave(EnemyWaveStatePostwave&& other) noexcept = default;
    EnemyWaveStatePostwave& operator=(const EnemyWaveStatePostwave& other) noexcept = default;
    EnemyWaveStatePostwave& operator=(EnemyWaveStatePostwave&& other) noexcept = default;
    virtual ~EnemyWaveStatePostwave() noexcept = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void DebugRender() const noexcept override;
    void EndFrame() noexcept override;

protected:
private:
    void ClayPostwave() noexcept;

    void RenderScoreElement() const noexcept;
    void RenderScoreMultiplierElement() const noexcept;


    void RenderPostWaveStatsElement() const noexcept;

    void RenderCityImageElements() const noexcept;

    void RenderMissileImageElements() const noexcept;


    EnemyWave* m_context{nullptr};
    mutable Stopwatch m_postWaveIncrementRate{ 0.33f };
    Stopwatch m_postWaveTimer{ 5.0f };
    int m_missilesRemainingPostWave{};
    std::size_t m_citiesRemainingPostWave{};
    bool m_showBonusCityText{ false };
    bool m_grantedCityThisWave{ false };
    bool m_canTransision{false};
    std::array<bool, GameConstants::max_cities> m_alive_cities{ false, false, false, false, false, false };
    std::size_t m_city_idx{0u};
    std::size_t m_remaining_cities{};

};