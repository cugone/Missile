#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Game/City.hpp"

#include <array>

class CityManager {
public:

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    bool IsBonusCityAvailable() const noexcept;
    void AddBonusCity() noexcept;

    std::size_t RemainingCities() const noexcept;

    const City& GetCity(std::size_t index) const noexcept;
    City& GetCity(std::size_t index) noexcept;

protected:
private:
    std::array<City, 6> m_cities{};
    int m_bonusCities{0};
};
