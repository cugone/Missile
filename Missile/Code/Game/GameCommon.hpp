#pragma once

#include "Engine/Core/FileUtils.hpp"

#include <array>
#include <filesystem>
#include <cstdint>

enum class Faction : std::uint8_t {
    None
    , Player
    , Enemy
    , Max
};

struct GameConstants {
    static inline constexpr const int max_launch_sounds = 2;
    static inline constexpr const int max_explosion_sounds = 5;
    static inline constexpr const int max_score_multiplier = 6;
    static inline constexpr const int enemy_missile_value = 25;
    static inline constexpr const int enemy_satellite_value = 100;
    static inline constexpr const int enemy_bomber_value = 100;
    static inline constexpr const int enemy_smart_bomb_value = 125;
    static inline constexpr const int unused_missile_value = 5;
    static inline constexpr const int saved_city_value = 100;
    static inline constexpr const std::array<int, 12> wave_score_multiplier_lookup{ 1,1,2,2,3,3,4,4,5,5,6,6 };
    static inline constexpr const float radar_line_distance = 100.0f;
    static inline const std::filesystem::path game_config_path = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameConfig) / "game.config";
    static inline const std::filesystem::path game_audio_folder = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{ "Audio" };
};
