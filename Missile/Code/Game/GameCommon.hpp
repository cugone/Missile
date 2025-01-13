#pragma once

#include "Engine/Core/FileUtils.hpp"

enum class Faction : uint8_t {
    None
    , Player
    , Enemy
    , Max
};

struct GameConstants {
    static inline constexpr const int max_launch_sounds = 2;
    static inline constexpr const int max_explosion_sounds = 5;
    static inline const std::filesystem::path game_config_path = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameConfig) / "game.config";
    static inline const std::filesystem::path game_audio_folder = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{ "Audio" };
};
