#pragma once

namespace Hooks
{
	struct LightArmor
	{
		static bool InstallHooks();

		static void ProcessUpdate(RE::PlayerCharacter* a_player, float a_delta);

		static void InstallSprintingCostHook();
		static float CalcSprintingStaminaMod(float a_cost, const RE::Actor* a_actor);
	};
}