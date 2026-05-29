#pragma once

namespace Hooks
{
	struct Unarmed
	{
		static bool InstallHooks();

		static void ProcessCombatHit(const RE::HitData& a_hitData);
	};
}