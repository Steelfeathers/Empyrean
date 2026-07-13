#pragma once

namespace Hooks
{
	class Smithing final
	{
	public:
		static void LoadData();
		static void CreateStasisCubeFromAutomaton(RE::Actor* automaton);
	private:
		static void CreatePotionFromEffects(RE::EffectSetting* mainSpawnMagEff, RE::BSTArray<RE::Effect> effects, std::string modelName, float weight, float value);
	};
}
