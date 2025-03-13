#include "Game/GameStateMain.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"
#include "Game/GameState.hpp"
#include "Game/GameStateTitle.hpp"

#include <format>

void GameStateMain::OnEnter() noexcept {

    auto dims = Vector2{ g_theRenderer->GetOutput()->GetDimensions() };
    m_world_bounds.ScalePadding(dims.x, dims.y);
    m_world_bounds.Translate(-m_world_bounds.CalcCenter());

    m_cameraController = OrthographicCameraController{ OrthographicCameraController::Options{.lockInput = true, .lockTranslation = true, .lockZoom = true}};
    m_cameraController.SetPosition(m_world_bounds.CalcCenter());
    m_cameraController.SetZoomLevelRange(Vector2{ 8.0f, 450.0f });
    m_cameraController.SetZoomLevel(450.0f);

    m_ui_camera = m_cameraController;

    g_theInputSystem->SetCursorToWindowCenter();
    m_mouse_delta = Vector2::Zero;
    m_mouse_pos = g_theInputSystem->GetMouseCoords();

    g_theInputSystem->HideMouseCursor();

    const auto groundplane = m_ground.CalcCenter().y - m_ground.CalcDimensions().y;
    const auto basePosition = Vector2::Y_Axis * groundplane;

    m_missileBaseLeft.SetPosition(basePosition - Vector2::X_Axis * 700.0f);
    m_missileBaseLeft.SetTimeToTarget(TimeUtils::FPSeconds{ 1.0f });

    m_missileBaseCenter.SetPosition(basePosition);
    m_missileBaseCenter.SetTimeToTarget(TimeUtils::Frames{ 8 });

    m_missileBaseRight.SetPosition(basePosition + Vector2::X_Axis * 700.0f);
    m_missileBaseRight.SetTimeToTarget(TimeUtils::FPSeconds{ 1.0f });

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
    m_waves.SetMissileSpawnRate(TimeUtils::FPSeconds{ 1.0f });
    m_waves.ChangeState(EnemyWave::State::Prewave);

    auto desc = AudioSystem::SoundDesc{};
    desc.loopCount = 6;
    desc.stopWhenFinishedLooping = true;
    g_theAudioSystem->Play(GameConstants::game_audio_klaxon_path, desc);
}

void GameStateMain::OnExit() noexcept {

}

void GameStateMain::BeginFrame() noexcept {
    m_waves.BeginFrame();
    m_missileBaseLeft.BeginFrame();
    m_missileBaseCenter.BeginFrame();
    m_missileBaseRight.BeginFrame();
    m_explosionManager.BeginFrame();
    m_cityManager.BeginFrame();
}

void GameStateMain::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {

    g_theRenderer->UpdateGameTime(deltaSeconds);
    HandleDebugInput(deltaSeconds);
    HandlePlayerInput(deltaSeconds);

    m_ui_camera.Update(deltaSeconds);
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

void GameStateMain::HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds) {
    HandleKeyboardInput(deltaSeconds);
    HandleControllerInput(deltaSeconds);
    HandleMouseInput(deltaSeconds);
}

void GameStateMain::HandleKeyboardInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        if (auto* g = GetGameAs<Game>(); g != nullptr) {
            //g->ChangeState(std::make_unique<GameStateTitle>());
            if (auto* app = ServiceLocator::get<IAppService>(); app != nullptr) {
                app->SetIsQuitting(true);
            }
        }
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

void GameStateMain::HandleControllerInput(TimeUtils::FPSeconds /*deltaSeconds*/) {

}

void GameStateMain::HandleMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
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

void GameStateMain::CalculateCrosshairLocation() noexcept {
    m_mouse_world_pos = CalcCrosshairPositionFromRawMousePosition();
    ClampCrosshairToRadar();
}

void GameStateMain::ClampCrosshairToRadar() noexcept {
    AABB2 cull = m_cameraController.CalcCullBounds();
    cull.maxs.y -= GameConstants::radar_line_distance;
    m_mouse_world_pos = MathUtils::CalcClosestPoint(m_mouse_world_pos, cull);
    m_mouse_pos = g_theRenderer->ConvertWorldToScreenCoords(m_cameraController.GetCamera(), m_mouse_world_pos);
}

