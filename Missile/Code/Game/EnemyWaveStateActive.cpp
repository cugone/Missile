#include "Game/EnemyWaveStateActive.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"
#include "Game/GameStateMain.hpp"
#include "Game/EnemyWave.hpp"

#include "Game/EnemyWaveStatePostwave.hpp"

#include <format>

static Clay_LayoutConfig fullscreen_layout = {
    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
    .padding = CLAY_PADDING_ALL(0),
    .childGap = 0,
    .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_TOP},
    .layoutDirection = Clay_LayoutDirection::CLAY_TOP_TO_BOTTOM,
};

EnemyWaveStateActive::EnemyWaveStateActive(EnemyWave* context) noexcept
    : m_context(context)
{
    /* DO NOTHING */
}

void EnemyWaveStateActive::OnEnter() noexcept {
    g_theUISystem->SetClayLayoutCallback([this]() { this->ClayActive(); });
    m_context->SetMissileCount(m_context->GetMissileCountForWave());
    m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ m_context->GetFlierCooldown() });
    m_flierSpawnRate.Reset();
    m_context->ActivateWave();
}

void EnemyWaveStateActive::OnExit() noexcept {
    /* DO NOTHING */
}

void EnemyWaveStateActive::BeginFrame() noexcept {
    m_missiles.BeginFrame();
    if (m_bomber) {
        m_bomber->BeginFrame();
    }
    if (m_satellite) {
        m_satellite->BeginFrame();
    }
}

void EnemyWaveStateActive::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    UpdateMissiles(deltaSeconds);
    UpdateBomber(deltaSeconds);
    UpdateSatellite(deltaSeconds);
}

void EnemyWaveStateActive::Render() const noexcept {
    m_missiles.Render();
    if (m_bomber) {
        m_bomber->Render();
    }
    if (m_satellite) {
        m_satellite->Render();
    }
}

void EnemyWaveStateActive::DebugRender() const noexcept {
    m_missiles.DebugRender();
    if (m_bomber) {
        m_bomber->DebugRender();
    }
    if (m_satellite) {
        m_satellite->DebugRender();
    }
}

void EnemyWaveStateActive::EndFrame() noexcept {
    m_missiles.EndFrame();
    if (m_bomber) {
        m_bomber->EndFrame();
        if (m_bomber->IsDead()) {
            m_bomber.reset();
        }
    }
    if (m_satellite) {
        m_satellite->EndFrame();
        if (m_satellite->IsDead()) {
            m_satellite.reset();
        }
    }
    if (CanSpawnFlier()) {
        if (const auto is_bomber = MathUtils::GetRandomBool(); is_bomber) {
            if (!m_bomber) {
                SpawnBomber();
                m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ m_context->GetFlierCooldown() });
                m_flierSpawnRate.Reset();
            }
        } else {
            if (!m_satellite) {
                SpawnSatellite();
                m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ m_context->GetFlierCooldown() });
                m_flierSpawnRate.Reset();
            }
        }
    }
    if (IsWaveOver()) {
        AdvanceToNextWave();
    }
}

const MissileManager* EnemyWaveStateActive::GetMissileManager() const noexcept {
    return &m_missiles;
}

MissileManager* EnemyWaveStateActive::GetMissileManager() noexcept {
    return &m_missiles;
}

Bomber* const EnemyWaveStateActive::GetBomber() const noexcept {
    return m_bomber.get();
}

Satellite* const EnemyWaveStateActive::GetSatellite() const noexcept {
    return m_satellite.get();
}

bool EnemyWaveStateActive::CanSpawnFlier() const noexcept {
    return m_context->IsWaveActive() && m_context->GetWaveId() > 0 && m_context->GetRemainingMissiles() > 0 && (!m_bomber || !m_satellite) && m_flierSpawnRate.Check();
}

bool EnemyWaveStateActive::IsWaveOver() const noexcept {
    if (auto* g = GetGameAs<Game>(); g != nullptr) {
        if (auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState()); state != nullptr) {
            const auto all_explosions_finished = state->GetExplosionManager().ActiveExplosionCount() == 0;
            const auto no_missiles_in_flight = GetMissileManager()->ActiveMissileCount() == 0;
            const auto player_has_no_missiles_remaining = !state->HasMissilesRemaining();
            const auto wave_has_no_missiles_remaining = m_context->GetRemainingMissiles() == 0;
            const auto cant_score_points = player_has_no_missiles_remaining && no_missiles_in_flight && all_explosions_finished;
            const auto everything_dead = wave_has_no_missiles_remaining && !m_bomber && !m_satellite && all_explosions_finished;
            if (cant_score_points || everything_dead) {
                return true;
            }
        }
    }
    return false;
}

void EnemyWaveStateActive::SpawnBomber() noexcept {
    if (m_bomber) {
        return;
    }
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 bomber_spawn_area = state->GetWorldBounds();
    bomber_spawn_area.Translate(Vector2::X_Axis * -100.0f);
    bomber_spawn_area.AddPaddingToSides(0.0f, -GameConstants::radar_line_distance);
    bomber_spawn_area.maxs.x = state->GetWorldBounds().mins.x;
    m_bomber = std::make_unique<Bomber>(this->m_context, MathUtils::GetRandomPointInside(bomber_spawn_area));
}

