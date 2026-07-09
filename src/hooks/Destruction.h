#pragma once

namespace Hooks
{
	class Destruction final
	{
	public:
		static void LoadData();
		static void PatchSpell(RE::SpellItem* spell, RE::Effect* eff, RE::EffectSetting* magEff);
		static void ProcessMagicEffectAdded_DES(RE::Actor* a_target, RE::Actor* a_caster, RE::ActiveEffect* a_effect, RE::SpellItem* a_spell);

	private:
		static void PatchFireSpell(RE::SpellItem* spell, RE::Effect* eff, RE::EffectSetting* magEff);
		static void PatchFrostSpell(RE::SpellItem* spell, RE::Effect* eff, RE::EffectSetting* magEff);
		static void PatchShockSpell(RE::SpellItem* spell, RE::Effect* eff, RE::EffectSetting* magEff);
		static bool AddMagicEffect(RE::MagicItem* a_spell, RE::EffectSetting* a_effSetting, float a_mag, RE::Effect* a_refEff);
		static bool SplitMagicEffect();
	};
}
