#pragma once

namespace Hooks
{
	class Alteration final
	{
	public:
		static bool InstallHooks();
		static void ProcessUpdate(RE::PlayerCharacter* a_player, float a_delta);

	
	};
}