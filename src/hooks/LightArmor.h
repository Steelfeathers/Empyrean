#pragma once

namespace Hooks
{
	class LightArmor final
	{
	public:
		static bool InstallHooks();
		static void ProcessUpdate(RE::PlayerCharacter* a_player, float a_delta);

	private:
		static void InstallSprintingCostHook();
		static float CalcSprintingStaminaMod(float a_cost, const RE::Actor* a_actor);
	};
}