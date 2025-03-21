#include "Game/EnemyWaveStatePostwave.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"
#include "Game/EnemyWave.hpp"
#include "Game/EnemyWaveStatePrewave.hpp"

#include <format>

static Clay_LayoutConfig fullscreen_layout = {
    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
    .padding = CLAY_PADDING_ALL(0),
    .childGap = 0,
    .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_TOP},
    .layoutDirection = Clay_LayoutDirection::CLAY_TOP_TO_BOTTOM,
};



EnemyWaveStatePostwave::EnemyWaveStatePostwave(EnemyWave* context) noexcept
    : m_context(context)
{
    /* DO NOTHING */
}

void EnemyWaveStatePostwave::OnEnter() noexcept {
    auto* g = GetGameAs<Game>();
    auto* state = g->GetCurrentState();
    auto* main_state = dynamic_cast<GameStateMain*>(state);

    m_context->DeactivateWave();
    m_postWaveIncrementRate.Reset();
    m_postWaveTimer.Reset();
    m_missilesRemainingPostWave = 0;
    m_citiesRemainingPostWave = 0;
    m_alive_cities = std::array<bool, GameConstants::max_cities>{ false, false, false, false, false, false };
    for (std::size_t i = 0; i < GameConstants::max_cities; ++i) {
        m_alive_cities[i] = main_state->GetCityManager().GetCity(i).IsAlive();
    }
    m_city_idx = 0u;
    m_canTransision = false;
    m_showBonusCityText = false;
    m_remaining_cities = main_state->GetCityManager().RemainingCitiesCount();

    g_theUISystem->SetClayLayoutCallback([this]() { this->ClayPostwave(); });
}

void EnemyWaveStatePostwave::OnExit() noexcept {
    auto* g = GetGameAs<Game>();
    auto* state = g->GetCurrentState();
    auto* main_state = dynamic_cast<GameStateMain*>(state);
    if (m_grantedCityThisWave) {
        main_state->GetCityManager().RedeemBonusCIty();
        if (std::any_of(std::begin(m_alive_cities), std::end(m_alive_cities), [](bool a) { return a == false; })) {
            auto i = MathUtils::GetRandomLessThan(m_alive_cities.size());
            do {
                i = MathUtils::GetRandomLessThan(m_alive_cities.size());
            } while (m_alive_cities[i] == true);
            for (std::size_t idx = 0; idx < m_alive_cities.size(); ++idx) {
                if(m_alive_cities[idx]) {
                    main_state->GetCityManager().GetCity(i).Resurrect();
                    break;
                }
            }
        }
    }
    for (std::size_t i = 0; i < m_alive_cities.size(); ++i) {
        if (m_alive_cities[i]) {
            main_state->GetCityManager().GetCity(i).Resurrect();
        }
    }
    std::fill(std::begin(m_alive_cities), std::end(m_alive_cities), false);
    m_remaining_cities = main_state->GetCityManager().RemainingCitiesCount();
    m_city_idx = 0;
    m_showBonusCityText = false;
    m_canTransision = false;
    m_context->IncrementWave();
}

void EnemyWaveStatePostwave::BeginFrame() noexcept {
    auto* g = GetGameAs<Game>();
    auto* state = g->GetCurrentState();
    auto* main_state = dynamic_cast<GameStateMain*>(state);
    if (main_state->HasMissilesRemaining()) {
        if (m_postWaveIncrementRate.CheckAndReset()) {
            g->AdjustPlayerScore(GameConstants::unused_missile_value * m_context->GetScoreMultiplier());
            main_state->DecrementTotalMissiles();
            ++m_missilesRemainingPostWave;
            g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
        }
    } else if (main_state->GetCityManager().RemainingCitiesCount()) {
        if (m_postWaveIncrementRate.CheckAndReset()) {
            if (m_city_idx < GameConstants::max_cities) {
                if (main_state->GetCityManager().GetCity(m_city_idx).IsAlive()) {
                    m_alive_cities[m_city_idx] = true;
                    g->AdjustPlayerScore(GameConstants::saved_city_value * m_context->GetScoreMultiplier());
                    main_state->GetCityManager().GetCity(m_city_idx).Kill();
                    ++m_citiesRemainingPostWave;
                    g_theAudioSystem->Play(GameConstants::game_audio_counting_path, AudioSystem::SoundDesc{});
                }
            }
            ++m_city_idx;
        }
    } else if (main_state->GetCityManager().IsBonusCityAvailable() && !m_grantedCityThisWave) {
        if (m_postWaveIncrementRate.CheckAndReset() && !m_showBonusCityText) {
            m_grantedCityThisWave = true;
            m_showBonusCityText = true;
            g_theAudioSystem->Play(GameConstants::game_audio_bonuscity_path, AudioSystem::SoundDesc{});
        }
    } else {
        if (!m_canTransision) {
            m_canTransision = true;
            m_postWaveTimer.Reset();
        }
    }
}

