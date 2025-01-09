#pragma once

#include "Engine/Core/FileUtils.hpp"

struct GameConstants {
    static inline constexpr const int max_launch_sounds = 2;
    static inline constexpr const int max_explosion_sounds = 5;
    static inline const std::filesystem::path game_config_path = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameConfig) / "game.config";
};
