#pragma once

namespace Hooks
{
	struct PlayerMovement
	{
		static bool InstallHooks();
		static void HandleSprintingCost(const RE::Actor* a_actor, float& a_cost);
		static void SprintingCostPatch();
		static float CalcSprintingStaminaMod(float a_cost, const RE::Actor* a_actor);

	};
}