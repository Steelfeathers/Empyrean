#pragma once

namespace Hooks
{
	struct LightArmor
	{
		static bool InstallHooks();

		static void HandleUpdate(RE::PlayerCharacter* a_player, float a_delta);

		static void HandleSprintingCost(const RE::Actor* a_actor, float& a_cost);
		static void InstallSprintingCostHook();
		static float CalcSprintingStaminaMod(float a_cost, const RE::Actor* a_actor);
	};
}