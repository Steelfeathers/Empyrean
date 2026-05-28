#pragma once

namespace Hooks
{
	struct PlayerMovement
	{
		static bool InstallHooks();

		static void InstallUpdateHook();
		static void Update(RE::PlayerCharacter* a_player, float a_delta);
		inline static REL::Relocation<decltype(&Update)> _Update;
		static int GetCountEquippedLightArmor(const RE::Actor* a_actor, bool allowSubstituteHelmet);

		static void HandleSprintingCost(const RE::Actor* a_actor, float& a_cost);
		static void InstallSprintingCostHook();
		static float CalcSprintingStaminaMod(float a_cost, const RE::Actor* a_actor);
		//static bool HasEquippedLightArmorBoots(const RE::Actor* a_actor);
	};
}