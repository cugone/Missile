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
    for (std::size_t i = 0u; i < m_missiles.size(); ++i) {
        if (m_missiles[i].IsDead()) {
            m_deadMissiles.push_back(i);
        }
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
    std::sort(std::begin(m_deadMissiles), std::end(m_deadMissiles), std::greater<std::size_t>());
    for (const auto& i : m_deadMissiles) {
        std::swap(m_missiles[i], m_missiles.back());
        m_missiles.pop_back();
    }
    m_deadMissiles.clear();
}

void MissileManager::LaunchMissile(Vector2 position, Direction direction, TimeUtils::FPSeconds timeToTarget) noexcept {
    LaunchMissile(position, Target{position + direction.value * 1000.0f}, timeToTarget);
}

void MissileManager::LaunchMissile(Vector2 position, Target target, TimeUtils::FPSeconds timeToTarget) noexcept {
    if (m_missileFired) {
        m_missileFired = false;
        m_missiles.emplace_back( position, target.value, timeToTarget );
    }
}

void MissileManager::FireMissile() noexcept {
    m_missileFired = true;
}
