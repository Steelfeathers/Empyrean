#pragma once

namespace Hooks
{
	struct Unarmed
	{
		static bool InstallHooks();

		static void ExperiencePatch();
		static void ProcessUnarmedXP(const RE::HitData& a_hitData);
	};
}