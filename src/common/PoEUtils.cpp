#include "PoEUtils.h"
#include "Data/Lookup.h"
#include "Data/ModObjectManager.h"

namespace Common
{
	std::vector<RE::TESObjectARMO*>* PoEUtils::GetEquippedArmor(const RE::Actor* a_actor)
	{
		if (!a_actor) return {};

		const auto invChanges = const_cast<RE::Actor*>(a_actor)->GetInventoryChanges();
		if (!invChanges) {
			return {};
		}

		const auto armorList = new std::vector<RE::TESObjectARMO*>{};

		const auto invLists = invChanges->entryList;
		if (!invLists || invLists->empty()) {
			return {};
		}

		for (const auto& entry : *invLists) {
			const auto obj = entry ? entry->object : nullptr;
			if (!obj) { continue; }
			const auto xLists = entry->extraLists;
			if (!xLists) { continue; }

			const auto armor = obj->As<RE::TESObjectARMO>();
			if (!armor) { continue; }

			bool worn = false;
			for (const auto& xList : *xLists) {
				if (xList && xList->HasType<RE::ExtraWorn>()) {
					worn = true;
					break;
				}
			}
			if (!worn) { continue; }
			//if (armor->bipedModelData.armorType == RE::BIPED_MODEL::ArmorType::kClothing) { continue; }
			armorList->push_back(armor);
		}

		return armorList;
	}

	int PoEUtils::GetCountEquippedLightArmor(const RE::Actor* a_actor, bool allowSubstituteHelmet)
	{
		if (!a_actor) return 0;

		const auto armorList = GetEquippedArmor(a_actor);
		if (armorList->empty()) return 0;

		int count = 0;
		const auto perkSubstituteHelmet = Data::ModObject<RE::BGSPerk>("PerkSubstituteHelmetLight"sv);
		const auto kArmorSubstituteHelmet = Data::ModObject<RE::BGSKeyword>("KeywordSubstituteHelmet"sv);
		const auto kArmorLightHelmet = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightHelmet"sv);
		const auto kArmorLightCuirass = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightCuirass"sv);
		const auto kArmorLightGauntlets = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightGauntlets"sv);
		const auto kArmorLightBoots = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightBoots"sv);

		for (const auto& armor : *armorList) 
		{	
			if (armor->IsLightArmor() && (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kForearms) || armor->HasKeyword(kArmorLightGauntlets)))
			{
				count += 1;
				continue;
			}
			if (armor->IsLightArmor() && (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) || armor->HasKeyword(kArmorLightCuirass)))
			{
				count += 1;
				continue;
			}
			if (armor->IsLightArmor() && (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kCalves) || armor->HasKeyword(kArmorLightBoots)))
			{
				count += 1;
				continue;
			}
			if ((armor->IsLightArmor() && (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHead) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair) || armor->HasKeyword(kArmorLightHelmet))) ||
				(allowSubstituteHelmet && a_actor->HasPerk(perkSubstituteHelmet) && armor->HasKeyword(kArmorSubstituteHelmet)))
			{
				count += 1;
				continue;
			}
		}

		return count;
	}

	bool PoEUtils::HasEquppedLightArmorGauntlets(const RE::Actor* a_actor)
	{
		if (!a_actor) return false;

		const auto armorList = GetEquippedArmor(a_actor);
		if (armorList->empty()) return false;

		const auto kArmorLightGauntlets = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightGauntlets"sv);

		for (const auto& armor : *armorList)
		{
			if (armor->IsLightArmor() && (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kForearms) || armor->HasKeyword(kArmorLightGauntlets)))
			{
				return true;
			}
		}
		return false;
	}
}