const bool GameStateMain::IsCrosshairClampedToRadar() const noexcept {
    const auto cull = [this]() {
        auto bounds = m_cameraController.CalcCullBounds();
        bounds.maxs.y -= GameConstants::radar_line_distance;
        return bounds;
        }(); //IIIL
    return MathUtils::IsEquivalent(MathUtils::CalcClosestPoint(m_mouse_world_pos, cull).y, cull.maxs.y);
}

Vector2 GameStateMain::CalculatePlayerMissileTarget() noexcept {
    return CalcCrosshairPositionFromRawMousePosition();
}

Vector2 GameStateMain::BaseLocationLeft() const noexcept {
    return m_missileBaseLeft.GetMissileLauncherPosition();
}

Vector2 GameStateMain::BaseLocationCenter() const noexcept {
    return m_missileBaseCenter.GetMissileLauncherPosition();
}

Vector2 GameStateMain::BaseLocationRight() const noexcept {
    return m_missileBaseRight.GetMissileLauncherPosition();
}

Vector2 GameStateMain::CityLocation(std::size_t index) const noexcept {
    return m_cityManager.GetCity(index).GetCollisionMesh().CalcCenter();
}

const std::array<MissileManager::Target, 9> GameStateMain::GetValidTargets() const noexcept {
    return std::array<MissileManager::Target, 9>{ BaseLocationLeft(), BaseLocationCenter(), BaseLocationRight(), CityLocation(0), CityLocation(1), CityLocation(2), CityLocation(3), CityLocation(4), CityLocation(5) };
}

Vector2 GameStateMain::CalcCrosshairPositionFromRawMousePosition() noexcept {
    return m_cameraController.ConvertScreenToWorldCoords(m_mouse_pos);
}

const CityManager& GameStateMain::GetCityManager() const noexcept {
    return m_cityManager;
}

CityManager& GameStateMain::GetCityManager() noexcept {
    return m_cityManager;
}

void GameStateMain::CreateExplosionAt(Vector2 position, Faction faction) noexcept {
    m_explosionManager.CreateExplosionAt(ExplosionManager::ExplosionData{ Vector4{position, GameConstants::max_explosion_size, 3.0f}, faction });
}

MissileManager& GameStateMain::GetMissileManager() noexcept {
    return m_waves.GetMissileManager();
}

const ExplosionManager& GameStateMain::GetExplosionManager() const noexcept {
    return m_explosionManager;
}

ExplosionManager& GameStateMain::GetExplosionManager() noexcept {
    return m_explosionManager;
}

AABB2 GameStateMain::GetWorldBounds() const noexcept {
    return m_world_bounds;
}

bool GameStateMain::HasMissilesRemaining() const noexcept {
    return m_missileBaseLeft.HasMissilesRemaining() || m_missileBaseCenter.HasMissilesRemaining() || m_missileBaseRight.HasMissilesRemaining();
}

void GameStateMain::ResetMissileCount() noexcept {
    m_missileBaseLeft.ResetMissiles();
    m_missileBaseCenter.ResetMissiles();
    m_missileBaseRight.ResetMissiles();
}

void GameStateMain::DecrementTotalMissiles() noexcept {
    if(m_missileBaseLeft.HasMissilesRemaining()) {
        m_missileBaseLeft.DecrementMissiles();
    } else if(m_missileBaseCenter.HasMissilesRemaining()) {
        m_missileBaseCenter.DecrementMissiles();
    } else if(m_missileBaseRight.HasMissilesRemaining()) {
        m_missileBaseRight.DecrementMissiles();
    } else {
        /* DO NOTHING */
    }
}

void GameStateMain::HandleMissileExplosionCollisions(MissileManager& missileManager) noexcept {
    const auto& missiles = missileManager.GetMissilePositions();
    const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
    for (const auto& e : explosions) {
        for (auto idx = std::size_t{}; idx < missiles.size(); ++idx) {
            const auto& m = missiles[idx];
            if (MathUtils::IsPointInside(e, m)) {
                missileManager.KillMissile(idx);
                if (auto* g = GetGameAs<Game>(); g != nullptr) {
                    g->AdjustPlayerScore(GameConstants::enemy_missile_value * m_waves.GetScoreMultiplier());
                }
            }
        }
    }
}