void EnemyWaveStatePostwave::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePostwave::Render() const noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePostwave::DebugRender() const noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePostwave::EndFrame() noexcept {
    if (m_postWaveTimer.CheckAndReset()) {
        if (m_canTransision) {
            m_canTransision = false;
            m_context->ChangeState(std::make_unique<EnemyWaveStatePrewave>(m_context));
        }
    }
}

void EnemyWaveStatePostwave::ClayPostwave() noexcept {
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = fullscreen_layout, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        RenderScoreElement();
        RenderPostWaveStatsElement();
    }
}

void EnemyWaveStatePostwave::RenderScoreElement() const noexcept {
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

void EnemyWaveStatePostwave::RenderScoreMultiplierElement() const noexcept {
    CLAY({ .id = CLAY_ID("ScoreMultiplier"), .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(0), .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER}}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        static auto points_str = std::string{};
        points_str = std::format("{} X POINTS", m_context->GetScoreMultiplier());
        Clay_TextElementConfig textConfig{};
        textConfig.userData = g_theRenderer->GetFont("System32");
        textConfig.textColor = Clay::RgbaToClayColor(m_context->GetObjectColor());
        CLAY_TEXT(Clay::StrToClayString(points_str), CLAY_TEXT_CONFIG(textConfig));
    }
}

