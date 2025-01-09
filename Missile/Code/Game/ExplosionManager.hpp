#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Mesh.hpp"

#include "Game/Explosion.hpp"

#include <vector>

class ExplosionManager {
public:
    struct ExplosionData {
        Vector4 position2_radius_ttlSeconds;
    };

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    void CreateExplosionAt(ExplosionData&& newExplosionData) noexcept;

protected:
private:
    mutable Mesh::Builder m_builder{};
    std::vector<Explosion> m_explosions{};
    std::vector<std::size_t> m_deadExplosions{};
};
