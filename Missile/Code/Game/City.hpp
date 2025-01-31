#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
class City {
public:

    City() = default;
    City(const City& other) = default;
    City(City&& other) = default;
    City& operator=(const City& other) = default;
    City& operator=(City&& other) = default;
    ~City() = default;

    explicit City(Vector2 position) noexcept;

    void SetPosition(Vector2 position) noexcept;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    AABB2 GetCollisionMesh() const noexcept;

    bool IsDead() const noexcept;
    void Kill() noexcept;

protected:
private:

    Rgba GetCityColor() const noexcept;

    Vector2 m_position{};
    int m_health{1};
};
