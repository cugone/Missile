#pragma once

#include "Engine/Core/OrthographicCameraController.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include "Game/GameState.hpp"

#include "Game/EnemyWave.hpp"
#include "Game/MissileBase.hpp"
#include "Game/MissileManager.hpp"
#include "Game/ExplosionManager.hpp"
#include "Game/CityManager.hpp"

#include <array>

class GameStateMain : public GameState {
public:
    virtual ~GameStateMain() = default;

    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;
    void OnEnter() noexcept override;
    void OnExit() noexcept override;

    const std::array<MissileManager::Target, 9> GetValidTargets() const noexcept;
    AABB2 GetWorldBounds() const noexcept;
    bool HasMissilesRemaining() const noexcept;
    void ResetMissileCount() noexcept;

    void DecrementTotalMissiles() noexcept;
    int GetTotalMissiles() const noexcept;

    MissileManager& GetMissileManager() noexcept;
    const ExplosionManager& GetExplosionManager() const noexcept;
    ExplosionManager& GetExplosionManager() noexcept;

    const CityManager& GetCityManager() const noexcept;
    CityManager& GetCityManager() noexcept;

    void CreateExplosionAt(Vector2 position, Faction faction) noexcept;
    std::size_t GetWaveId() const noexcept;

    Rgba GetGroundColor() const noexcept;
    Rgba GetPlayerColor() const noexcept;

    const OrthographicCameraController& GetCameraController() const noexcept;
    OrthographicCameraController& GetCameraController() noexcept;

protected:
private:

    void HandleDebugInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleDebugMouseInput(TimeUtils::FPSeconds deltaSeconds);

    void HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleKeyboardInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleControllerInput(TimeUtils::FPSeconds deltaSeconds);
    void HandleMouseInput(TimeUtils::FPSeconds deltaSeconds);

    void CalculateCrosshairLocation() noexcept;
    void ClampCrosshairToRadar() noexcept;
    const bool IsCrosshairClampedToRadar() const noexcept;

    Vector2 CalculatePlayerMissileTarget() noexcept;
    Vector2 CalcCrosshairPositionFromRawMousePosition() noexcept;

    Vector2 BaseLocationLeft() const noexcept;
    Vector2 BaseLocationCenter() const noexcept;
    Vector2 BaseLocationRight() const noexcept;
    Vector2 CityLocation(std::size_t index) const noexcept;

    void HandleMissileExplosionCollisions(MissileManager& missileManager) noexcept;
    void HandleBomberExplosionCollision() noexcept;
    void HandleSatelliteExplosionCollision() noexcept;
    void HandleMissileGroundCollisions(MissileManager& missileManager) noexcept;
    void HandleCityExplosionCollisions() noexcept;
    void HandleBaseExplosionCollisions() noexcept;

    void UpdateHighScore() const noexcept;

    void RenderGround() const noexcept;
    void RenderObjects() const noexcept;
    void RenderCrosshair() const noexcept;
    void RenderCrosshairAt(Vector2 pos) const noexcept;
    void RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept;
    void RenderRadarLine() const noexcept;

    OrthographicCameraController m_cameraController{};
    mutable OrthographicCameraController m_ui_camera{};
    EnemyWave m_waves{};
    MissileBase m_missileBaseLeft{};
    MissileBase m_missileBaseCenter{};
    MissileBase m_missileBaseRight{};
    ExplosionManager m_explosionManager{};
    CityManager m_cityManager{};
    AABB2 m_world_bounds{ AABB2::Zero_to_One };
    AABB2 m_ground{ Vector2::Y_Axis * 450.0f, 800.0f, 20.0f };
    Vector2 m_mouse_pos{};
    Vector2 m_mouse_world_pos{};
    Vector2 m_mouse_delta{};

};
