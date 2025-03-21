#pragma once

#include "Engine/Core/TimeUtils.hpp"

class EnemyWaveState {
public:
    virtual void OnEnter() noexcept = 0;
    virtual void OnExit() noexcept = 0;
    virtual void BeginFrame() noexcept = 0;
    virtual void Update(TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void Render() const noexcept = 0;
    virtual void DebugRender() const noexcept = 0;
    virtual void EndFrame() noexcept = 0;
    virtual ~EnemyWaveState() noexcept = 0;
protected:
private:
};
