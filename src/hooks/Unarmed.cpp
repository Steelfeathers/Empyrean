#include "Unarmed.h"
#include "Utils/ArmorUtils.h"
#include "Settings/INI/INISettings.h"
#include "Data/Lookup.h"
#include "RE/Offset.h"
#include <xbyak/xbyak.h>

namespace Hooks
{
	bool Unarmed::InstallHooks() {
		logger::info("  Installing Unarmed Hooks..."sv);
		return true;
	}

	void Unarmed::ProcessCombatHit(const RE::HitData& a_hitData)
	{
		const auto aggressor = a_hitData.aggressor.get();
		if (!aggressor || !aggressor->GetIsPlayerOwner())
			return;

		if (!a_hitData.weapon || !a_hitData.weapon->IsHandToHandMelee())
			return;

		if (a_hitData.totalDamage <= 0.0f) return;

		auto giveLightXP = Settings::INI::GetSetting<bool>(Settings::INI::LIGHTARMOR_XP_FROM_UNARMED).value_or(false);
		auto giveHeavyXP = Settings::INI::GetSetting<bool>(Settings::INI::HEAVYARMOR_XP_FROM_UNARMED).value_or(false);
		if (!giveLightXP && !giveHeavyXP) return;

		float xp = a_hitData.totalDamage * "SkillXPFromUnarmedMult"_gv.value_or(0.25f);
		//logger::info(FMT_STRING("  *Unarmed hit! TotalDamage = {}"), a_hitData.totalDamage);

		if (Utils::ArmorUtils::HasEquppedLightArmorGauntlets(aggressor->As<RE::Actor>()))
		{
			if (giveLightXP) RE::PlayerCharacter::GetSingleton()->AddSkillExperience(RE::ActorValue::kLightArmor, xp);
		}
		else if (Utils::ArmorUtils::HasEquppedHeavyArmorGauntlets(aggressor->As<RE::Actor>()))
		{
			if (giveHeavyXP) RE::PlayerCharacter::GetSingleton()->AddSkillExperience(RE::ActorValue::kHeavyArmor, xp);
		}
		
	}
}