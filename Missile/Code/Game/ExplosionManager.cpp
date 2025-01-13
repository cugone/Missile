#include "Game/ExplosionManager.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <algorithm>

void ExplosionManager::BeginFrame() noexcept {
    for (auto& e : m_explosions) {
        e.BeginFrame();
    }
}

void ExplosionManager::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    for (auto& e : m_explosions) {
        e.Update(deltaSeconds);
    }
    for (std::size_t i = 0u; i < m_explosions.size(); ++i) {
        if (m_explosions[i].IsDead()) {
            m_deadExplosions.push_back(i);
        }
    }
}

void ExplosionManager::Render() const noexcept {
    g_theRenderer->SetModelMatrix();
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    for (const auto& e : m_explosions) {
        g_theRenderer->DrawFilledCircle2D(e.GetCollisionMesh().center, e.GetCollisionMesh().radius, e.GetColor());
    }
}

void ExplosionManager::EndFrame() noexcept {
    for (auto& e : m_explosions) {
        e.EndFrame();
    }
    std::sort(std::begin(m_deadExplosions), std::end(m_deadExplosions), std::greater<std::size_t>());
    for (const auto& i : m_deadExplosions) {
        std::swap(m_explosions[i], m_explosions.back());
        m_explosions.pop_back();
    }
    m_deadExplosions.clear();
}

void ExplosionManager::CreateExplosionAt(ExplosionData&& newExplosionData) noexcept {
    m_explosions.emplace_back(newExplosionData.position2_radius_ttlSeconds.GetXY(), newExplosionData.position2_radius_ttlSeconds.z, TimeUtils::FPSeconds{ newExplosionData.position2_radius_ttlSeconds.w}, newExplosionData.faction);
}

std::vector<Disc2> ExplosionManager::GetExplosionCollisionMeshes() const noexcept {
    std::vector<Disc2> results;
    results.reserve(m_explosions.size());
    for(const auto& e : m_explosions) {
        results.push_back(e.GetCollisionMesh());
    }
    return results;
}

