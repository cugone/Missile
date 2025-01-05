#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Game/MissileManager.hpp"

class MissileBase {
public:
    void BeginFrame() noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;
protected:
private:
    MissileManager m_missileManager{};
    int m_maxMissiles{10};
};