void GameStateMain::HandleBomberExplosionCollision() noexcept {
    if (auto* bomber = m_waves.GetBomber(); bomber == nullptr) {
        return;
    } else {
        const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
        for (const auto& e : explosions) {
            if (MathUtils::DoDiscsOverlap(e, bomber->GetCollisionMesh())) {
                bomber->Kill();
                if (auto* g = GetGameAs<Game>(); g != nullptr) {
                    g->AdjustPlayerScore(GameConstants::enemy_bomber_value * m_waves.GetScoreMultiplier());
                }
            }
        }
    }
}

void GameStateMain::HandleSatelliteExplosionCollision() noexcept {
    if (auto* sat = m_waves.GetSatellite(); sat == nullptr) {
        return;
    } else {
        const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
        for (const auto& e : explosions) {
            if (MathUtils::DoDiscsOverlap(e, sat->GetCollisionMesh())) {
                sat->Kill();
                if (auto* g = GetGameAs<Game>(); g != nullptr) {
                    g->AdjustPlayerScore(GameConstants::enemy_satellite_value * m_waves.GetScoreMultiplier());
                }
            }
        }
    }
}

void GameStateMain::HandleMissileGroundCollisions(MissileManager& missileManager) noexcept {
    const auto& missiles = missileManager.GetMissilePositions();
    const auto s = missiles.size();
    for (auto idx = std::size_t{}; idx < s; ++idx) {
        const auto& m = missiles[idx];
        if (MathUtils::IsPointInside(m_ground, m)) {
            missileManager.KillMissile(idx);
        }
    }
}

void GameStateMain::HandleCityExplosionCollisions() noexcept {
    const auto& explosions = m_explosionManager.GetExplosionCollisionMeshes();
    for (int i = 0; i < GameConstants::max_cities; ++i) {
        auto& city = m_cityManager.GetCity(i);
        for (const auto& explosion : explosions) {
            if (MathUtils::Contains(explosion, city.GetCollisionMesh())) {
                city.Kill();
            }
        }
    }
}

void GameStateMain::HandleBaseExplosionCollisions() noexcept {
    for (const auto& explosion : m_explosionManager.GetExplosionCollisionMeshes()) {
        if (MathUtils::Contains(explosion, m_missileBaseLeft.GetCollisionMesh())) {
            m_missileBaseLeft.Kill();
        }
        if (MathUtils::Contains(explosion, m_missileBaseCenter.GetCollisionMesh())) {
            m_missileBaseCenter.Kill();
        }
        if (MathUtils::Contains(explosion, m_missileBaseRight.GetCollisionMesh())) {
            m_missileBaseRight.Kill();
        }
    }
}

void GameStateMain::UpdateHighScore() const noexcept {
    if (auto* g = GetGameAs<Game>(); g != nullptr) {
        g->UpdateHighScore();
    }
}

void GameStateMain::RenderGround() const noexcept {
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    const auto S = Matrix4::CreateScaleMatrix(Vector2::One * Vector2{ 1600.0f, 40.0f });
    const auto R = Matrix4::I;
    const auto T = Matrix4::CreateTranslationMatrix(Vector2::Y_Axis * 450.0f);
    const auto M = Matrix4::MakeSRT(S, R, T);

    g_theRenderer->DrawQuad2D(M, Rgba(GameConstants::wave_ground_color_lookup[GetWaveId() % GameConstants::wave_array_size]));
}

void GameStateMain::RenderObjects() const noexcept {
    m_waves.Render();
    m_missileBaseLeft.Render();
    m_missileBaseCenter.Render();
    m_missileBaseRight.Render();
    m_cityManager.Render();
    m_explosionManager.Render();
}

void GameStateMain::RenderCrosshair() const noexcept {
    RenderCrosshairAt(m_mouse_pos);
}

void GameStateMain::RenderCrosshairAt(Vector2 pos) const noexcept {
    RenderCrosshairAt(pos, Rgba::White);
}

