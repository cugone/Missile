#include "Game/MissileManager.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <algorithm>

void MissileManager::BeginFrame() noexcept {
    m_builder.Clear();
    for (auto& m : m_missiles) {
        m.BeginFrame();
    }
}

void MissileManager::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    for (auto& m : m_missiles) {
        m.Update(deltaSeconds);
    }
    for (auto& m : m_missiles) {
        m.AppendToMesh(m_builder);
    }
}

void MissileManager::Render() const noexcept {
    g_theRenderer->SetModelMatrix();
    Mesh::Render(m_builder);
}

void MissileManager::EndFrame() noexcept {
    for (auto& m : m_missiles) {
        m.EndFrame();
    }
    m_missiles.erase(std::remove_if(std::begin(m_missiles), std::end(m_missiles), [](const Missile& m) { return m.IsDead(); }), std::end(m_missiles));
}

void MissileManager::LaunchMissile(Vector2 position, Direction direction, TimeUtils::FPSeconds timeToTarget) noexcept {
    LaunchMissile(position, Target{position + direction.value * 1000.0f}, timeToTarget);
}

void MissileManager::LaunchMissile(Vector2 position, Target target, TimeUtils::FPSeconds timeToTarget) noexcept {
    if (m_missileFired) {
        m_missileFired = false;
        m_missiles.push_back(Missile{ position, target.value, timeToTarget });
    }
}

void MissileManager::FireMissile() noexcept {
    m_missileFired = true;
}
