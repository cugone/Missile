#include "Game/Explosion.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Math/Disc2.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Game/GameCommon.hpp"

#include <format>

Explosion::Explosion(Vector2 position, float maxRadius, TimeUtils::FPSeconds lifetime) noexcept
    : _position{position}
    , _max_radius{maxRadius}
    , _ttl{lifetime / 2.0f}
    , _color{ Rgba::Random() }
{
    g_theAudioSystem->Play(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / "Audio" / std::format("Explosion{}.wav", idx), AudioSystem::SoundDesc{});
    idx = (idx + 1) % max_explosion_sounds;
}

void Explosion::BeginFrame() noexcept {
    _color = Rgba::Random();
}

void Explosion::Update([[maybe_unused]] TimeUtils::FPSeconds deltaTime) noexcept {
    _current_radius = _max_radius * MathUtils::SineWave(TimeUtils::FPSeconds{TimeUtils::GetCurrentTimeElapsed()} / _ttl);
}

void Explosion::AppendToMesh(Mesh::Builder& builder) noexcept {
    builder.Begin(PrimitiveType::Triangles);
    builder.SetColor(_color);

    builder.AddVertex(_position);
    const auto num_sides = 64;
    constexpr const auto anglePerVertex = 360.0f / num_sides;
    for (float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        const auto radians = MathUtils::ConvertDegreesToRadians(degrees);
        const auto pX = _current_radius * std::cos(radians) + _position.x;
        const auto pY = _current_radius * std::sin(radians) + _position.y;
        builder.AddVertex(Vector2{pX, pY});
    }


    builder.indicies = std::vector<unsigned int>((num_sides - 1) * 3);
    unsigned int j = 1u;
    for (std::size_t i = 1; i < builder.indicies.size() - 1; i += 3) {
        builder.indicies[i] = (j++) % num_sides;
        builder.indicies[i + 1] = (j == num_sides ? 1 : j) % num_sides;
    }
    builder.End(g_theRenderer->GetMaterial("__2D"));
}

void Explosion::EndFrame() noexcept {
    /* DO NOTHING */
}

bool Explosion::IsDead() const noexcept {
    return _ttl == TimeUtils::FPSeconds::zero();
}
