#include "Game/GameStateTitle.hpp"

#include "Engine/Core/BuildConfig.hpp"

#ifdef PROFILE_BUILD
#include <Thirdparty/Tracy/tracy/Tracy.hpp>
#endif

void GameStateTitle::OnEnter() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
}

void GameStateTitle::OnExit() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
}

void GameStateTitle::BeginFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
}

void GameStateTitle::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
}

void GameStateTitle::Render() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
}

void GameStateTitle::EndFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScoped;
#endif
}

