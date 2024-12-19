#pragma once

#include "Engine/Core/TimeUtils.hpp"

class Missile {
public:
    Missile() = default;
    Missile(const Missile& other) = default;
    Missile(Missile&& other) = default;
    Missile& operator=(const Missile& other) = default;
    Missile& operator=(Missile&& other) = default;
    ~Missile() = default;

    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaTime) noexcept;
    void Render() const;
    void EndFrame() noexcept;

protected:
private:
};