void GameStateMain::RenderCrosshairAt(Vector2 pos, const Rgba& color) const noexcept {
    auto* mat = g_theRenderer->GetMaterial("crosshair");
    g_theRenderer->SetMaterial(mat);
    const auto&& [x, y, _] = mat->GetTexture(Material::TextureID::Diffuse)->GetDimensions().GetXYZ();
    const auto dims = IntVector2{ x, y };
    const auto ui_scale = []() {
        if (auto* g = GetGameAs<Game>(); g != nullptr) {
            if (const auto& settings = dynamic_cast<const MySettings*>(g->GetSettings()); settings != nullptr) {
                return settings->GetUiScale();
            } else {
                return 1.0f;
            }
        }
        return 1.0f;
    }(); //IIIL

    const auto scale = ui_scale * Vector2{ dims };
    const auto S = Matrix4::CreateScaleMatrix(scale);
    const auto R = Matrix4::I;
    const auto T = Matrix4::CreateTranslationMatrix(pos);
    const auto M = Matrix4::MakeSRT(S, R, T);
    g_theRenderer->DrawQuad2D(M, color);
}

void GameStateMain::RenderRadarLine() const noexcept {
    if (IsCrosshairClampedToRadar()) {
        g_theRenderer->SetModelMatrix();
        g_theRenderer->SetMaterial("__2D");
        AABB2 cull = m_cameraController.CalcCullBounds();
        cull.maxs.y -= GameConstants::radar_line_distance;
        const auto t = g_theRenderer->GetGameTime().count();
        const auto alpha = MathUtils::SineWave(t, TimeUtils::FPSeconds{ 1.0f });
        auto color = Rgba(GameConstants::wave_player_color_lookup[this->GetWaveId() % GameConstants::wave_array_size]);
        color.ScaleAlpha(alpha);
        g_theRenderer->DrawLine2D(Vector2{ cull.mins.x, cull.maxs.y }, Vector2{ cull.maxs.x, cull.maxs.y }, color);
    }
}

std::size_t GameStateMain::GetWaveId() const noexcept {
    return m_waves.GetWaveId();
}

const OrthographicCameraController& GameStateMain::GetCameraController() const noexcept {
    return m_cameraController;
}

OrthographicCameraController& GameStateMain::GetCameraController() noexcept {
    return m_cameraController;
}

void GameStateMain::HandleDebugInput(TimeUtils::FPSeconds deltaSeconds) {
    HandleDebugKeyboardInput(deltaSeconds);
    HandleDebugMouseInput(deltaSeconds);
}

void GameStateMain::HandleDebugKeyboardInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theUISystem->WantsInputKeyboardCapture()) {
        return;
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
    if (g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        g_theUISystem->ToggleClayDebugWindow();
    }
}

void GameStateMain::HandleDebugMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/) {
    if (g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
}

void GameStateMain::Render() const noexcept {

    g_theRenderer->BeginRenderToBackbuffer(m_waves.GetBackgroundColor());


    //3D World View


    //2D World / HUD View
    if(auto* g = GetGameAs<Game>(); g != nullptr) {
        const auto ui_view_height = static_cast<float>(g->GetSettings()->GetWindowHeight());
        const auto ui_view_width = ui_view_height * m_ui_camera.GetAspectRatio();
        const auto ui_view_extents = Vector2{ ui_view_width, ui_view_height };
        const auto ui_view_half_extents = ui_view_extents * 0.5f;
        const auto ui_cam_pos = m_ui_camera.CalcViewBounds().CalcCenter();

        g_theRenderer->BeginHUDRender(m_ui_camera.GetCamera(), ui_cam_pos, ui_view_height);

        RenderGround();
        RenderObjects();
        RenderCrosshairAt(m_mouse_world_pos);
        RenderRadarLine();
    }
}

void GameStateMain::EndFrame() noexcept {
    m_mouse_pos += m_mouse_delta;
    if (!g_theUISystem->IsAnyDebugWindowVisible()) {
        g_theInputSystem->SetCursorToWindowCenter();
        if (g_theInputSystem->IsMouseCursorVisible()) {
            g_theInputSystem->HideMouseCursor();
        }
    }
    m_mouse_delta = Vector2::Zero;
    m_missileBaseLeft.EndFrame();
    m_missileBaseCenter.EndFrame();
    m_missileBaseRight.EndFrame();
    m_cityManager.EndFrame();
    m_explosionManager.EndFrame();
    m_waves.EndFrame();
}
