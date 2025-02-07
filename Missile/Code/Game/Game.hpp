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

#include <array>
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
    int bonus_cities{ 0u };
    int current_level{1u};
    int score_multiplier{ 1u };
};

class Game : public GameBase {
public:
    Game() = default;
    Game(const Game& other) = default;
    Game(Game&& other) = default;
    Game& operator=(const Game& other) = default;
    Game& operator=(Game&& other) = default;
    ~Game() = default;

    void Initialize() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

    const GameSettings* GetSettings() const noexcept override;
    GameSettings* GetSettings() noexcept override;

    void CreateExplosionAt(Vector2 position, Faction faction) noexcept;
    Vector2 CalculatePlayerMissileTarget() noexcept;
    Vector2 BaseLocationLeft() const noexcept;
    Vector2 BaseLocationCenter() const noexcept;
    Vector2 BaseLocationRight() const noexcept;
    Vector2 CityLocation(std::size_t index) const noexcept;
    const std::array<MissileManager::Target, 9> GetValidTargets() const noexcept;
    void RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept;

    MissileManager& GetMissileManager() noexcept;

    Player* GetPlayerData() noexcept;

    AABB2 GetWorldBounds() const noexcept;

    std::size_t GetWaveId() const noexcept;

protected:
private:

    void LoadOrCreateConfigFile() noexcept;
    void CalculateCrosshairLocation() noexcept;
    Vector2 CalcCrosshairPositionFromRawMousePosition() noexcept;
    void ClampCrosshairToRadar() noexcept;
    const bool IsCrosshairClampedToRadar() const noexcept;

    void RenderObjects() const noexcept;
    void Debug_RenderObjects() const noexcept;
    void RenderCrosshair() const noexcept;
    void RenderCrosshairAt(Vector2 pos) const noexcept;
    void RenderGround() const noexcept;
    void RenderRadarLine() const noexcept;
    void RenderHighscoreAndWave() const noexcept;

    void HandleMissileExplosionCollisions(MissileManager& missileManager) noexcept;
    void HandleBomberExplosionCollision() noexcept;
    void HandleSatelliteExplosionCollision() noexcept;
    void HandleMissileGroundCollisions(MissileManager& missileManager) noexcept;

    void HandleDebugInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugMouseInput(TimeUtils::FPSeconds deltaSeconds);

    void HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleControllerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleMouseInput(TimeUtils::FPSeconds deltaSeconds);

    void UpdateHighScore() noexcept;

    mutable Camera2D m_ui_camera2D{};
    OrthographicCameraController m_cameraController{};
    Vector2 m_mouse_pos{};
    Vector2 m_mouse_world_pos{};
    Vector2 m_mouse_delta{};
    AABB2 m_world_bounds{ AABB2::Zero_to_One };
    AABB2 m_ground{ Vector2::Y_Axis * 450.0f, 800.0f, 20.0f };
    MissileBase m_missileBaseLeft{};
    MissileBase m_missileBaseCenter{};
    MissileBase m_missileBaseRight{};
    EnemyWave m_waves{};
    CityManager m_cityManager{};
    ExplosionManager m_explosionManager{};
    MySettings m_mySettings{};
    Player m_playerData{};
    int m_currentHighScore{GameConstants::default_highscore};
    int m_bonusCities{};
    bool m_debug_render{false};
};

