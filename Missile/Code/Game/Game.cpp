#include "Game/Game.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
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

void MySettings::SaveToConfig(Config& config) noexcept {
    GameSettings::SaveToConfig(config);
    config.SetValue("uiScale", m_UiScale);
}

void MySettings::SetToDefault() noexcept {
    GameSettings::SetToDefault();
    m_UiScale = m_defaultUiScale;
}

float MySettings::GetUiScale() const noexcept {
    return m_UiScale;
}

void MySettings::SetUiScale(float newScale) noexcept {
    m_UiScale = newScale;
}

float MySettings::DefaultUiScale() const noexcept {
    return m_defaultUiScale;
}

void Game::Initialize() noexcept {
    (void)g_theConfig->AppendFromFile(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameConfig) / "game.config");
    if(g_theConfig->HasKey("uiScale")) {
        float value = m_mySettings.GetUiScale();
        g_theConfig->GetValueOr("uiScale", value, m_mySettings.DefaultUiScale());
        m_mySettings.SetUiScale(value);
    } else {
        g_theConfig->SetValue("uiScale", 1.0f);
    }
    if(!g_theConfig->SaveToFile(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameConfig) / "game.config")) {
        g_theFileLogger->LogWarnLine("Could not save game config.");
    }
    g_theRenderer->SetVSync(true);
    g_theRenderer->RegisterMaterialsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameMaterials));
    g_theRenderer->RegisterFontsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameFonts));

    m_cameraController = OrthographicCameraController();
    m_cameraController.SetPosition(Vector2::Zero);
    m_cameraController.SetZoomLevelRange(Vector2{8.0f, 450.0f});
    m_cameraController.SetZoomLevel(450.0f);
    g_theInputSystem->SetCursorToWindowCenter();
    m_mouse_delta = Vector2::Zero;
    m_mouse_pos = g_theInputSystem->GetMouseCoords();
    g_theInputSystem->HideMouseCursor();
}

void Game::BeginFrame() noexcept {
    m_explosionManager.BeginFrame();
    m_missileManager.BeginFrame();
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    g_theRenderer->UpdateGameTime(deltaSeconds);
    HandleDebugInput(deltaSeconds);
    HandlePlayerInput(deltaSeconds);

    m_ui_camera2D.Update(deltaSeconds);
    m_cameraController.Update(deltaSeconds);

    CalculateCrosshairLocation();
    m_missileManager.LaunchMissile(BaseLocation(), MissileManager::Target{ CalculatePlayerMissileTarget() }, TimeUtils::FPSeconds{1.0f});
    m_missileManager.Update(deltaSeconds);
    m_explosionManager.Update(deltaSeconds);
}

Vector2 Game::CalculatePlayerMissileTarget() noexcept {
    return CalcCrosshairPositionFromRawMousePosition();
}

Vector2 Game::BaseLocation() const noexcept {
    return Vector2::Y_Axis * m_cameraController.CalcViewBounds().maxs.y;
}

Vector2 Game::CalcCrosshairPositionFromRawMousePosition() noexcept {
    return g_theRenderer->ConvertScreenToWorldCoords(m_cameraController.GetCamera(), m_mouse_pos);
}

void Game::ClampCrosshairToView() noexcept {
    AABB2 view = m_cameraController.CalcViewBounds();
    m_mouse_world_pos = MathUtils::CalcClosestPoint(m_mouse_world_pos, view);
    m_mouse_pos = g_theRenderer->ConvertWorldToScreenCoords(m_cameraController.GetCamera(), m_mouse_world_pos);
}

void Game::CalculateCrosshairLocation() noexcept {
    m_mouse_world_pos = CalcCrosshairPositionFromRawMousePosition();
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
        m_mouse_delta = g_theInputSystem->GetMouseDeltaFromWindowCenter();
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
        m_debug_render = !m_debug_render;
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
}

void Game::HandleDebugMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
}

void Game::CreateExplosionAt(Vector2 position) noexcept {
    m_explosionManager.CreateExplosionAt(position, 40.0f, TimeUtils::FPSeconds{3.0f});
}

void Game::Render() const noexcept {

    g_theRenderer->BeginRenderToBackbuffer();


    //3D World View


    //2D World / HUD View
    {

        const auto ui_view_height = static_cast<float>(GetSettings()->GetWindowHeight());
        const auto ui_view_width = ui_view_height * m_ui_camera2D.GetAspectRatio();
        const auto ui_view_extents = Vector2{ui_view_width, ui_view_height};
        const auto ui_view_half_extents = ui_view_extents * 0.5f;
        const auto ui_cam_pos = Vector2::Zero;
        g_theRenderer->BeginHUDRender(m_ui_camera2D, ui_cam_pos, ui_view_height);

        RenderObjects();
        RenderGround();
        RenderBase();
        RenderCrosshairAt(m_mouse_world_pos);

    }
}

void Game::RenderObjects() const noexcept {
    m_missileManager.Render();
    m_explosionManager.Render();
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

    //g_theRenderer->DrawFilledPolygon2D(p, Rgba::Red);
}

void Game::RenderCrosshair() const noexcept {
    RenderCrosshairAt(m_mouse_pos);
}

void Game::RenderCrosshairAt(Vector2 pos) const noexcept {
    RenderCrosshairAt(pos, Rgba::White);
}

void Game::RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept {
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("crosshair"));
    const auto&& [x, y, _] = g_theRenderer->GetMaterial("crosshair")->GetTexture(Material::TextureID::Diffuse)->GetDimensions().GetXYZ();
    const auto dims = IntVector2{x, y};
    const auto ui_scale = [this]() {
        if(const auto& settings = dynamic_cast<const MySettings*>(this->GetSettings()); settings != nullptr) {
            return settings->GetUiScale();
        } else {
            return 1.0f;
        }
    }();
    {
        const auto scale = ui_scale * Vector2{ dims };
        const auto S = Matrix4::CreateScaleMatrix(scale);
        const auto R = Matrix4::I;
        const auto T = Matrix4::CreateTranslationMatrix(pos);
        const auto M = Matrix4::MakeSRT(S, R, T);
        g_theRenderer->DrawQuad2D(M, color);
    }
}

void Game::EndFrame() noexcept {
    m_mouse_pos += m_mouse_delta;
    g_theInputSystem->SetCursorToWindowCenter();
    m_mouse_delta = Vector2::Zero;
    m_missileManager.EndFrame();
    m_explosionManager.EndFrame();
}

const GameSettings* Game::GetSettings() const noexcept {
    return &m_mySettings;
}

GameSettings* Game::GetSettings() noexcept {
    return &m_mySettings;
}
