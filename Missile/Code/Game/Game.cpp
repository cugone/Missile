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

void Game::ChangeState(std::unique_ptr<GameState> newState) noexcept {
    m_nextState = std::move(newState);
}

void Game::Initialize() noexcept {
    LoadOrCreateConfigFile();
    g_theRenderer->SetVSync(true);
    g_theRenderer->RegisterMaterialsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameMaterials));
    g_theRenderer->RegisterFontsFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameFonts));

    ChangeState(std::move(std::make_unique<GameStateMain>()));

}

void Game::BeginFrame() noexcept {
    if(m_nextState) {
        m_currentState->OnExit();
        m_currentState = std::move(m_nextState);
        m_currentState->OnEnter();
        m_nextState.reset(nullptr);
    }
    m_currentState->BeginFrame();
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    g_theRenderer->UpdateGameTime(deltaSeconds);
    m_currentState->Update(deltaSeconds);
}

void Game::Render() const noexcept {
    m_currentState->Render();
}

void Game::EndFrame() noexcept {
    m_currentState->EndFrame();
}

const Player* Game::GetPlayerData() const noexcept {
    return &m_playerData;
}

Player* Game::GetPlayerData() noexcept {
    return &m_playerData;
}

int Game::GetPlayerScore() const noexcept {
    return GetPlayerData()->score;
}

void Game::AdjustPlayerScore(int scoreToAdd) noexcept {
    m_playerData.score += scoreToAdd;
    m_playerData.scoreRemainingForBonusCity -= scoreToAdd;
    if(m_playerData.scoreRemainingForBonusCity <= 0) {
        if (auto* main_state = dynamic_cast<GameStateMain*>(this->GetCurrentState()); main_state != nullptr) {
            m_playerData.scoreRemainingForBonusCity = MathUtils::Wrap(m_playerData.scoreRemainingForBonusCity, 0, 10000);
            main_state->GetCityManager().GrantBonusCIty();
        }
    }
}

int Game::GetHighScore() const noexcept {
    return m_currentHighScore;
}

void Game::UpdateHighScore() noexcept {
    m_currentHighScore = (std::max)(GetPlayerScore(), GetHighScore());
}

GameState* const Game::GetCurrentState() const noexcept {
    return m_currentState.get();
}

const GameSettings* Game::GetSettings() const noexcept {
    return &m_mySettings;
}

GameSettings* Game::GetSettings() noexcept {
    return &m_mySettings;
}
