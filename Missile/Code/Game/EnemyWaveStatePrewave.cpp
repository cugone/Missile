#include "Game/EnemyWaveStatePrewave.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"
#include "Game/GameState.hpp"
#include "Game/GameStateMain.hpp"

#include "Game/EnemyWave.hpp"
#include "Game/EnemyWaveStateActive.hpp"

#include <format>

EnemyWaveStatePrewave::EnemyWaveStatePrewave(EnemyWave* context) noexcept
    : m_context(context)
{
    /* DO NOTHING */
}

void EnemyWaveStatePrewave::OnEnter() noexcept {
    m_context->DeactivateWave();
    m_preWaveTimer.Reset();
    g_theUISystem->SetClayLayoutCallback([this]() { this->ClayPrewave(); });
}

void EnemyWaveStatePrewave::OnExit() noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePrewave::BeginFrame() noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePrewave::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePrewave::Render() const noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePrewave::DebugRender() const noexcept {
    /* DO NOTHING */
}

void EnemyWaveStatePrewave::EndFrame() noexcept {
    if (m_preWaveTimer.CheckAndReset()) {
        auto* g = GetGameAs<Game>();
        auto* state = dynamic_cast<GameStateMain*>(g->GetCurrentState());
        state->ResetMissileCount();
        m_context->ChangeState(std::make_unique<EnemyWaveStateActive>(m_context));
    }
}

static Clay_LayoutConfig fullscreen_layout = {
    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
    .padding = CLAY_PADDING_ALL(0),
    .childGap = 0,
    .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_TOP},
    .layoutDirection = Clay_LayoutDirection::CLAY_TOP_TO_BOTTOM,
};

void EnemyWaveStatePrewave::ClayPrewave() noexcept {
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = fullscreen_layout, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        RenderScoreElement();
        RenderScoreMultiplierElement();
    }
}

void EnemyWaveStatePrewave::RenderScoreElement() const noexcept {
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

void EnemyWaveStatePrewave::RenderScoreMultiplierElement() const noexcept {
    CLAY({ .id = CLAY_ID("ScoreMultiplier"), .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(0), .childAlignment = {.x = Clay_LayoutAlignmentX::CLAY_ALIGN_X_CENTER, .y = Clay_LayoutAlignmentY::CLAY_ALIGN_Y_CENTER}}, .backgroundColor = Clay::RgbaToClayColor(Rgba::NoAlpha) }) {
        static auto points_str = std::string{};
        points_str = std::format("{} X POINTS", m_context->GetScoreMultiplier());
        Clay_TextElementConfig textConfig{};
        textConfig.userData = g_theRenderer->GetFont("System32");
        textConfig.textColor = Clay::RgbaToClayColor(m_context->GetObjectColor());
        CLAY_TEXT(Clay::StrToClayString(points_str), CLAY_TEXT_CONFIG(textConfig));
    }
}