void EnemyWaveStateActive::SpawnSatellite() noexcept {
    if (m_satellite) {
        return;
    }
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 satellite_spawn_area = state->GetWorldBounds();
    satellite_spawn_area.Translate(Vector2::X_Axis * 100.0f);
    satellite_spawn_area.AddPaddingToSides(0.0f, -100.0f);
    satellite_spawn_area.mins.x = state->GetWorldBounds().maxs.x;
    m_satellite = std::make_unique<Satellite>(this->m_context, MathUtils::GetRandomPointInside(satellite_spawn_area));
}

void EnemyWaveStateActive::SpawnMissile() noexcept {
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 missile_spawn_area = state->GetWorldBounds();
    missile_spawn_area.Translate(Vector2::Y_Axis * -100.0f);
    missile_spawn_area.AddPaddingToSides(-100.0f, 0.0f);
    missile_spawn_area.maxs.y = state->GetWorldBounds().mins.y;
    Vector2 pos = MathUtils::GetRandomPointInside(missile_spawn_area);

    LaunchMissileFrom(pos);
}

void EnemyWaveStateActive::UpdateMissiles(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if (m_context->IsWaveActive() && CanSpawnMissile()) {
        if (m_missileSpawnRate.CheckAndReset()) {
            SpawnMissile();
        }
    }
    m_missiles.Update(deltaSeconds);
}

bool EnemyWaveStateActive::CanSpawnMissile() const noexcept {
    return m_context->GetRemainingMissiles() > 0 && m_missiles.ActiveMissileCount() < GameConstants::max_missles_on_screen;
}

void EnemyWaveStateActive::UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if (!m_satellite) {
        return;
    }
    m_satellite->Update(deltaSeconds);
    Disc2 satellite_visible{ m_satellite->GetPosition(), 50.0f };
    const auto satellite_right = satellite_visible.center.x + satellite_visible.radius;
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 bounds = state->GetWorldBounds();
    const auto bounds_left = bounds.mins.x;
    if (satellite_right < bounds_left) {
        m_satellite.reset();
        m_flierSpawnRate.Reset();
    }
}

void EnemyWaveStateActive::UpdateBomber(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if (!m_bomber) {
        return;
    }
    m_bomber->Update(deltaSeconds);
    Disc2 bomber_visible{ m_bomber->GetPosition(), 50.0f };
    const auto bomber_left = bomber_visible.center.x - bomber_visible.radius;
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 bounds = state->GetWorldBounds();
    const auto bounds_right = bounds.maxs.x;
    if (bounds_right < bomber_left) {
        m_bomber.reset();
        m_flierSpawnRate.Reset();
    }
}

void EnemyWaveStateActive::AdvanceToNextWave() noexcept {
    m_bomber.reset();
    m_satellite.reset();
    m_context->ChangeState(std::make_unique<EnemyWaveStatePostwave>(m_context));
}

bool EnemyWaveStateActive::LaunchMissileFrom(Vector2 position) noexcept {
    if (CanSpawnMissile()) {
        if (const auto* g = GetGameAs<Game>(); g != nullptr) {
            if (const auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState()); state != nullptr) {
                const auto& targets = state->GetValidTargets();
                const auto& target = targets[MathUtils::GetRandomLessThan(targets.size())];
                m_context->DecrementMissileCount();
                return m_missiles.LaunchMissile(position, target, m_context->GetMissileImpactTime(), Faction::Enemy, m_context->GetObjectColor());
            }
        }
    }
    return false;
}

void EnemyWaveStateActive::ClayActive() noexcept {
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = fullscreen_layout, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        RenderScoreElement();
    }
}

void EnemyWaveStateActive::RenderScoreElement() const noexcept {
    CLAY({ .id = CLAY_ID("Score"), .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(0.1f)}, .padding = CLAY_PADDING_ALL(0), .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_TOP},}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        static auto points_str = std::string{};
        points_str = [this]()->std::string {
            const auto player_score = GetGameAs<Game>()->GetPlayerScore();
            const auto highscore = GetGameAs<Game>()->GetHighScore();
            const auto wave = m_context->GetWaveId() + 1;
            if (player_score > highscore) {
                return std::format("{} <- {}\nWave: {}", player_score, highscore, wave);
            } else {
                return std::format("{} -> {}\nWave: {}", player_score, highscore, wave);
            }
            }();
        Clay_TextElementConfig textConfig{};
        textConfig.userData = g_theRenderer->GetFont("System32");
        textConfig.textColor = Clay::RgbaToClayColor(Rgba::White);
        textConfig.wrapMode = Clay_TextElementConfigWrapMode::CLAY_TEXT_WRAP_NEWLINES;
        CLAY_TEXT(Clay::StrToClayString(points_str), CLAY_TEXT_CONFIG(textConfig));
    }
}

void EnemyWaveStateActive::RenderScoreMultiplierElement() const noexcept {
    CLAY({ .id = CLAY_ID("ScoreMultiplier"), .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(0), .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER}}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        static auto points_str = std::string{};
        points_str = std::format("{} X POINTS", m_context->GetScoreMultiplier());
        Clay_TextElementConfig textConfig{};
        textConfig.userData = g_theRenderer->GetFont("System32");
        textConfig.textColor = Clay::RgbaToClayColor(m_context->GetObjectColor());
        CLAY_TEXT(Clay::StrToClayString(points_str), CLAY_TEXT_CONFIG(textConfig));
    }
}
