#pragma once

namespace Hooks
{
	class TwoHanded final
	{
	public:
		static void ProcessCombatHit(RE::Actor* a_this, RE::HitData* a_hitData);

	};
}
