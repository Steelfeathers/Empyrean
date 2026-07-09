#pragma once

namespace Utils
{
	struct MagicUtils
	{
		static void RemoveOldestEffectStackWithKeyword(RE::Actor* target, RE::BGSKeyword* keyword);
	};
}