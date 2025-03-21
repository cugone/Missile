#pragma once

#include "Engine/Game/GameBase.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/OrthographicCameraController.hpp"

#include "Engine/Math/AABB2.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Missile.hpp"
#include "Game/MissileBase.hpp"
#include "Game/MissileManager.hpp"
#include "Game/ExplosionManager.hpp"
#include "Game/EnemyWave.hpp"
#include "Game/City.hpp"
#include "Game/CityManager.hpp"

#include "Game/GameState.hpp"
#include "Game/GameStateTitle.hpp"
#include "Game/GameStateMain.hpp"
#include "Game/GameStateGameOver.hpp"

#include <array>
#include <cstdint>
#include <vector>

class MySettings : public GameSettings {
public:
    void SaveToConfig(Config& config) noexcept override;
    void SetToDefault() noexcept override;

    virtual float GetUiScale() const noexcept;
    virtual void SetUiScale(float newScale) noexcept;
    virtual float DefaultUiScale() const noexcept;

protected:
    float m_UiScale{1.0f};
    float m_defaultUiScale{1.0f};
};

struct Player {
    int score{ 0ull };
    int scoreRemainingForBonusCity{GameConstants::default_bonus_city_score};
};

class Game : public GameBase {
public:
    Game() = default;
    Game(const Game& other) = default;
    Game(Game&& other) = default;
    Game& operator=(const Game& other) = default;
    Game& operator=(Game&& other) = default;
    ~Game() = default;

    void ChangeState(std::unique_ptr<GameState> newState) noexcept;

    void Initialize() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

    const GameSettings* GetSettings() const noexcept override;
    GameSettings* GetSettings() noexcept override;

    const Player* GetPlayerData() const noexcept;
    Player* GetPlayerData() noexcept;
    int GetPlayerScore() const noexcept;
    void AdjustPlayerScore(int scoreToAdd) noexcept;

    int GetHighScore() const noexcept;
    void UpdateHighScore() noexcept;

    GameState* const GetCurrentState() const noexcept;

protected:
private:

    void LoadOrCreateConfigFile() noexcept;

    int m_currentHighScore{ GameConstants::default_highscore };
    MySettings m_mySettings{};
    std::unique_ptr<GameState> m_currentState{std::make_unique<GameStateTitle>()};
    std::unique_ptr<GameState> m_nextState{};
    Player m_playerData{};
};
