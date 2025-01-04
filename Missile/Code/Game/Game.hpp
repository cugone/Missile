#pragma once

#include "Engine/Game/GameBase.hpp"

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/OrthographicCameraController.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include "Game/Missile.hpp"
#include "Game/ExplosionManager.hpp"

#include <vector>

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

    const GameSettings& GetSettings() const noexcept override;
    GameSettings& GetSettings() noexcept override;

    void CreateExplosionAt(Vector2 position) noexcept;
    Vector2 CalculatePlayerMissileTarget() noexcept;
    Vector2 BaseLocation() const noexcept;
    void RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept;

protected:
private:


    void CalculateCrosshairLocation() noexcept;
    Vector2 CalcCrosshairPositionFromRawMousePosition() noexcept;
    void ClampCrosshairToView() noexcept;

    void RenderObjects() const noexcept;
    void RenderCrosshair() const noexcept;
    void RenderCrosshairAt(Vector2 pos) const noexcept;
    void RenderGround() const noexcept;
    void RenderBase() const noexcept;

    void HandleDebugInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugMouseInput(TimeUtils::FPSeconds deltaSeconds);

    void HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleControllerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleMouseInput(TimeUtils::FPSeconds deltaSeconds);

    mutable Camera2D _ui_camera2D{};
    OrthographicCameraController _cameraController{};
    Vector2 _mouse_pos{};
    Vector2 _mouse_world_pos{};
    Vector2 _mouse_delta{};
    std::vector<Missile> _missiles{};
    ExplosionManager _explosionManager{};
    mutable Mesh::Builder m_builder{};
    bool _missile_fired{false};
    bool _debug_render{false};
};

