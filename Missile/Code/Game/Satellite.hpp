#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Mesh.hpp"

class Satellite {
public:
    Satellite() = default;
    Satellite(const Satellite& other) = default;
    Satellite(Satellite&& other) = default;
    Satellite& operator=(const Satellite& other) = default;
    Satellite& operator=(Satellite&& other) = default;
    ~Satellite() = default;

    explicit Satellite(Vector2 position) noexcept;

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
    Mesh::Builder m_builder{};
    Vector2 m_position{};
    float m_speed{ 100.0f };
    float m_radius{ 25.0f };
    Rgba m_color{};
    int m_health{ 1 };
};
