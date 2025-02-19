#include "Game/Game.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Utilities.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Polygon2.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IAudioService.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/GameConfig.hpp"

#include <algorithm>
#include <format>
#include <utility>

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

void Game::LoadOrCreateConfigFile() noexcept {
    if (!g_theConfig->AppendFromFile(GameConstants::game_config_path)) {
        if (g_theConfig->HasKey("uiScale")) {
            float value = m_mySettings.GetUiScale();
            g_theConfig->GetValueOr("uiScale", value, m_mySettings.DefaultUiScale());
            m_mySettings.SetUiScale(value);
        } else {
            m_mySettings.SaveToConfig(*g_theConfig);
        }
        if (!g_theConfig->SaveToFile(GameConstants::game_config_path)) {
            g_theFileLogger->LogWarnLine("Could not save game config.");
        }
    }
}

void Game::Initialize() noexcept {
    LoadOrCreateConfigFile();
    g_theRenderer->SetVSync(true);
    g_theRenderer->RegisterMaterialsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameMaterials));
    g_theRenderer->RegisterFontsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameFonts));

    auto dims = Vector2{ g_theRenderer->GetOutput()->GetDimensions() };
    m_world_bounds.ScalePadding(dims.x, dims.y);
    m_world_bounds.Translate(-m_world_bounds.CalcCenter());

    m_cameraController = OrthographicCameraController{};
    m_cameraController.SetPosition(m_world_bounds.CalcCenter());
    m_cameraController.SetZoomLevelRange(Vector2{8.0f, 450.0f});
    m_cameraController.SetZoomLevel(450.0f);
    
    g_theInputSystem->SetCursorToWindowCenter();
    m_mouse_delta = Vector2::Zero;
    m_mouse_pos = g_theInputSystem->GetMouseCoords();

    g_theInputSystem->HideMouseCursor();

    const auto groundplane = m_ground.CalcCenter().y - m_ground.CalcDimensions().y;
    const auto basePosition = Vector2::Y_Axis * groundplane;

    m_missileBaseLeft.SetPosition(basePosition - Vector2::X_Axis * 700.0f);
    m_missileBaseLeft.SetTimeToTarget(TimeUtils::FPSeconds{1.0f});

    m_missileBaseCenter.SetPosition(basePosition);
    m_missileBaseCenter.SetTimeToTarget(TimeUtils::Frames{8});

    m_missileBaseRight.SetPosition(basePosition + Vector2::X_Axis * 700.0f);
    m_missileBaseRight.SetTimeToTarget(TimeUtils::FPSeconds{1.0f});

    auto left_center_displacement = m_missileBaseCenter.GetMissileLauncherPosition() - m_missileBaseLeft.GetMissileLauncherPosition();
    const auto left_len = left_center_displacement.Normalize();
    m_cityManager.GetCity(0).SetPosition(m_missileBaseLeft.GetMissileLauncherPosition() + left_center_displacement * left_len * 0.25f);
    m_cityManager.GetCity(1).SetPosition(m_missileBaseLeft.GetMissileLauncherPosition() + left_center_displacement * left_len * 0.50f);
    m_cityManager.GetCity(2).SetPosition(m_missileBaseLeft.GetMissileLauncherPosition() + left_center_displacement * left_len * 0.75f);

    auto right_center_displacement = m_missileBaseCenter.GetMissileLauncherPosition() - m_missileBaseRight.GetMissileLauncherPosition();
    const auto right_len = right_center_displacement.Normalize();
    m_cityManager.GetCity(3).SetPosition(m_missileBaseRight.GetMissileLauncherPosition() + right_center_displacement * right_len * 0.25f);
    m_cityManager.GetCity(4).SetPosition(m_missileBaseRight.GetMissileLauncherPosition() + right_center_displacement * right_len * 0.50f);
    m_cityManager.GetCity(5).SetPosition(m_missileBaseRight.GetMissileLauncherPosition() + right_center_displacement * right_len * 0.75f);

    m_waves.SetMissileCount(m_waves.GetMissileCount());
    m_waves.SetMissileSpawnRate(TimeUtils::FPSeconds{1.0f});
    m_waves.ActivateWave();

    auto desc = AudioSystem::SoundDesc{};
    desc.loopCount = 6;
    desc.stopWhenFinishedLooping = true;
    g_theAudioSystem->Play(GameConstants::game_audio_klaxon_path, desc);
}

