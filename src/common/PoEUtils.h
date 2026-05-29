#pragma once

namespace Common
{
	struct PoEUtils
	{
		static std::vector<RE::TESObjectARMO*>* GetEquippedArmor(const RE::Actor* a_actor);
		static int GetCountEquippedLightArmor(const RE::Actor* a_actor, bool allowSubstituteHelmet);
	};
}