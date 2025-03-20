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

namespace GameConstants {
    constexpr const int max_launch_sounds{2};
    constexpr const int max_explosion_sounds{5};
    constexpr const int max_score_multiplier{6};
    constexpr const int max_missile_count{20};
    constexpr const int max_player_missile_count{30};
    constexpr const int max_smartbomb_count{7};
    constexpr const int max_missles_on_screen{4};
    constexpr const int max_cities{6};
    constexpr const float min_missile_impact_time{2.0f};
    constexpr const float min_missile_speed{0.0f};
    constexpr const float min_bomber_cooldown{32.0f};
    constexpr const float min_bomber_firerate{16.0f};
    constexpr const float max_explosion_size{45.0f};
    constexpr const int enemy_missile_value{25};
    constexpr const int enemy_satellite_value{100};
    constexpr const int enemy_bomber_value{100};
    constexpr const int enemy_smart_bomb_value{125};
    constexpr const int unused_missile_value{5};
    constexpr const int saved_city_value{100};
    constexpr const int default_highscore{17000};
    constexpr const std::size_t wave_array_size{20u};
    constexpr std::array<int, wave_array_size> wave_score_multiplier_lookup{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 };
    constexpr std::array<int, wave_array_size> wave_missile_count_lookup{ 12, 15, 18, 12, 16, 14, 17, 10, 13, 16, 19, 12, 14, 16, 18, 14, 16, 18, 20, 20 };
    constexpr std::array<int, wave_array_size> wave_smartbomb_count_lookup{ 0, 0, 0, 0, 0, 1, 1, 2, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 7, 7 };
    constexpr std::array<float, wave_array_size> wave_missile_impact_time{12.0f, 11.0f, 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
    constexpr const std::array wave_flier_cooldown_lookup{36000.0f, 240.0f, 160.0f, 128.0f, 128.0f, 96.0f, 64.0f, 32.0f};
    constexpr const std::array wave_flier_firerate_lookup{36000.0f, 128.0f, 96.0f, 64.0f, 48.0f, 32.0f, 32.0f, 16.0f};
    constexpr const std::array<uint32_t, wave_array_size> wave_object_color_lookup{ uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0x00ff00ffu}, uint32_t{0x00ff00ffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0xff00ffu}, uint32_t{0xff00ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu} };
    constexpr const std::array<uint32_t, wave_array_size> wave_player_color_lookup{ uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu}, uint32_t{0x00ff00ffu}, uint32_t{0x00ff00ffu}, uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0x00ff00ffu}, uint32_t{0x00ff00ffu}, uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu} };
    constexpr const std::array<uint32_t, wave_array_size> wave_background_color_lookup{ uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x000000ffu}, uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu}, uint32_t{0x00ffffffu}, uint32_t{0x00ffffffu}, uint32_t{0xff00ffffu}, uint32_t{0xff00ffffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0xc0c0c0ffu}, uint32_t{0xc0c0c0ffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu} };
    constexpr const std::array<uint32_t, wave_array_size> wave_ground_color_lookup{ uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0x0000ffffu}, uint32_t{0x0000ffffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu}, uint32_t{0x00ff00ffu}, uint32_t{0x00ff00ffu}, uint32_t{0x00ff00ffu}, uint32_t{0x00ff00ffu}, uint32_t{0xff0000ffu}, uint32_t{0xff0000ffu}, uint32_t{0xffff00ffu}, uint32_t{0xffff00ffu} };
    constexpr const float radar_line_distance{100.0f};
    const std::filesystem::path game_config_path{FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameConfig) / std::filesystem::path{"game.config"}};
    const std::filesystem::path game_audio_folder{FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{ "Audio" }};
    const std::filesystem::path game_audio_klaxon_path{game_audio_folder / std::filesystem::path{"Klaxon.wav"}};
    const std::filesystem::path game_audio_bomber_path{game_audio_folder / std::filesystem::path{"Bomber.wav"}};
    const std::filesystem::path game_audio_satellite_path{game_audio_folder / std::filesystem::path{"Satellite.wav"}};
    const std::filesystem::path game_audio_nomissiles_path{game_audio_folder / std::filesystem::path{"NoMissiles.wav"}};
    const std::filesystem::path game_audio_lowmissiles_path{game_audio_folder / std::filesystem::path{"LowMissiles.wav"}};
    const std::filesystem::path game_audio_counting_path{game_audio_folder / std::filesystem::path{"Counting.wav"}};
    const std::filesystem::path game_audio_bonuscity_path{game_audio_folder / std::filesystem::path{"BonusCity.wav"}};
};