void Game::BeginFrame() noexcept {
    switch (m_state) {
    case State::Title:
        BeginFrame_Title();
        break;
    case State::Main:
        BeginFrame_Main();
        break;
    case State::GameOver:
        BeginFrame_GameOver();
        break;
    default:
        ERROR_AND_DIE("Game State scoped enum has changed.");
    }
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    switch (m_state) {
    case State::Title:
        Update_Title(deltaSeconds);
        break;
    case State::Main:
        Update_Main(deltaSeconds);
        break;
    case State::GameOver:
        Update_GameOver(deltaSeconds);
        break;
    default:
        ERROR_AND_DIE("Game State scoped enum has changed.");
    }
}

void Game::Render() const noexcept {
    switch (m_state) {
    case State::Title:
        Render_Title();
        break;
    case State::Main:
        Render_Main();
        break;
    case State::GameOver:
        Render_GameOver();
        break;
    default:
        ERROR_AND_DIE("Game State scoped enum has changed.");
    }
}

void Game::EndFrame() noexcept {
    switch (m_state) {
    case State::Title:
        EndFrame_Title();
        break;
    case State::Main:
        EndFrame_Main();
        break;
    case State::GameOver:
        EndFrame_GameOver();
        break;
    default:
        ERROR_AND_DIE("Game State scoped enum has changed.");
    }
}

void Game::BeginFrame_Title() noexcept {

}

void Game::BeginFrame_Main() noexcept {
    m_waves.BeginFrame();
    m_missileBaseLeft.BeginFrame();
    m_missileBaseCenter.BeginFrame();
    m_missileBaseRight.BeginFrame();
    m_explosionManager.BeginFrame();
    m_cityManager.BeginFrame();
}

void Game::BeginFrame_GameOver() noexcept {

}

