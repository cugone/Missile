#include "Game/Explosion.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Math/Disc2.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/GameCommon.hpp"

#include <format>

Explosion::Explosion(Vector2 position, float maxRadius, TimeUtils::FPSeconds lifetime) noexcept
    : _position{position}
    , _max_radius{maxRadius}
    , _ttl{lifetime}
    , _color{ Rgba::Random() }
{
    //g_theAudioSystem->Play(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / "Audio" / std::format("Explosion{}.wav", idx), AudioSystem::SoundDesc{});
    idx = (idx + 1) % GameConstants::max_explosion_sounds;
}

void Explosion::BeginFrame() noexcept {
    _color = Rgba::Random();
}

void Explosion::Update([[maybe_unused]] TimeUtils::FPSeconds deltaTime) noexcept {
    DoSizeEaseOut();
    _t += deltaTime;
}

void Explosion::AppendToMesh(Mesh::Builder& builder) noexcept {
    builder.Begin(PrimitiveType::Triangles);
    builder.SetColor(_color);

    const auto index_start = builder.verticies.size();
    builder.AddVertex(_position);
    const auto num_sides = 64;
    constexpr const auto anglePerVertex = 360.0f / num_sides;
    for (float degrees = 0.0f; degrees < 360.0f; degrees += anglePerVertex) {
        const auto radians = MathUtils::ConvertDegreesToRadians(degrees);
        const auto pX = _current_radius * std::cos(radians) + _position.x;
        const auto pY = _current_radius * std::sin(radians) + _position.y;
        builder.AddVertex(Vector2{pX, pY});
    }

    builder.indicies.insert(std::end(builder.indicies), static_cast<unsigned int>(num_sides - 1u) * 3u, static_cast<unsigned int>(index_start));
    unsigned int j = static_cast<unsigned int>(index_start) + 1;
    for (std::size_t i = j; i < builder.indicies.size() - 1; i += 3) {
        builder.indicies[i] = (j++) % num_sides;
        builder.indicies[i + 1] = (j == num_sides ? index_start + 1 : j) % num_sides;
    }
    builder.End(g_theRenderer->GetMaterial("__2D"));
}

void Explosion::EndFrame() noexcept {
    /* DO NOTHING */
}

bool Explosion::IsDead() const noexcept {
    return MathUtils::IsEquivalent(_t.count(), _ttl.count());
}

Disc2 Explosion::GetCollisionMesh() const noexcept {
    return Disc2{_position, _current_radius};
}

Rgba Explosion::GetColor() const noexcept {
    return _color;
}

void Explosion::DoSizeEaseOut() noexcept {
    if (_t < TimeUtils::FPSeconds::zero()) {
        _t = TimeUtils::FPSeconds::zero();
    }
    if (_t > _ttl) {
        _t = _ttl;
    }
    const auto delta = _t / _ttl;
    _current_radius = _max_radius * MathUtils::EasingFunctions::SmoothStop<1>(MathUtils::EasingFunctions::Arc<5>(delta));
}
