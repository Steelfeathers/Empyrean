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
		WeaponSpeedMultPatch();
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

	[[nodiscard]] static RE::TESObjectWEAP* GetUnarmedWeapon()
	{
		static constinit RE::TESObjectWEAP* UnarmedWeapon = nullptr;
		if (!UnarmedWeapon) {
			UnarmedWeapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(0x1F4);
		}
		return UnarmedWeapon;
	}

	using GetCurrentWeapon_t = RE::TESObjectWEAP* (const RE::Actor*, bool);
	template <std::uint64_t ID, std::ptrdiff_t Off>
	struct CallHookImpl<struct WeaponSpeedTag, GetCurrentWeapon_t, ID, Off>
	{
		static RE::TESObjectWEAP* func(const RE::Actor* a_actor, bool a_leftHand)
		{
			auto weapon = util::call_original<&func>(a_actor, a_leftHand);
			if (!weapon) {
				const auto process = a_actor->currentProcess;
				const auto middleHigh = process ? process->middleHigh : nullptr;
				if (middleHigh) {
					const auto object = !a_leftHand ? middleHigh->rightHand : middleHigh->leftHand;
					if (!object) {
						weapon = GetUnarmedWeapon();
					}
				}
			}
			else if (weapon->IsStaff()) {
				// workaround issue where the left punch uses the speed of the staff
				weapon = nullptr;
			}
			return weapon;
		}
	};

	void Unarmed::WeaponSpeedMultPatch()
	{
		// TRAMPOLINE: 28
		util::CallHook<
			struct WeaponSpeedTag,
			GetCurrentWeapon_t,
			RE::Offset::HandleWeaponSpeedChannel.id(),
			0xE>::write5();

		util::CallHook<
			struct WeaponSpeedTag,
			GetCurrentWeapon_t,
			RE::Offset::HandleLeftWeaponSpeedChannel.id(),
			0xE>::write5();
	}
}