void Game::Update_Title([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {

}

void Game::Update_Main(TimeUtils::FPSeconds deltaSeconds) noexcept {

    g_theRenderer->UpdateGameTime(deltaSeconds);
    HandleDebugInput(deltaSeconds);
    HandlePlayerInput(deltaSeconds);

    m_ui_camera2D.Update(deltaSeconds);
    m_cameraController.Update(deltaSeconds);

    CalculateCrosshairLocation();
    m_waves.Update(deltaSeconds);
    m_missileBaseLeft.Update(deltaSeconds);
    m_missileBaseCenter.Update(deltaSeconds);
    m_missileBaseRight.Update(deltaSeconds);
    m_explosionManager.Update(deltaSeconds);
    HandleMissileExplosionCollisions(m_waves.GetMissileManager());
    HandleBomberExplosionCollision();
    HandleSatelliteExplosionCollision();
    HandleCityExplosionCollisions();
    HandleBaseExplosionCollisions();
    HandleMissileGroundCollisions(m_waves.GetMissileManager());
    m_cityManager.Update(deltaSeconds);
    UpdateHighScore();
}

void Game::Update_GameOver([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {

}

void Game::Render_Title() const noexcept {

}

void Game::Render_Main() const noexcept {

    g_theRenderer->BeginRenderToBackbuffer(m_waves.GetBackgroundColor());


    //3D World View


    //2D World / HUD View
    {

        const auto ui_view_height = static_cast<float>(GetSettings()->GetWindowHeight());
        const auto ui_view_width = ui_view_height * m_ui_camera2D.GetAspectRatio();
        const auto ui_view_extents = Vector2{ ui_view_width, ui_view_height };
        const auto ui_view_half_extents = ui_view_extents * 0.5f;
        const auto ui_cam_pos = Vector2::Zero;
        g_theRenderer->BeginHUDRender(m_ui_camera2D, ui_cam_pos, ui_view_height);

        RenderGround();
        RenderObjects();
        RenderCrosshairAt(m_mouse_world_pos);
        RenderRadarLine();
        RenderHighscoreAndWave();

        if (m_debug_render) {
            Debug_RenderObjects();
        }

    }
}

void Game::Render_GameOver() const noexcept {

}

void Game::EndFrame_Title() noexcept {

}

void Game::EndFrame_Main() noexcept {
    m_mouse_pos += m_mouse_delta;
    g_theInputSystem->SetCursorToWindowCenter();
    m_mouse_delta = Vector2::Zero;
    m_missileBaseLeft.EndFrame();
    m_missileBaseCenter.EndFrame();
    m_missileBaseRight.EndFrame();
    m_cityManager.EndFrame();
    m_explosionManager.EndFrame();
    m_waves.EndFrame();
}

void Game::EndFrame_GameOver() noexcept {

}

void Game::UpdateHighScore() noexcept {
    m_currentHighScore = (std::max)(m_playerData.score, m_currentHighScore);
}

Vector2 Game::CalculatePlayerMissileTarget() noexcept {
    return CalcCrosshairPositionFromRawMousePosition();
}

Vector2 Game::BaseLocationLeft() const noexcept {
    return m_missileBaseLeft.GetMissileLauncherPosition();
}


Vector2 Game::BaseLocationCenter() const noexcept {
    return m_missileBaseCenter.GetMissileLauncherPosition();
}


Vector2 Game::BaseLocationRight() const noexcept {
    return m_missileBaseRight.GetMissileLauncherPosition();
}

Vector2 Game::CityLocation(std::size_t index) const noexcept {
    return m_cityManager.GetCity(index).GetCollisionMesh().CalcCenter();
}

const std::array<MissileManager::Target, 9> Game::GetValidTargets() const noexcept {
    return std::array<MissileManager::Target, 9>{ BaseLocationLeft(), BaseLocationCenter(), BaseLocationRight(), CityLocation(0), CityLocation(1), CityLocation(2), CityLocation(3), CityLocation(4), CityLocation(5) };
}

Vector2 Game::CalcCrosshairPositionFromRawMousePosition() noexcept {
    return g_theRenderer->ConvertScreenToWorldCoords(m_cameraController.GetCamera(), m_mouse_pos);
}

void Game::ClampCrosshairToRadar() noexcept {
    AABB2 cull = m_cameraController.CalcCullBounds();
    cull.maxs.y -= GameConstants::radar_line_distance;
    m_mouse_world_pos = MathUtils::CalcClosestPoint(m_mouse_world_pos, cull);
    m_mouse_pos = g_theRenderer->ConvertWorldToScreenCoords(m_cameraController.GetCamera(), m_mouse_world_pos);
}

void Game::CalculateCrosshairLocation() noexcept {
    m_mouse_world_pos = CalcCrosshairPositionFromRawMousePosition();
    ClampCrosshairToRadar();
}

const bool Game::IsCrosshairClampedToRadar() const noexcept {
    const auto cull = [this]() {
        auto bounds = m_cameraController.CalcCullBounds();
        bounds.maxs.y -= GameConstants::radar_line_distance;
        return bounds;
    }(); //IIIL
    return MathUtils::IsEquivalent(MathUtils::CalcClosestPoint(m_mouse_world_pos, cull).y, cull.maxs.y);
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
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::A)) {
        m_missileBaseLeft.Fire(MissileManager::Target{ CalculatePlayerMissileTarget() });
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::W)) {
        m_missileBaseCenter.Fire(MissileManager::Target{ CalculatePlayerMissileTarget() });
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::D)) {
        m_missileBaseRight.Fire(MissileManager::Target{ CalculatePlayerMissileTarget() });
    }
}

void Game::HandleControllerInput(TimeUtils::FPSeconds /*deltaSeconds*/) {

}

void Game::HandleMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theInputSystem->WasMouseMoved()) {
        m_mouse_delta = g_theInputSystem->GetMouseDeltaFromWindowCenter();
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::LButton)) {
        m_missileBaseLeft.Fire(MissileManager::Target{ CalculatePlayerMissileTarget() });
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::MButton)) {
        m_missileBaseCenter.Fire(MissileManager::Target{ CalculatePlayerMissileTarget() });
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::RButton)) {
        m_missileBaseRight.Fire(MissileManager::Target{ CalculatePlayerMissileTarget() });
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
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::B)) {
        m_waves.SpawnBomber();
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::S)) {
        m_waves.SpawnSatellite();
    }
    if (g_theInputSystem->IsKeyDown(KeyCode::M)) {
        m_waves.SpawnMissile();
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::I)) {
        m_waves.IncrementWave();
    }
}

