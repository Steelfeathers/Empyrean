#pragma once

namespace Hooks
{
	class OneHanded final
	{
	public:
		static void ProcessCombatHit(RE::Actor* a_this, RE::HitData* a_hitData);

	};
}
