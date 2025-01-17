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
    Vector2 BaseLocation() const noexcept;
    void RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept;

protected:
private:

    void LoadOrCreateConfigFile() noexcept;
    void CalculateCrosshairLocation() noexcept;
    Vector2 CalcCrosshairPositionFromRawMousePosition() noexcept;
    void ClampCrosshairToRadar() noexcept;

    void RenderObjects() const noexcept;
    void RenderCrosshair() const noexcept;
    void RenderCrosshairAt(Vector2 pos) const noexcept;
    void RenderGround() const noexcept;

    void HandleMissileExplosionCollisions(MissileManager& missileManager) noexcept;
    void HandleMissileGroundCollisions(MissileManager& missileManager) noexcept;

    void HandleDebugInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugMouseInput(TimeUtils::FPSeconds deltaSeconds);

    void HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleControllerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleMouseInput(TimeUtils::FPSeconds deltaSeconds);

    mutable Camera2D m_ui_camera2D{};
    OrthographicCameraController m_cameraController{};
    Vector2 m_mouse_pos{};
    Vector2 m_mouse_world_pos{};
    Vector2 m_mouse_delta{};
    AABB2 m_ground{ Vector2::Y_Axis * 450.0f, 800.0f, 20.0f };
    MissileBase m_missileBaseLeft{};
    MissileBase m_missileBaseCenter{};
    MissileBase m_missileBaseRight{};
    EnemyWave m_waves{};
    ExplosionManager m_explosionManager{};
    MySettings m_mySettings{};
    bool m_debug_render{false};
};

