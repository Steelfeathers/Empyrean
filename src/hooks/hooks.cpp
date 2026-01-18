#include "Hooks.h"

#include "TESObjectWEAP/FireHook.h"

namespace Hooks {
	bool Install() {
		logger::info("Installing hooks..."sv);
		static constexpr std::size_t requiredHookAllocation = 14u; // 1 * 14 bytes for CALL.
		std::size_t additionalAlloc = 0u;

		SKSE::AllocTrampoline(requiredHookAllocation, additionalAlloc);

		bool result = true;

		result &= TESObjectWEAP::FireHook::InstallFireHook();

		logger::info("Finished installing hooks."sv);
		return result;
	}
}