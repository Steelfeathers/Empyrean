#include "Hooks/hooks.h"
#include "PlayerMovement.h"

namespace Hooks {
	bool Install() {
		logger::info("Installing hooks..."sv);
		static constexpr std::size_t requiredHookAllocation = 14u; // 1 * 14 bytes for CALL.
		std::size_t additionalAlloc = 0u;

		//SKSE::AllocTrampoline(requiredHookAllocation, additionalAlloc);
		SKSE::AllocTrampoline(249);

		bool result = true;

		result &= PlayerMovement::InstallHooks();

		logger::info("Finished installing hooks."sv);
		return result;
	}
}