void EnemyWaveStatePostwave::RenderPostWaveStatsElement() const noexcept {
    const Clay_TextElementConfig textConfig{ .userData = g_theRenderer->GetFont("System32"), .textColor = Clay::RgbaToClayColor(m_context->GetObjectColor()) };

    CLAY({ .id = CLAY_ID("PostwaveStatsContainer"), .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .childGap = 16, .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_TOP}, .layoutDirection = Clay_LayoutDirection::CLAY_TOP_TO_BOTTOM,}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        CLAY_TEXT(CLAY_STRING_CONST("BONUS POINTS"), CLAY_TEXT_CONFIG(textConfig));
        CLAY_TEXT(CLAY_STRING_CONST("MISSILES"), CLAY_TEXT_CONFIG(textConfig));
        {
            const auto dims = Clay::Vector2ToClayDimensions(Vector2(IntVector2(g_theRenderer->GetMaterial("missile")->GetTexture(Material::TextureID::Diffuse)->GetDimensions())));
            CLAY({ .id = CLAY_ID("MissileImagesContainer"), .layout = {.sizing = {.width = CLAY_SIZING_FIXED(dims.width * GameConstants::max_player_missile_count), .height = CLAY_SIZING_FIXED(dims.height)}, .childGap = 8, .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_LEFT, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER}, .layoutDirection = Clay_LayoutDirection::CLAY_LEFT_TO_RIGHT} }) {
                RenderMissileImageElements();
            }
        }
        CLAY_TEXT(CLAY_STRING_CONST("CITIES"), CLAY_TEXT_CONFIG(textConfig));
        {
            const auto dims = Clay::Vector2ToClayDimensions(Vector2(IntVector2(g_theRenderer->GetMaterial("city")->GetTexture(Material::TextureID::Diffuse)->GetDimensions())));
            CLAY({ .id = CLAY_ID("CityImagesContainer"), .layout = {.sizing = {.width = CLAY_SIZING_FIXED(dims.width * GameConstants::max_cities), .height = CLAY_SIZING_FIXED(dims.height)}, .childGap = static_cast<uint8_t>(dims.width + 8), .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_LEFT, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER}, .layoutDirection = Clay_LayoutDirection::CLAY_LEFT_TO_RIGHT} }) {
                RenderCityImageElements();
            }
        }
        if (m_showBonusCityText) {
            CLAY({ .layout = {.sizing = {}, .padding = {0, 0, 16, 0}, .childGap = 16, .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_LEFT, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER}, .layoutDirection = Clay_LayoutDirection::CLAY_LEFT_TO_RIGHT} }) {
                CLAY_TEXT(CLAY_STRING_CONST("BONUS CITY"), CLAY_TEXT_CONFIG(textConfig));
                const auto player_color = []()->Rgba {
                    if (const auto* g = GetGameAs<Game>(); g != nullptr) {
                        if (auto* mainState = dynamic_cast<GameStateMain*>(g->GetCurrentState()); mainState != nullptr) {
                            return mainState->GetPlayerColor();
                        }
                        return Rgba::Black;
                    }
                    return Rgba::Black;
                    }();
                const auto mat = g_theRenderer->GetMaterial("city");
                const auto dims = Clay::Vector2ToClayDimensions(Vector2(IntVector2(mat->GetTexture(Material::TextureID::Diffuse)->GetDimensions())));
                CLAY({ .layout = {.sizing = {.width = CLAY_SIZING_FIXED(dims.width), .height = CLAY_SIZING_FIXED(dims.height)}},  .backgroundColor = Clay::RgbaToClayColor(player_color), .image = {.imageData = mat, .sourceDimensions = dims} }) {}
            }
        }
    }
}


void EnemyWaveStatePostwave::RenderCityImageElements() const noexcept {
    const auto player_color = []()->Rgba {
        if (const auto* g = GetGameAs<Game>(); g != nullptr) {
            if (auto* mainState = dynamic_cast<GameStateMain*>(g->GetCurrentState()); mainState != nullptr) {
                return mainState->GetPlayerColor();
            }
            return Rgba::Black;
        }
        return Rgba::Black;
        }();
    const auto mat = g_theRenderer->GetMaterial("city");
    const auto dims = Clay::Vector2ToClayDimensions(Vector2(IntVector2(mat->GetTexture(Material::TextureID::Diffuse)->GetDimensions())));
    std::size_t j = 1u;
    for (std::size_t i = m_citiesRemainingPostWave - (m_citiesRemainingPostWave - j); j <= m_citiesRemainingPostWave; ++i, ++j) {
        CLAY({ .backgroundColor = Clay::RgbaToClayColor(player_color), .image = {.imageData = mat, .sourceDimensions = dims} }) {}
    }
}

void EnemyWaveStatePostwave::RenderMissileImageElements() const noexcept {
    const auto player_color = []()->Rgba {
        if (const auto* g = GetGameAs<Game>(); g != nullptr) {
            if (auto* mainState = dynamic_cast<GameStateMain*>(g->GetCurrentState()); mainState != nullptr) {
                return mainState->GetPlayerColor();
            }
            return Rgba::Black;
        }
        return Rgba::Black;
        }();
    const auto mat = g_theRenderer->GetMaterial("missile");
    const auto dims = Clay::Vector2ToClayDimensions(Vector2(IntVector2(mat->GetTexture(Material::TextureID::Diffuse)->GetDimensions())));
    int j = 1;
    for (int i = m_missilesRemainingPostWave - (m_missilesRemainingPostWave - j); j <= m_missilesRemainingPostWave; ++i, ++j) {
        CLAY({ .backgroundColor = Clay::RgbaToClayColor(player_color), .image = {.imageData = mat, .sourceDimensions = dims} }) {}
    }
}
