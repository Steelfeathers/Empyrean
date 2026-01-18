#include "FireHook.h"

#include "Controller/Multishot/MultishotController.h"
#include "RE/Offset.h"

namespace Hooks::TESObjectWEAP
{
	bool FireHook::InstallFireHook() {
		logger::info("  Installing Weapon Firing Hook..."sv);
		REL::Relocation<std::uintptr_t> target{ RE::Offset::TESObjectWEAP::Fire, 0xE60 };

		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to install Weapon Firing Hook. Callsite exhibits unexpected pattern."sv);
			return false;
		}

		auto& trampoline = SKSE::GetTrampoline();
		_CreateProjectile = trampoline.write_call<5>(target.address(), &CreateProjectile);
		logger::info("    >Weapon Firing Hook installed successfully."sv);
		return true;
	}

	RE::ProjectileHandle* FireHook::CreateProjectile(RE::ProjectileHandle* a_result, 
		RE::Projectile::LaunchData& a_data)
	{
		auto* result = _CreateProjectile(a_result, a_data);
		auto* multishot = Controller::Multishot::MultishotController::GetSingleton();
		if (multishot) {
			multishot->AttemptMultishot(a_data, result);
		}
		return result;
	}
}