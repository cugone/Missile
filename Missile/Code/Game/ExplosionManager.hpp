#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Mesh.hpp"

#include "Game/Explosion.hpp"

#include <vector>

class ExplosionManager {
public:

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    void CreateExplosionAt(Vector2 position, float maxRadius, TimeUtils::FPSeconds ttl) noexcept;

protected:
private:
    std::vector<Explosion> m_explosions{};
    std::vector<Explosion> m_pendingExplosions{};
    mutable Mesh::Builder m_builder{};
};