void Game::HandleDebugMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
}

void Game::CreateExplosionAt(Vector2 position, Faction faction) noexcept {
    m_explosionManager.CreateExplosionAt(ExplosionManager::ExplosionData{ Vector4{position, GameConstants::max_explosion_size, 3.0f}, faction });
}

void Game::RenderHighscoreAndWave() const noexcept {
    const auto highscore_line = [this]()-> std::string {
        if (m_playerData.score < m_currentHighScore) {
            return std::format("{} -> {}\nWave: {}", m_playerData.score, m_currentHighScore, std::size_t{ 1u } + this->GetWaveId());
        } else {
            return std::format("{} <- {}\nWave: {}", m_playerData.score, m_currentHighScore, std::size_t{ 1u } + this->GetWaveId());
        }
        }();

    const auto* font = g_theRenderer->GetFont("System32");
    const auto top = m_cameraController.CalcViewBounds().mins.y;
    const auto font_width = font->CalculateTextWidth(highscore_line);
    const auto S = Matrix4::I;
    const auto R = Matrix4::I;
    const auto T = Matrix4::CreateTranslationMatrix(Vector2{ font_width * -0.5f, top });
    const auto M = Matrix4::MakeSRT(S, R, T);
    g_theRenderer->SetModelMatrix(M);
    g_theRenderer->DrawMultilineText(font, highscore_line);
}

void Game::RenderRadarLine() const noexcept {
    if(IsCrosshairClampedToRadar()) {
        g_theRenderer->SetModelMatrix();
        g_theRenderer->SetMaterial("__2D");
        AABB2 cull = m_cameraController.CalcCullBounds();
        cull.maxs.y -= GameConstants::radar_line_distance;
        const auto t = g_theRenderer->GetGameTime().count();
        const auto alpha = MathUtils::SineWave(t, TimeUtils::FPSeconds{1.0f});
        const auto color = [this, alpha]() {
            if (const auto id = this->GetWaveId() % GameConstants::wave_array_size; id != GameConstants::wave_array_size - 1 && id != GameConstants::wave_array_size - 2) {
                return Rgba{1.0f, 0.0f, 0.0f, alpha};
            } else {
                return Rgba{0.0f, 0.0f, 0.0f, alpha};
            }
        }();
        g_theRenderer->DrawLine2D(Vector2{cull.mins.x, cull.maxs.y}, Vector2{cull.maxs.x, cull.maxs.y}, color);
    }
}

void Game::RenderObjects() const noexcept {
    m_waves.Render();
    m_missileBaseLeft.Render();
    m_missileBaseCenter.Render();
    m_missileBaseRight.Render();
    m_cityManager.Render();
    m_explosionManager.Render();
}

void Game::Debug_RenderObjects() const noexcept {
    m_waves.DebugRender();
    m_missileBaseLeft.DebugRender();
    m_missileBaseCenter.DebugRender();
    m_missileBaseRight.DebugRender();
    m_cityManager.DebugRender();
    m_explosionManager.DebugRender();
}

void Game::RenderGround() const noexcept {
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    const auto S = Matrix4::CreateScaleMatrix(Vector2::One * Vector2{ 1600.0f, 40.0f });
    const auto R = Matrix4::I;
    const auto T = Matrix4::CreateTranslationMatrix(Vector2::Y_Axis * 450.0f);
    const auto M = Matrix4::MakeSRT(S, R, T);

    g_theRenderer->DrawQuad2D(M, Rgba(GameConstants::wave_ground_color_lookup[GetWaveId() % GameConstants::wave_array_size]));
}

void Game::HandleMissileExplosionCollisions(MissileManager& missileManager) noexcept {
    const auto& missiles = missileManager.GetMissilePositions();
    const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
    for(const auto& e : explosions) {
        for (auto idx = std::size_t{}; idx < missiles.size(); ++idx) {
            const auto& m = missiles[idx];
            if(MathUtils::IsPointInside(e, m)) {
                missileManager.KillMissile(idx);
                m_playerData.score += GameConstants::enemy_missile_value * m_waves.GetScoreMultiplier();
            }
        }
    }
}

