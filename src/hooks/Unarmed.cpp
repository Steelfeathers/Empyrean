#include "Unarmed.h"
#include "RE/Offset.h"
#include <xbyak/xbyak.h>

namespace Hooks
{
	bool Unarmed::InstallHooks() {
		logger::info("  Installing Unarmed Hooks..."sv);
		ExperiencePatch();
		return true;
	}

	void Unarmed::ExperiencePatch()
	{
		auto hook = REL::Relocation<std::uintptr_t>(RE::Offset::Actor::CombatHit, 0x292);
		REL::make_pattern<"48 8B 8C 24 80 00 00 00">().match_or_fail(hook.address());

		struct Patch : Xbyak::CodeGenerator
		{
			Patch(std::uintptr_t a_hookAddr)
			{
				Xbyak::Label retn;
				Xbyak::Label funcLbl;

				lea(rcx, ptr[rsp + 0x50]);
				call(ptr[rip + funcLbl]);
				mov(rcx, ptr[rsp + 0x80]);

				jmp(ptr[rip]);
				dq(a_hookAddr + 0x8);

				L(funcLbl);
				dq(std::bit_cast<std::uintptr_t>(&Unarmed::ProcessUnarmedXP));
			}
		};

		auto patch = new Patch(hook.address());

		// TRAMPOLINE: 8
		auto& trampoline = SKSE::GetTrampoline();
		REL::safe_fill(hook.address(), REL::NOP, 0x8);
		trampoline.write_branch<6>(hook.address(), patch->getCode());
	}

	void Unarmed::ProcessUnarmedXP(const RE::HitData& a_hitData)
	{
		const auto aggressor = a_hitData.aggressor.get();
		if (!aggressor || !aggressor->GetIsPlayerOwner())
			return;

		if (!a_hitData.weapon || !a_hitData.weapon->IsHandToHandMelee())
			return;

		//const auto playerChar = aggressor->As<RE::PlayerCharacter>();
		//if (!playerChar) { return; }
		//playerChar->AddSkillExperience(RE::ActorValue::kLightArmor, skillUse);
		RE::PlayerCharacter::GetSingleton()->AddSkillExperience(RE::ActorValue::kLightArmor, 10.0f);
	}
}