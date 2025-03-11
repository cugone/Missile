#include "Game/EnemyWave.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IRendererService.hpp"
#include "Engine/Services/IInputService.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"

#include <format>
#include <utility>

void EnemyWave::BeginFrame() noexcept {
    if(m_nextState != m_currentState) {
        m_currentState = m_nextState;
        if(m_currentState != State::Active) {
            m_postWaveIncrementRate.Reset();
            m_postWaveTimer.Reset();
            m_preWaveTimer.Reset();
            DeactivateWave();
            if(m_currentState == State::Prewave) {
                g_theUISystem->SetClayLayoutCallback([this]() { this->ClayPrewave(); });
            }
            if(m_currentState == State::Postwave) {
                g_theUISystem->SetClayLayoutCallback([this]() { this->ClayPostwave(); });
            }
        } else {
            if(m_currentState == State::Active) {
                g_theUISystem->SetClayLayoutCallback([this]() { this->ClayActive(); });
            }
            ActivateWave();
        }
    }
    switch (m_currentState) {
    case State::Inactive:
        BeginFrame_Inactive();
        break;
    case State::Prewave:
        BeginFrame_Prewave();
        break;
    case State::Active:
        BeginFrame_Active();
        break;
    case State::Postwave:
        BeginFrame_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    switch (m_currentState) {
    case State::Inactive:
        Update_Inactive(deltaSeconds);
        break;
    case State::Prewave:
        Update_Prewave(deltaSeconds);
        break;
    case State::Active:
        Update_Active(deltaSeconds);
        break;
    case State::Postwave:
        Update_Postwave(deltaSeconds);
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::Render() const noexcept {
    switch (m_currentState) {
    case State::Inactive:
        Render_Inactive();
        break;
    case State::Prewave:
        Render_Prewave();
        break;
    case State::Active:
        Render_Active();
        break;
    case State::Postwave:
        Render_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::EndFrame() noexcept {
    switch (m_currentState) {
    case State::Inactive:
        EndFrame_Inactive();
        break;
    case State::Prewave:
        EndFrame_Prewave();
        break;
    case State::Active:
        EndFrame_Active();
        break;
    case State::Postwave:
        EndFrame_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

/************************************************************************/
/*                         BEGIN FRAME                                  */
/************************************************************************/

void EnemyWave::BeginFrame_Inactive() noexcept {
    /* DO NOTHING */
}

void EnemyWave::BeginFrame_Prewave() noexcept {
    /* DO NOTHING */
}

void EnemyWave::BeginFrame_Active() noexcept {
    m_missiles.BeginFrame();
    if (m_bomber) {
        m_bomber->BeginFrame();
    }
    if (m_satellite) {
        m_satellite->BeginFrame();
    }
}

void EnemyWave::BeginFrame_Postwave() noexcept {
    static int wave_bonus = 0;
    const auto* g = GetGameAs<Game>();
    auto* state = g->GetCurrentState();
    auto* main_state = dynamic_cast<GameStateMain*>(state);
    if(main_state->HasMissilesRemaining()) {
        if(m_postWaveIncrementRate.CheckAndReset()) {
            wave_bonus += GameConstants::unused_missile_value * GetScoreMultiplier();
            main_state->DecrementTotalMissiles();
            g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
        }
    } else {
        if(main_state->GetCityManager().RemainingCities()) {
            if(m_postWaveIncrementRate.CheckAndReset()) {
                if(main_state->GetCityManager().GetCity(0).IsDead() == false) {
                    wave_bonus += GameConstants::saved_city_value * GetScoreMultiplier();
                    main_state->GetCityManager().GetCity(0).Kill();
                    g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
                }
                if(main_state->GetCityManager().GetCity(1).IsDead() == false) {
                    wave_bonus += GameConstants::saved_city_value * GetScoreMultiplier();
                    main_state->GetCityManager().GetCity(1).Kill();
                    g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
                }
                if(main_state->GetCityManager().GetCity(2).IsDead() == false) {
                    wave_bonus += GameConstants::saved_city_value * GetScoreMultiplier();
                    main_state->GetCityManager().GetCity(2).Kill();
                    g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
                }
                if(main_state->GetCityManager().GetCity(3).IsDead() == false) {
                    wave_bonus += GameConstants::saved_city_value * GetScoreMultiplier();
                    main_state->GetCityManager().GetCity(3).Kill();
                    g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
                }
                if(main_state->GetCityManager().GetCity(4).IsDead() == false) {
                    wave_bonus += GameConstants::saved_city_value * GetScoreMultiplier();
                    main_state->GetCityManager().GetCity(4).Kill();
                    g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
                }
                if(main_state->GetCityManager().GetCity(5).IsDead() == false) {
                    wave_bonus += GameConstants::saved_city_value * GetScoreMultiplier();
                    main_state->GetCityManager().GetCity(5).Kill();
                    g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
                }
            }
        }
    }
    if (m_postWaveTimer.CheckAndReset()) {
        IncrementWave();
        ChangeState(EnemyWave::State::Prewave);
    }
    wave_bonus = 0;
}

/************************************************************************/
/*                         UPDATE                                       */
/************************************************************************/


void EnemyWave::Update_Inactive([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void EnemyWave::Update_Prewave([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void EnemyWave::Update_Active(TimeUtils::FPSeconds deltaSeconds) noexcept {
    UpdateMissiles(deltaSeconds);
    UpdateBomber(deltaSeconds);
    UpdateSatellite(deltaSeconds);
}

void EnemyWave::Update_Postwave([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {

}

void EnemyWave::UpdateMissiles(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(IsWaveActive() && CanSpawnMissile()) {
        if(m_missileSpawnRate.CheckAndReset()) {
            SpawnMissile();
        }
    }
    m_missiles.Update(deltaSeconds);
}

bool EnemyWave::CanSpawnMissile() const noexcept {
    return m_missileCount != 0 && m_missiles.ActiveMissileCount() < GameConstants::max_missles_on_screen;
}

void EnemyWave::UpdateSatellite(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(!m_satellite) {
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

void EnemyWave::UpdateBomber(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(!m_bomber) {
        return;
    }
    m_bomber->Update(deltaSeconds);
    Disc2 bomber_visible{ m_bomber->GetPosition(), 50.0f };
    const auto bomber_left = bomber_visible.center.x - bomber_visible.radius;
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 bounds = state->GetWorldBounds();
    const auto bounds_right = bounds.maxs.x;
    if(bounds_right < bomber_left) {
        m_bomber.reset();
        m_flierSpawnRate.Reset();
    }
}

bool EnemyWave::LaunchMissileFrom(Vector2 position) noexcept {
    if(CanSpawnMissile()) {
        if (const auto* g = GetGameAs<Game>(); g != nullptr) {
            if (const auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState()); state != nullptr) {
                const auto& targets = state->GetValidTargets();
                const auto& target = targets[MathUtils::GetRandomLessThan(targets.size())];
                m_missileCount = (std::max)(0, m_missileCount - 1);
                return m_missiles.LaunchMissile(position, target, GetMissileImpactTime(), Faction::Enemy, GetObjectColor());
            }
        }
    }
    return false;
}

/************************************************************************/
/*                         RENDER                                       */
/************************************************************************/

void EnemyWave::Render_Inactive() const noexcept {
    /* DO NOTHING */
}

static Clay_LayoutConfig fullscreen_layout = {
    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
    .padding = CLAY_PADDING_ALL(16),
    .childGap = 16
};

void EnemyWave::ClayPrewave() noexcept {
    fullscreen_layout.childAlignment.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER;
    fullscreen_layout.childAlignment.y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER;
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = fullscreen_layout, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        CLAY({ .id = CLAY_ID("ScoreMultiplier"), .layout = {.padding = CLAY_PADDING_ALL(16)}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
            static auto points_str = std::string{};
            points_str = std::format("{} X POINTS", this->GetScoreMultiplier());
            Clay_TextElementConfig textConfig{};
            textConfig.userData = g_theRenderer->GetFont("System32");
            textConfig.textColor = Clay::RgbaToClayColor(this->GetObjectColor());
            CLAY_TEXT(Clay::StrToClayString(points_str), CLAY_TEXT_CONFIG(textConfig));
        }
    }
}

void EnemyWave::ClayActive() noexcept {
    fullscreen_layout.childAlignment.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER;
    fullscreen_layout.childAlignment.y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_TOP;
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = fullscreen_layout, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        CLAY({ .id = CLAY_ID("Score"), .layout = {.padding = CLAY_PADDING_ALL(0)}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
            static auto points_str = std::string{};
            points_str = [this]()->std::string {
                const auto player_score = GetGameAs<Game>()->GetPlayerScore();
                const auto highscore = GetGameAs<Game>()->GetHighScore();
                const auto wave = this->GetWaveId() + 1;
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
}

void EnemyWave::ClayPostwave() noexcept {
    /* DO NOTHING */
}

void EnemyWave::Render_Prewave() const noexcept {
    /* DO NOTHING */
}

void EnemyWave::Render_Active() const noexcept {
    m_missiles.Render();
    if (m_bomber) {
        m_bomber->Render();
    }
    if (m_satellite) {
        m_satellite->Render();
    }
}

void EnemyWave::Render_Postwave() const noexcept {
    /* DO NOTHING */
}

void EnemyWave::DebugRender() const noexcept {
    switch (m_currentState) {
    case State::Inactive:
        DebugRender_Inactive();
        break;
    case State::Prewave:
        DebugRender_Prewave();
        break;
    case State::Active:
        DebugRender_Active();
        break;
    case State::Postwave:
        DebugRender_Postwave();
        break;
    default:
        ERROR_AND_DIE("EnemyWave State scoped enum has changed");
    }
}

void EnemyWave::DebugRender_Inactive() const noexcept {

}

void EnemyWave::DebugRender_Prewave() const noexcept {

}

void EnemyWave::DebugRender_Active() const noexcept {
    m_missiles.DebugRender();
    if (m_bomber) {
        m_bomber->DebugRender();
    }
    if (m_satellite) {
        m_satellite->DebugRender();
    }
}

void EnemyWave::DebugRender_Postwave() const noexcept {

}

/************************************************************************/
/*                         END FRAME                                    */
/************************************************************************/

void EnemyWave::EndFrame_Inactive() noexcept {

}

void EnemyWave::EndFrame_Prewave() noexcept {
    if (m_preWaveTimer.CheckAndReset()) {
        SetMissileCount(GetMissileCount());
        m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ GetFlierCooldown() });
        m_flierSpawnRate.Reset();
        auto* g = GetGameAs<Game>();
        auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
        state->ResetMissileCount();
        ChangeState(EnemyWave::State::Active);
    }
}

void EnemyWave::EndFrame_Active() noexcept {
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
                m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ GetFlierCooldown() });
                m_flierSpawnRate.Reset();
            }
        } else {
            if (!m_satellite) {
                SpawnSatellite();
                m_flierSpawnRate.SetSeconds(TimeUtils::FPFrames{ GetFlierCooldown() });
                m_flierSpawnRate.Reset();
            }
        }
    }
    if (IsWaveOver()) {
        AdvanceToNextWave();
    }
}

void EnemyWave::EndFrame_Postwave() noexcept {

}

void EnemyWave::AdvanceToNextWave() noexcept {
    if (auto* g = GetGameAs<Game>(); g != nullptr) {
        auto* state = g->GetCurrentState();
        if (auto* main_state = dynamic_cast<GameStateMain*>(state); main_state != nullptr) {
            m_postWaveCityCount = main_state->GetCityManager().RemainingCities();
        }
    }
    m_bomber.reset();
    m_satellite.reset();
    ChangeState(State::Postwave);
}

void EnemyWave::ChangeState(State newState) noexcept {
    m_nextState = newState;
}

bool EnemyWave::CanSpawnFlier() const noexcept {
    return IsWaveActive() && m_waveId > 0 && m_missileCount && (!m_bomber || !m_satellite) && m_flierSpawnRate.Check();
}

bool EnemyWave::IsWaveOver() const noexcept {
    if (auto* g = GetGameAs<Game>(); g != nullptr) {
        if (auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState()); state != nullptr) {
            const auto all_explosions_finished = state->GetExplosionManager().ActiveExplosionCount() == 0;
            const auto no_missiles_in_flight = state->GetMissileManager().ActiveMissileCount() == 0;
            const auto player_has_no_missiles_remaining = !state->HasMissilesRemaining();
            const auto wave_has_no_missiles_remaining = m_missileCount == 0;
            const auto cant_score_points = player_has_no_missiles_remaining && no_missiles_in_flight && all_explosions_finished;
            const auto everything_dead = wave_has_no_missiles_remaining && !m_bomber && !m_satellite && all_explosions_finished;
            if (cant_score_points || everything_dead) {
                return true;
            }
        }
    }
    return false;
}

float EnemyWave::GetFlierCooldown() const noexcept {
    return m_waveId < GameConstants::wave_flier_cooldown_lookup.size() ? GameConstants::wave_flier_cooldown_lookup[m_waveId] : GameConstants::min_bomber_cooldown;
}

const MissileManager& EnemyWave::GetMissileManager() const noexcept {
    return m_missiles;
}

MissileManager& EnemyWave::GetMissileManager() noexcept {
    return m_missiles;
}

std::size_t EnemyWave::GetWaveId() const noexcept {
    return m_waveId;
}

void EnemyWave::IncrementWave() noexcept {
    m_waveId += 1;
}

int EnemyWave::GetScoreMultiplier() const noexcept {
    return m_waveId < GameConstants::wave_score_multiplier_lookup.size() ? GameConstants::wave_score_multiplier_lookup[m_waveId] : GameConstants::max_score_multiplier;
}

Rgba EnemyWave::GetObjectColor() const noexcept {
    return Rgba(GameConstants::wave_object_color_lookup[m_waveId % GameConstants::wave_array_size]);
}

Rgba EnemyWave::GetBackgroundColor() const noexcept {
    return Rgba(GameConstants::wave_background_color_lookup[m_waveId % GameConstants::wave_array_size]);
}

TimeUtils::FPSeconds EnemyWave::GetMissileImpactTime() const noexcept {
    return TimeUtils::FPSeconds{m_waveId < GameConstants::wave_missile_impact_time.size() ? GameConstants::wave_missile_impact_time[m_waveId] : GameConstants::min_missile_impact_time};
}

int EnemyWave::GetMissileCount() const noexcept {
    return m_waveId < GameConstants::wave_missile_count_lookup.size() ? GameConstants::wave_missile_count_lookup[m_waveId] : GameConstants::max_missile_count;
}

void EnemyWave::SetMissileSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept {
    m_missileSpawnRate.SetSeconds(secondsBetween);
}

void EnemyWave::SetBomberSpawnRate(TimeUtils::FPSeconds secondsBetween) noexcept {
    m_flierSpawnRate.SetSeconds(secondsBetween);
}

void EnemyWave::SetMissileCount(int newMissileCount) noexcept {
    m_missileCount = newMissileCount;
}

int EnemyWave::GetRemainingMissiles() const noexcept {
    return m_missileCount;
}

bool EnemyWave::IsWaveActive() const noexcept {
    return m_isActive;
}

void EnemyWave::ActivateWave() noexcept {
    m_isActive = true;
}

void EnemyWave::DeactivateWave() noexcept {
    m_isActive = false;
}

void EnemyWave::SpawnBomber() noexcept {
    if(m_bomber) {
        return;
    }
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 bomber_spawn_area = state->GetWorldBounds();
    bomber_spawn_area.Translate(Vector2::X_Axis * -100.0f);
    bomber_spawn_area.AddPaddingToSides(0.0f, -GameConstants::radar_line_distance);
    bomber_spawn_area.maxs.x = state->GetWorldBounds().mins.x;
    m_bomber = std::make_unique<Bomber>(this, MathUtils::GetRandomPointInside(bomber_spawn_area));
}

void EnemyWave::SpawnSatellite() noexcept {
    if (m_satellite) {
        return;
    }
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 satellite_spawn_area = state->GetWorldBounds();
    satellite_spawn_area.Translate(Vector2::X_Axis * 100.0f);
    satellite_spawn_area.AddPaddingToSides(0.0f, -100.0f);
    satellite_spawn_area.mins.x = state->GetWorldBounds().maxs.x;
    m_satellite = std::make_unique<Satellite>(this, MathUtils::GetRandomPointInside(satellite_spawn_area));
}

void EnemyWave::SpawnMissile() noexcept {
    auto* g = GetGameAs<Game>();
    auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
    AABB2 missile_spawn_area = state->GetWorldBounds();
    missile_spawn_area.Translate(Vector2::Y_Axis * -100.0f);
    missile_spawn_area.AddPaddingToSides(-100.0f, 0.0f);
    missile_spawn_area.maxs.y = state->GetWorldBounds().mins.y;
    Vector2 pos = MathUtils::GetRandomPointInside(missile_spawn_area);

    LaunchMissileFrom(pos);
}

Bomber* const EnemyWave::GetBomber() const noexcept {
    return m_bomber.get();
}

Satellite* const EnemyWave::GetSatellite() const noexcept {
    return m_satellite.get();
}
