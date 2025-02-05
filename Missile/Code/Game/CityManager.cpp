#include "Game/CityManager.hpp"

#include <algorithm>

void CityManager::BeginFrame() noexcept {
    for(auto& city : m_cities) {
        city.BeginFrame();
    }
}

void CityManager::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    for (auto& city : m_cities) {
        city.Update(deltaSeconds);
    }
}

void CityManager::Render() const noexcept {
    for (auto& city : m_cities) {
        city.Render();
    }
}

void CityManager::EndFrame() noexcept {
    for (auto& city : m_cities) {
        city.EndFrame();
    }
}

bool CityManager::IsBonusCityAvailable() const noexcept {
    return m_bonusCities > 0;
}

void CityManager::GrantBonusCIty() noexcept {
    m_bonusCities += 1;
}

std::size_t CityManager::RemainingCities() const noexcept {
    return std::count_if(std::cbegin(m_cities), std::cend(m_cities), [](const City& city) { return !city.IsDead(); });
}

const City& CityManager::GetCity(std::size_t index) const noexcept {
    return m_cities[index];
}

City& CityManager::GetCity(std::size_t index) noexcept {
    return m_cities[index];
}
