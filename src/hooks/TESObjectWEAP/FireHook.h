#pragma once

namespace Hooks
{
	namespace TESObjectWEAP
	{
		struct FireHook
		{
			static bool InstallFireHook();

			static RE::ProjectileHandle* CreateProjectile(RE::ProjectileHandle* a_result, 
				RE::Projectile::LaunchData& a_data);
			inline static REL::Relocation<decltype(CreateProjectile)> _CreateProjectile;
		};
	}
}