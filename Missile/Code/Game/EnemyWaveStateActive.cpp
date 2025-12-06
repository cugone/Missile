#include "Game/EnemyWaveStateActive.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"
#include "Game/GameStateMain.hpp"
#include "Game/EnemyWave.hpp"

#include "Game/EnemyWaveStatePostwave.hpp"

#ifdef PROFILE_BUILD
#include <Thirdparty/Tracy/tracy/Tracy.hpp>
#endif

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
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    /* DO NOTHING */
}

void EnemyWaveStateActive::OnEnter() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    g_theUISystem->SetClayLayoutCallback([this]() { this->ClayActive(); });
    m_context->SetMissileCount(m_context->GetMissileCountForWave());
    m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ m_context->GetFlierCooldown() });
    m_flierSpawnRate.Reset();
    m_context->ActivateWave();
}

void EnemyWaveStateActive::OnExit() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    /* DO NOTHING */
}

void EnemyWaveStateActive::BeginFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_missiles.BeginFrame();
    if (m_bomber) {
        m_bomber->BeginFrame();
    }
    if (m_satellite) {
        m_satellite->BeginFrame();
    }
    if (m_smartBomb) {
        m_smartBomb->BeginFrame();
    }
}

void EnemyWaveStateActive::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    UpdateMissiles(deltaSeconds);
    UpdateBomber(deltaSeconds);
    UpdateSatellite(deltaSeconds);
    UpdateSmartBomb(deltaSeconds);
}

void EnemyWaveStateActive::Render() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_missiles.Render();
    if (m_bomber) {
        m_bomber->Render();
    }
    if (m_satellite) {
        m_satellite->Render();
    }
    if (m_smartBomb) {
        m_smartBomb->Render();
    }
}

void EnemyWaveStateActive::DebugRender() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_missiles.DebugRender();
    if (m_bomber) {
        m_bomber->DebugRender();
    }
    if (m_satellite) {
        m_satellite->DebugRender();
    }
    if (m_smartBomb) {
        m_smartBomb->DebugRender();
    }
}

void EnemyWaveStateActive::EndFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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
    if (m_smartBomb) {
        m_smartBomb->EndFrame();
        if (m_smartBomb->IsDead()) {
            m_smartBomb.reset();
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
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return &m_missiles;
}

MissileManager* EnemyWaveStateActive::GetMissileManager() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return &m_missiles;
}

Bomber* const EnemyWaveStateActive::GetBomber() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_bomber.get();
}

Satellite* const EnemyWaveStateActive::GetSatellite() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_satellite.get();
}

SmartBomb* const EnemyWaveStateActive::GetSmartBomb() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_smartBomb.get();
}

bool EnemyWaveStateActive::CanSpawnFlier() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_context->IsWaveActive() && m_context->GetWaveId() > 0 && m_context->GetRemainingMissiles() > 0 && (!m_bomber || !m_satellite) && m_flierSpawnRate.Check();
}

bool EnemyWaveStateActive::IsWaveOver() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 missile_spawn_area = state->GetWorldBounds();
    missile_spawn_area.Translate(Vector2::Y_Axis * -100.0f);
    missile_spawn_area.AddPaddingToSides(-100.0f, 0.0f);
    missile_spawn_area.maxs.y = state->GetWorldBounds().mins.y;
    Vector2 pos = MathUtils::GetRandomPointInside(missile_spawn_area);

    LaunchMissileFrom(pos);
}

void EnemyWaveStateActive::SpawnSmartBomb() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    if (m_context->IsWaveActive() && CanSpawnMissile()) {
        if (m_missileSpawnRate.CheckAndReset()) {
            SpawnMissile();
        }
    }
    m_missiles.Update(deltaSeconds);
}

bool EnemyWaveStateActive::CanSpawnMissile() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_context->GetRemainingMissiles() > 0 && m_missiles.ActiveMissileCount() < GameConstants::max_missles_on_screen;
}

void EnemyWaveStateActive::UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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

void EnemyWaveStateActive::UpdateSmartBomb(TimeUtils::FPSeconds deltaSeconds) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    if(!m_smartBomb) {
        return;
    }
    m_smartBomb->Update(deltaSeconds);
}

void EnemyWaveStateActive::AdvanceToNextWave() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    m_bomber.reset();
    m_satellite.reset();
    m_smartBomb.reset();
    m_context->ChangeState(std::make_unique<EnemyWaveStatePostwave>(m_context));
}

bool EnemyWaveStateActive::LaunchMissileFrom(Vector2 position) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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

bool EnemyWaveStateActive::CanSpawnSmartBomb() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    return m_context->GetRemainingSmartBombs() > 0 && m_missiles.ActiveMissileCount() < GameConstants::max_missles_on_screen;
}

bool EnemyWaveStateActive::LaunchSmartBombFrom(Vector2 position) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    //if(CanSpawnSmartBomb()) {
        if (const auto* g = GetGameAs<Game>(); g != nullptr) {
            if (auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState()); state != nullptr) {
                const auto& targets = state->GetValidTargets();
                const auto& target = targets[MathUtils::GetRandomLessThan(targets.size())].value;
                m_context->DecrementSmartBombCount();
                m_smartBomb = std::make_unique<SmartBomb>(&state->GetExplosionManager(), position, target);
                return true;
            }
        }
    //}
    return false;
}

void EnemyWaveStateActive::ClayActive() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = fullscreen_layout, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        RenderScoreElement();
    }
}

void EnemyWaveStateActive::RenderScoreElement() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
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
        auto* font = g_theRenderer->GetDefaultFont();
        textConfig.fontId = static_cast<uint16_t>(g_theRenderer->GetFontId(font->GetName()));
        textConfig.fontSize = static_cast<uint16_t>(font->GetEmSize());
        textConfig.textColor = Clay::RgbaToClayColor(Rgba::White);
        textConfig.wrapMode = Clay_TextElementConfigWrapMode::CLAY_TEXT_WRAP_NEWLINES;
        CLAY_TEXT(Clay::StrToClayString(points_str), CLAY_TEXT_CONFIG(textConfig));
    }
}

void EnemyWaveStateActive::RenderScoreMultiplierElement() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
    CLAY({ .id = CLAY_ID("ScoreMultiplier"), .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(0), .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER}}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        static auto points_str = std::string{};
        points_str = std::format("{} X POINTS", m_context->GetScoreMultiplier());
        Clay_TextElementConfig textConfig{};
        textConfig.userData = g_theRenderer->GetDefaultFont();
        textConfig.textColor = Clay::RgbaToClayColor(m_context->GetObjectColor());
        CLAY_TEXT(Clay::StrToClayString(points_str), CLAY_TEXT_CONFIG(textConfig));
    }
}
