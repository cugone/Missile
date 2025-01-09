#pragma once

#include "Engine/Game/GameBase.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/OrthographicCameraController.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include "Game/Missile.hpp"
#include "Game/MissileManager.hpp"
#include "Game/ExplosionManager.hpp"

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

    void CreateExplosionAt(Vector2 position) noexcept;
    Vector2 CalculatePlayerMissileTarget() noexcept;
    Vector2 BaseLocation() const noexcept;
    void RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept;

protected:
private:

    void LoadOrCreateConfigFile() noexcept;
    void CalculateCrosshairLocation() noexcept;
    Vector2 CalcCrosshairPositionFromRawMousePosition() noexcept;
    void ClampCrosshairToView() noexcept;

    void RenderObjects() const noexcept;
    void RenderCrosshair() const noexcept;
    void RenderCrosshairAt(Vector2 pos) const noexcept;
    void RenderGround() const noexcept;
    void RenderBase() const noexcept;

    void HandleMissileExplosionCollisions() noexcept;

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
    MissileManager m_missileManager{};
    ExplosionManager m_explosionManager{};
    MySettings m_mySettings{};
    bool m_debug_render{false};
};

