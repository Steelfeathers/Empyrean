#include "LightArmor.h"
#include "Common/PoEUtils.h"
#include "Data/Lookup.h"
#include "Data/ModObjectManager.h"
#include "RE/Offset.h"
#include <xbyak/xbyak.h>
#include <Windows.h>

namespace Hooks
{
	bool LightArmor::InstallHooks() {
		logger::info("  Installing LightArmor Hooks..."sv);
		InstallSprintingCostHook();
		return true;
	}

	void LightArmor::HandleUpdate(RE::PlayerCharacter* a_player, float a_delta)
	{
		//Give player LightArmor XP if moving while wearing at least 2 pieces of light armor; more XP during combat
		if (a_player->IsOnMount()) { return; }
		int armorCount = Common::PoEUtils::GetCountEquippedLightArmor(a_player, false);
		if (armorCount < 2) { return; }

		if (a_player->IsSprinting() || a_player->IsRunning()) {
			float baseRate = "SkillXPLightAmorBaseRate"_gv.value_or(0.75f);
			float armorCountAdd = "SkillXPLightAmorGearCountAdd"_gv.value_or(0.25f) * (armorCount - 2);
			float combatMult = a_player->IsInCombat() ? "SkillXPLightAmorCombatMult"_gv.value_or(2.0f) : 1.0f;
			float sprintMult = a_player->IsSprinting() ? "SkillXPLightAmorSprintMult"_gv.value_or(1.5f) : 1.0f;
			float skillUse = a_delta * (baseRate + armorCountAdd) * combatMult * sprintMult;
			a_player->AddSkillExperience(RE::ActorValue::kLightArmor, skillUse);
		}
	}

	void LightArmor::InstallSprintingCostHook()
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
				dq(std::bit_cast<std::uintptr_t>(&LightArmor::CalcSprintingStaminaMod));
			}
		};

		auto& trampoline = SKSE::GetTrampoline();
		REL::safe_fill(hook.address(), REL::NOP, 0x7);
		trampoline.write_call<5>(hook.address(), Patch().getCode());
	}

	float LightArmor::CalcSprintingStaminaMod(float a_cost, const RE::Actor* a_actor)
	{
		float cost = a_cost;
		LightArmor::HandleSprintingCost(a_actor, cost);
		cost = (std::max)(cost, 0.0f);

		// xorps xmm0, -1.0
		return -cost;
	}

	void LightArmor::HandleSprintingCost(const RE::Actor* a_actor, float& a_cost)
	{
		if (const auto perk = Data::ModObject<RE::BGSPerk>("PerkSprintReduceCost"sv);
			perk && a_actor->HasPerk(perk) && Common::PoEUtils::GetCountEquippedLightArmor(a_actor, true) >= 4) {
			float costMult = "SprintReduceCostMult"_gv.value_or(0.5f);
			a_cost *= costMult;
		}
	}

}