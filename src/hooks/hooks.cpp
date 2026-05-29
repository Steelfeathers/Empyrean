#include "Hooks/hooks.h"
#include "LightArmor.h"
#include "Unarmed.h"
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
		result &= LightArmor::InstallHooks();
		result &= Unarmed::InstallHooks();

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

		LightArmor::HandleUpdate(a_player, a_delta);
	}
}