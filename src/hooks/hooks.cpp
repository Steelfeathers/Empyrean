#include "Hooks/hooks.h"
#include "LightArmor.h"
#include "Unarmed.h"
#include "Pickpocket.h"
#include "RE/Offset.h"
#include <xbyak/xbyak.h>
#include <Windows.h>

namespace Hooks {
	bool Install() {
		logger::info("Installing hooks..."sv);
		static constexpr std::size_t requiredHookAllocation = 14u; // 1 * 14 bytes for CALL.
		std::size_t additionalAlloc = 0u;

		//SKSE::AllocTrampoline(requiredHookAllocation, additionalAlloc);
		SKSE::AllocTrampoline(249);

		bool result = true;

		InstallUpdateHook();
		InstallCombatHitHook();
		result &= LightArmor::InstallHooks();
		result &= Unarmed::InstallHooks();
		result &= Pickpocket::InstallHooks();

		logger::info("Finished installing hooks."sv);
		return result;
	}

	void InstallUpdateHook()
	{
		auto vtbl = REL::Relocation<std::uintptr_t>(RE::Offset::PlayerCharacter::Vtbl);
		_Update = vtbl.write_vfunc(173, &Update);
	}

	void Update(RE::PlayerCharacter* a_player, float a_delta)
	{
		_Update(a_player, a_delta);

		LightArmor::ProcessUpdate(a_player, a_delta);
	}

	void InstallCombatHitHook()
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
				dq(std::bit_cast<std::uintptr_t>(&ProcessCombatHit));
			}
		};

		auto patch = new Patch(hook.address());

		// TRAMPOLINE: 8
		auto& trampoline = SKSE::GetTrampoline();
		REL::safe_fill(hook.address(), REL::NOP, 0x8);
		trampoline.write_branch<6>(hook.address(), patch->getCode());
	}

	void ProcessCombatHit(const RE::HitData& a_hitData)
	{
		Unarmed::ProcessCombatHit(a_hitData);
	}
}