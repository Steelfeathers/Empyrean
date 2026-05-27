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
		SprintingCostPatch();
		return true;
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
		float costMult = 0.1f;
		a_cost *= costMult;
		}
	}
}