#include "Game/ExplosionManager.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <algorithm>

void ExplosionManager::BeginFrame() noexcept {
    for (auto& e : m_explosions) {
        e.BeginFrame();
    }
    m_builder.Clear();
}

void ExplosionManager::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    for (auto& e : m_explosions) {
        e.Update(deltaSeconds);
    }
    for (auto& e : m_explosions) {
        e.AppendToMesh(m_builder);
    }
}

void ExplosionManager::Render() const noexcept {
    g_theRenderer->SetModelMatrix();
    Mesh::Render(m_builder);
}

void ExplosionManager::EndFrame() noexcept {
    for (auto& e : m_explosions) {
        e.EndFrame();
    }
    m_explosions.erase(std::remove_if(std::begin(m_explosions), std::end(m_explosions), [](const Explosion& e) { return e.IsDead(); }), std::end(m_explosions));
    m_explosions.reserve(m_explosions.size() + m_pendingExplosions.size());
    m_explosions.insert(std::end(m_explosions), std::begin(m_pendingExplosions), std::end(m_pendingExplosions));
    m_pendingExplosions.clear();
}

void ExplosionManager::CreateExplosionAt(Vector2 position, float maxRadius, TimeUtils::FPSeconds ttl) noexcept {
    m_pendingExplosions.emplace_back(position, maxRadius, ttl);
}

