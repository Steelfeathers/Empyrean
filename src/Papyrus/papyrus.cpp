#include "papyrus.h"
#include "Hooks/Pickpocket.h"
#include "Utils/ArmorUtils.h"

namespace Papyrus
{
	std::vector<int> GetVersion(STATIC_ARGS) {
		return { Plugin::VERSION[0], Plugin::VERSION[1], Plugin::VERSION[2] };
	}

	static void UpdateRacesAllowPickpocket(STATIC_ARGS)
	{
		Hooks::Pickpocket::SetRacesAllowPickpocket();
	}

	std::vector< RE::TESObjectARMO*> GetAllEquippedArmor(STATIC_ARGS, RE::Actor* a_actor)
	{
		return Utils::ArmorUtils::GetEquippedArmor(a_actor);
	}

	void Bind(VM& a_vm) {
		logger::info("  >Binding GetVersion..."sv);
		BIND(GetVersion);
		logger::info("  >Binding UpdateRacesAllowPickpocket..."sv);
		BIND(UpdateRacesAllowPickpocket);
		logger::info("  >Binding GetAllEquippedArmor..."sv);
		BIND(GetAllEquippedArmor);
	}

	bool RegisterFunctions(VM* a_vm) {
		logger::info("Binding papyrus functions in utility script {}..."sv, script);
		Bind(*a_vm);
		logger::info("Finished binding functions."sv);
		return true;
	}
}
