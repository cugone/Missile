#pragma once

#include "Game/GameState.hpp"

class GameStateTitle : public GameState {
public:
    virtual ~GameStateTitle() = default;
    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;
protected:
private:
    
};