void Game::HandleBomberExplosionCollision() noexcept {
    if(auto* bomber = m_waves.GetBomber(); bomber == nullptr) {
        return;
    } else {
        const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
        for (const auto& e : explosions) {
            if(MathUtils::DoDiscsOverlap(e, bomber->GetCollisionMesh())) {
                bomber->Kill();
                m_playerData.score += GameConstants::enemy_bomber_value * m_waves.GetScoreMultiplier();
            }
        }
    }
}

void Game::HandleSatelliteExplosionCollision() noexcept {
    if(auto* sat = m_waves.GetSatellite(); sat == nullptr) {
        return;
    } else {
        const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
        for (const auto& e : explosions) {
            if(MathUtils::DoDiscsOverlap(e, sat->GetCollisionMesh())) {
                sat->Kill();
                m_playerData.score += GameConstants::enemy_satellite_value * m_waves.GetScoreMultiplier();
            }
        }
    }
}

void Game::HandleMissileGroundCollisions(MissileManager& missileManager) noexcept {
    const auto& missiles = missileManager.GetMissilePositions();
    const auto s = missiles.size();
    for (auto idx = std::size_t{}; idx < s; ++idx) {
        const auto& m = missiles[idx];
        if (MathUtils::IsPointInside(m_ground, m)) {
            missileManager.KillMissile(idx);
        }
    }
}

void Game::HandleCityExplosionCollisions() noexcept {
    const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
    for(int i = 0; i < GameConstants::max_cities; ++i) {
        auto& city = m_cityManager.GetCity(i);
        for(const auto& explosion : explosions) {
            if(MathUtils::Contains(explosion, city.GetCollisionMesh())) {
                city.Kill();
            }
        }
    }

}

void Game::HandleBaseExplosionCollisions() noexcept {
    for(const auto& explosion : m_explosionManager.GetExplosionCollisionMeshes()) {
        if(MathUtils::Contains(explosion, m_missileBaseLeft.GetCollisionMesh())) {
            m_missileBaseLeft.Kill();
        }
        if(MathUtils::Contains(explosion, m_missileBaseCenter.GetCollisionMesh())) {
            m_missileBaseCenter.Kill();
        }
        if(MathUtils::Contains(explosion, m_missileBaseRight.GetCollisionMesh())) {
            m_missileBaseRight.Kill();
        }
    }
}

void Game::RenderCrosshair() const noexcept {
    RenderCrosshairAt(m_mouse_pos);
}

void Game::RenderCrosshairAt(Vector2 pos) const noexcept {
    RenderCrosshairAt(pos, Rgba::White);
}

void Game::RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept {
    auto* mat = g_theRenderer->GetMaterial("crosshair");
    g_theRenderer->SetMaterial(mat);
    const auto&& [x, y, _] = mat->GetTexture(Material::TextureID::Diffuse)->GetDimensions().GetXYZ();
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

MissileManager& Game::GetMissileManager() noexcept {
    return m_waves.GetMissileManager();
}

const ExplosionManager& Game::GetExplosionManager() const noexcept {
    return m_explosionManager;
}

ExplosionManager& Game::GetExplosionManager() noexcept {
    return m_explosionManager;
}

Player* Game::GetPlayerData() noexcept {
    return &m_playerData;
}

AABB2 Game::GetWorldBounds() const noexcept {
    return m_world_bounds;
}

std::size_t Game::GetWaveId() const noexcept {
    return m_waves.GetWaveId();
}

bool Game::HasMissilesRemaining() const noexcept {
    return m_missileBaseLeft.HasMissilesRemaining() || m_missileBaseCenter.HasMissilesRemaining() || m_missileBaseRight.HasMissilesRemaining();
}

void Game::ResetMissileCount() noexcept {
    m_missileBaseLeft.ResetMissiles();
    m_missileBaseCenter.ResetMissiles();
    m_missileBaseRight.ResetMissiles();
}

const GameSettings* Game::GetSettings() const noexcept {
    return &m_mySettings;
}

GameSettings* Game::GetSettings() noexcept {
    return &m_mySettings;
}
