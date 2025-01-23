#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"

class Bomber {
public:
    Bomber() = default;
    Bomber(const Bomber& other) = default;
    Bomber(Bomber&& other) = default;
    Bomber& operator=(const Bomber& other) = default;
    Bomber& operator=(Bomber&& other) = default;
    ~Bomber() = default;

    explicit Bomber(Vector2 position) noexcept;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    void Kill() noexcept;
    bool IsDead() const noexcept;

    Disc2 GetCollisionMesh() const noexcept;

    Vector2 GetPosition() const noexcept;
    void SetColor(Rgba newColor) noexcept;

protected:
private:
    Vector2 m_position{};
    float m_speed{100.0f};
    Rgba m_color;
    int m_health{1};
};
