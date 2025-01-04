#include "Game/Game.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Utilities.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Polygon2.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IAudioService.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <algorithm>

void Game::Initialize() noexcept {
    g_theRenderer->SetVSync(true);
    g_theRenderer->RegisterMaterialsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameMaterials));
    g_theRenderer->RegisterFontsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameFonts));

    _cameraController = OrthographicCameraController();
    _cameraController.SetPosition(Vector2::Zero);
    _cameraController.SetZoomLevelRange(Vector2{8.0f, 450.0f});
    _cameraController.SetZoomLevel(450.0f);
    g_theInputSystem->SetCursorToWindowCenter();
    _mouse_delta = Vector2::Zero;
    _mouse_pos = g_theInputSystem->GetMouseCoords();
    g_theInputSystem->HideMouseCursor();
}

void Game::BeginFrame() noexcept {
    _explosionManager.BeginFrame();
    m_missileManager.BeginFrame();
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    g_theRenderer->UpdateGameTime(deltaSeconds);
    HandleDebugInput(deltaSeconds);
    HandlePlayerInput(deltaSeconds);

    _ui_camera2D.Update(deltaSeconds);
    _cameraController.Update(deltaSeconds);

    CalculateCrosshairLocation();
    m_missileManager.LaunchMissile(BaseLocation(), MissileManager::Target{ CalculatePlayerMissileTarget() }, TimeUtils::FPSeconds{1.0f});
    m_missileManager.Update(deltaSeconds);
    _explosionManager.Update(deltaSeconds);
}

Vector2 Game::CalculatePlayerMissileTarget() noexcept {
    return CalcCrosshairPositionFromRawMousePosition();
}

Vector2 Game::BaseLocation() const noexcept {
    return Vector2::Y_Axis * _cameraController.CalcViewBounds().maxs.y;
}

Vector2 Game::CalcCrosshairPositionFromRawMousePosition() noexcept {
    return g_theRenderer->ConvertScreenToWorldCoords(_cameraController.GetCamera(), _mouse_pos);
}

void Game::ClampCrosshairToView() noexcept {
    AABB2 view = _cameraController.CalcViewBounds();
    _mouse_world_pos = MathUtils::CalcClosestPoint(_mouse_world_pos, view);
    _mouse_pos = g_theRenderer->ConvertWorldToScreenCoords(_cameraController.GetCamera(), _mouse_world_pos);
}

void Game::CalculateCrosshairLocation() noexcept {
    _mouse_world_pos = CalcCrosshairPositionFromRawMousePosition();
    ClampCrosshairToView();
}

void Game::HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds) {
    HandleKeyboardInput(deltaSeconds);
    HandleControllerInput(deltaSeconds);
    HandleMouseInput(deltaSeconds);
}

void Game::HandleKeyboardInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        auto* app = ServiceLocator::get<IAppService>();
        app->SetIsQuitting(true);
        return;
    }
}

void Game::HandleControllerInput(TimeUtils::FPSeconds /*deltaSeconds*/) {

}

void Game::HandleMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theInputSystem->WasMouseMoved()) {
        _mouse_delta = g_theInputSystem->GetMouseDeltaFromWindowCenter();
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::LButton)) {
        m_missileManager.FireMissile();
    }
}

void Game::HandleDebugInput(TimeUtils::FPSeconds deltaSeconds) {
    HandleDebugKeyboardInput(deltaSeconds);
    HandleDebugMouseInput(deltaSeconds);
}

void Game::HandleDebugKeyboardInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theUISystem->WantsInputKeyboardCapture()) {
        return;
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        _debug_render = !_debug_render;
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::M)) {
        Utils::FlipFlop([]() {g_theAudioSystem->SuspendAudio(); }, []() {g_theAudioSystem->ResumeAudio(); });
    }
}

void Game::HandleDebugMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
}

void Game::CreateExplosionAt(Vector2 position) noexcept {
    _explosionManager.CreateExplosionAt(position, 40.0f, TimeUtils::FPSeconds{3.0f});
}

void Game::Render() const noexcept {

    g_theRenderer->BeginRenderToBackbuffer();


    //3D World View


    //2D World / HUD View
    {

        const auto ui_view_height = static_cast<float>(GetSettings().GetWindowHeight());
        const auto ui_view_width = ui_view_height * _ui_camera2D.GetAspectRatio();
        const auto ui_view_extents = Vector2{ui_view_width, ui_view_height};
        const auto ui_view_half_extents = ui_view_extents * 0.5f;
        const auto ui_cam_pos = Vector2::Zero;
        g_theRenderer->BeginHUDRender(_ui_camera2D, ui_cam_pos, ui_view_height);

        RenderObjects();
        RenderGround();
        RenderBase();
        RenderCrosshairAt(_mouse_world_pos);

        const auto t = std::format("Mouse: {}", _mouse_world_pos);
        g_theRenderer->SetModelMatrix();
        g_theRenderer->DrawTextLine(g_theRenderer->GetFont("System32"), t);

    }
}

void Game::RenderObjects() const noexcept {
    m_missileManager.Render();
    _explosionManager.Render();
}

void Game::RenderGround() const noexcept {
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    const auto S = Matrix4::CreateScaleMatrix(Vector2::One * Vector2{ 1600.0f, 40.0f });
    const auto R = Matrix4::I;
    const auto T = Matrix4::CreateTranslationMatrix(Vector2::Y_Axis * 450.0f);
    const auto M = Matrix4::MakeSRT(S, R, T);

    g_theRenderer->DrawQuad2D(M, Rgba::Red);
}

void Game::RenderBase() const noexcept {
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    const auto S = Matrix4::CreateScaleMatrix(320.0f);
    const auto R = Matrix4::Create2DRotationDegreesMatrix(135.0f);
    const auto p = Polygon2(6);
    const auto T = Matrix4::CreateTranslationMatrix(BaseLocation());
    const auto M = Matrix4::MakeSRT(S, R, T);
    g_theRenderer->SetModelMatrix(M);

    g_theRenderer->DrawFilledPolygon2D(p, Rgba::Red);
}

void Game::RenderCrosshair() const noexcept {
    RenderCrosshairAt(_mouse_pos);
}

void Game::RenderCrosshairAt(Vector2 pos) const noexcept {
    RenderCrosshairAt(pos, Rgba::White);
}

void Game::RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept {
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("crosshair"));
    const auto scale = Vector2::One * 10.0f;
    {
        const auto S = Matrix4::CreateScaleMatrix(scale);
        const auto R = Matrix4::I;
        const auto T = Matrix4::CreateTranslationMatrix(pos);
        const auto M = Matrix4::MakeSRT(S, R, T);
        g_theRenderer->DrawQuad2D(M, color);
    }
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    g_theRenderer->SetModelMatrix();
    g_theRenderer->DrawFilledCircle2D(Disc2{pos, 40.0f}, color);
}

void Game::EndFrame() noexcept {
    _mouse_pos += _mouse_delta;
    g_theInputSystem->SetCursorToWindowCenter();
    _mouse_delta = Vector2::Zero;
    m_missileManager.EndFrame();
    _explosionManager.EndFrame();
}

const GameSettings& Game::GetSettings() const noexcept {
    return GameBase::GetSettings();
}

GameSettings& Game::GetSettings() noexcept {
    return GameBase::GetSettings();
}
