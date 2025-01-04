#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Mesh.hpp"

class Explosion {
public:
    Explosion() = default;
    Explosion(const Explosion& other) = default;
    Explosion(Explosion&& other) = default;
    Explosion& operator=(const Explosion& other) = default;
    Explosion& operator=(Explosion&& other) = default;
    ~Explosion() = default;

    Explosion(Vector2 position, float maxRadius, TimeUtils::FPSeconds lifetime) noexcept;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaTime) noexcept;
    void AppendToMesh(Mesh::Builder& builder) noexcept;
    void EndFrame() noexcept;

    bool IsDead() const noexcept;
    Disc2 GetCollisionMesh() const noexcept;

protected:
private:

    void DoSizeEaseOut(TimeUtils::FPSeconds deltaTime) noexcept;

    Vector2 _position{};
    float _max_radius{};
    float _current_radius{};
    TimeUtils::FPSeconds _t{TimeUtils::FPSeconds::zero()};
    TimeUtils::FPSeconds _ttl{1.0f};
    static inline int idx{0};
    Rgba _color{};
};
