#include "PlayerMovement.h"
#include "Data/Lookup.h"
#include "Data/ModObjectManager.h"
#include "RE/Offset.h"
#include <xbyak/xbyak.h>
#include <Windows.h>

namespace Hooks
{
	bool PlayerMovement::InstallHooks() {
		logger::info("  Installing PlayerMovement Hooks..."sv);
		ExperiencePatch();
		SprintingCostPatch();
		return true;
	}

	void PlayerMovement::ExperiencePatch()
	{
		auto vtbl = REL::Relocation<std::uintptr_t>(RE::Offset::PlayerCharacter::Vtbl);
		_Update = vtbl.write_vfunc(173, &PlayerMovement::Update);
	}

	void PlayerMovement::Update(RE::PlayerCharacter* a_player, float a_delta)
	{
		_Update(a_player, a_delta);
		if (a_player->IsOnMount()) { return; }
		if (PlayerMovement::GetCountEquippedLightArmor(a_player) < 2) { return; }

		if (a_player->IsSprinting() || a_player->IsRunning()) {
			float skillUse = a_delta * 5.0f;
			a_player->AddSkillExperience(RE::ActorValue::kLightArmor, skillUse);
		}
	}

	int PlayerMovement::GetCountEquippedLightArmor(const RE::Actor* a_actor)
	{
		if (!a_actor) return 0;

		const auto invChanges = const_cast<RE::Actor*>(a_actor)->GetInventoryChanges();
		if (!invChanges) {
			return 0;
		}

		const auto invLists = invChanges->entryList;
		if (!invLists || invLists->empty()) {
			return 0;
		}

		int count = 0;
		const auto kArmorLightHelmet = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightHelmet"sv);
		const auto kArmorLightCuirass = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightCuirass"sv);
		const auto kArmorLightGauntlets = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightGauntlets"sv);
		const auto kArmorLightBoots = Data::ModObject<RE::BGSKeyword>("KeywordArmorLightBoots"sv);

		for (const auto& entry : *invLists) {
			const auto obj = entry ? entry->object : nullptr;
			if (!obj) { continue; }
			const auto xLists = entry->extraLists;
			if (!xLists) { continue; }

			const auto armor = obj->As<RE::TESObjectARMO>();
			if (!armor) { continue; }
			if (!armor->IsLightArmor()) { continue; }

			bool worn = false;
			for (const auto& xList : *xLists) {
				if (xList && xList->HasType<RE::ExtraWorn>()) {
					worn = true;
					break;
				}
			}
			if (!worn) { continue; }
			if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kForearms) || armor->HasKeyword(kArmorLightGauntlets))
			{
				count += 1;
				continue;
			}
			if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) || armor->HasKeyword(kArmorLightCuirass))
			{
				count += 1;
				continue;
			}
			if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kCalves) || armor->HasKeyword(kArmorLightBoots))
			{
				count += 1;
				continue;
			}
			if (armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHead) || armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair) || armor->HasKeyword(kArmorLightHelmet) )
			{
				count += 1;
				continue;
			}
		}

		return count;
	}

	void PlayerMovement::SprintingCostPatch()
	{
		auto hook = REL::Relocation<std::uintptr_t>(RE::Offset::Actor::UpdateSprinting, 0xCE);
		REL::make_pattern<"0F 57 05">().match_or_fail(hook.address());

		// TRAMPOLINE: 17
		struct Patch : Xbyak::CodeGenerator
		{
			Patch() : Xbyak::CodeGenerator(17, SKSE::GetTrampoline().allocate(17))
			{
				mov(rdx, rdi);
				jmp(ptr[rip]);
				dq(std::bit_cast<std::uintptr_t>(&PlayerMovement::CalcSprintingStaminaMod));
			}
		};

		auto& trampoline = SKSE::GetTrampoline();
		REL::safe_fill(hook.address(), REL::NOP, 0x7);
		trampoline.write_call<5>(hook.address(), Patch().getCode());
	}

	float PlayerMovement::CalcSprintingStaminaMod(float a_cost, const RE::Actor* a_actor)
	{
		float cost = a_cost;
		PlayerMovement::HandleSprintingCost(a_actor, cost);
		cost = (std::max)(cost, 0.0f);

		// xorps xmm0, -1.0
		return -cost;
	}

	void PlayerMovement::HandleSprintingCost(const RE::Actor* a_actor, float& a_cost)
	{
		if (const auto perk = Data::ModObject<RE::BGSPerk>("PerkSprintReduceCost"sv);
			perk && a_actor->HasPerk(perk)) {
		float costMult = "SprintReduceCostMult"_gv.value_or(0.5f);
		a_cost *= costMult;
		}
	}
}