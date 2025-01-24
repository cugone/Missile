#pragma once

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Rgba.hpp"

#include <array>
#include <filesystem>
#include <cstdint>

enum class Faction : std::uint8_t {
    None
    , Player
    , Enemy
    , Max
};

static const std::array<Rgba, 20>& get_object_color_lookup() noexcept {
    static const std::array<Rgba, 20> object_color{ Rgba::Red, Rgba::Red, Rgba::Green, Rgba::Green, Rgba::Red, Rgba::Red, Rgba::Yellow, Rgba::Yellow, Rgba::Red, Rgba::Red, Rgba::Red, Rgba::Red, Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Magenta, Rgba::Magenta, Rgba::Black, Rgba::Black };
    return object_color;
}

static const std::array<Rgba, 20>& get_player_color_lookup() noexcept {
    static const std::array<Rgba, 20> player_color{ Rgba::Blue, Rgba::Blue, Rgba::Blue, Rgba::Blue, Rgba::Green, Rgba::Green, Rgba::Blue, Rgba::Blue, Rgba::Black, Rgba::Black, Rgba::Blue, Rgba::Blue, Rgba::Yellow, Rgba::Yellow, Rgba::Red, Rgba::Red, Rgba::Green, Rgba::Green, Rgba::Blue, Rgba::Blue };
    return player_color;
}

static const std::array<Rgba, 20>& get_background_color_lookup() noexcept {
    static const std::array<Rgba, 20> player_color{ Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Black, Rgba::Blue, Rgba::Blue, Rgba::Cyan, Rgba::Cyan, Rgba::Magenta, Rgba::Magenta, Rgba::Yellow, Rgba::Yellow, Rgba::LightGrey, Rgba::LightGrey, Rgba::Red, Rgba::Red };
    return player_color;
}

static const std::array<Rgba, 20>& get_ground_color_lookup() noexcept {
    static const std::array<Rgba, 20> player_color{ Rgba::Yellow, Rgba::Yellow, Rgba::Yellow, Rgba::Yellow, Rgba::Blue, Rgba::Blue, Rgba::Red, Rgba::Red, Rgba::Yellow, Rgba::Yellow, Rgba::Yellow, Rgba::Yellow, Rgba::Green, Rgba::Green, Rgba::Green, Rgba::Green, Rgba::Red, Rgba::Red, Rgba::Yellow, Rgba::Yellow };
    return player_color;
}

struct GameConstants {
    static inline constexpr const int max_launch_sounds{2};
    static inline constexpr const int max_explosion_sounds{5};
    static inline constexpr const int max_score_multiplier{6};
    static inline constexpr const int enemy_missile_value{25};
    static inline constexpr const int enemy_satellite_value{100};
    static inline constexpr const int enemy_bomber_value{100};
    static inline constexpr const int enemy_smart_bomb_value{125};
    static inline constexpr const int unused_missile_value{5};
    static inline constexpr const int saved_city_value{100};
    static inline constexpr const int default_highscore{17000};
    static inline constexpr const std::size_t wave_array_size{20u};
    static inline constexpr const std::array<int, wave_array_size> wave_score_multiplier_lookup{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 };
    static inline const std::array<Rgba, wave_array_size> wave_object_color_lookup{ get_object_color_lookup() };
    static inline const std::array<Rgba, wave_array_size> wave_player_color_lookup{ get_player_color_lookup() };
    static inline const std::array<Rgba, wave_array_size> wave_background_color_lookup{ get_background_color_lookup() };
    static inline const std::array<Rgba, wave_array_size> wave_ground_color_lookup{ get_ground_color_lookup() };
    static inline constexpr const float radar_line_distance{100.0f};
    static inline const std::filesystem::path game_config_path{FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameConfig) / std::filesystem::path{"game.config"}};
    static inline const std::filesystem::path game_audio_folder{FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{ "Audio" }};
    static inline const std::filesystem::path game_audio_klaxon_path{game_audio_folder / std::filesystem::path{"Klaxon.wav"}};
    static inline const std::filesystem::path game_audio_bomber_path{game_audio_folder / std::filesystem::path{"Bomber.wav"}};
    static inline const std::filesystem::path game_audio_satellite_path{game_audio_folder / std::filesystem::path{"Satellite.wav"}};
    static inline const std::filesystem::path game_audio_nomissiles_path{game_audio_folder / std::filesystem::path{"NoMissiles.wav"}};
    static inline const std::filesystem::path game_audio_lowmissiles_path{game_audio_folder / std::filesystem::path{"LowMissiles.wav"}};
};
