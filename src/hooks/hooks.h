#pragma once

namespace Hooks {
	bool Install();

	static void InstallUpdateHook();
	static void Update(RE::PlayerCharacter* a_player, float a_delta);
	inline static REL::Relocation<decltype(&Update)> _Update;
}