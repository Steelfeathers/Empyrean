#include "Destruction.h"
#include "Data/Lookup.h"
#include "Data/ModObjectManager.h"
#include "RE/Offset.h"
#include "Settings/INI/INISettings.h"
using namespace RE;

namespace Hooks
{
	inline std::unordered_set<RE::EffectSetting*> mainMagEffs_frost;

	bool IsMainMagEffFrost(EffectSetting* magEff) {
		return magEff && mainMagEffs_frost.contains(magEff);
	}

	bool AddMagicEffect(RE::MagicItem* a_spell, RE::EffectSetting* a_effSetting, float a_mag, RE::Effect* a_refEff)
	{
		if (auto effect = new RE::Effect()) {
			effect->effectItem.magnitude = a_mag;
			effect->effectItem.area = a_refEff->effectItem.area;
			effect->effectItem.duration = a_refEff->effectItem.duration;
			effect->baseEffect = a_effSetting;

			a_spell->effects.push_back(effect);
			return true;
		}
		return false;
	}

	void Destruction::PatchFrostSpells()
	{
		TESDataHandler* dataHandler = TESDataHandler::GetSingleton();
		if (!dataHandler) return;

		const auto KeywordMagicEffectMainFrost = Data::ModObject<RE::BGSKeyword>("KeywordMagicEffectMainFrost"sv);
		const auto MagEffChilledToTheBone_concAimed = Data::ModObject<RE::EffectSetting>("MagEffChilledToTheBone_concAimed"sv);
		const auto MagEffChilledToTheBone_FFAimed = Data::ModObject<RE::EffectSetting>("MagEffChilledToTheBone_FFAimed"sv);
		//const auto MagEffChilledToTheBone_FFTarget = Data::ModObject<RE::EffectSetting>("MagEffChilledToTheBone_FFTarget"sv);
		const auto MagEffPiercingCold_concAimed = Data::ModObject<RE::EffectSetting>("MagEffPiercingCold_concAimed"sv);
		const auto MagEffPiercingCold_FFAimed = Data::ModObject<RE::EffectSetting>("MagEffPiercingCold_FFAimed"sv);
		const auto MagEffFrostborn_concAimed = Data::ModObject<RE::EffectSetting>("MagEffFrostborn_concAimed"sv);
		const auto MagEffFrostborn_FFAimed = Data::ModObject<RE::EffectSetting>("MagEffFrostborn_FFAimed"sv);

		auto spells = dataHandler->GetFormArray<SpellItem>();

		for (auto* spell : spells)
		{
			if (!spell) continue;
			auto effs = spell->effects;
			for (auto* eff : effs)
			{
				if (!eff) continue;

				auto magEff = eff->baseEffect;
				if (!magEff) continue;

				//Cache the magic effects that are "main" effects for valid frost spells
				bool isValid = false;
				if (magEff->HasKeyword(KeywordMagicEffectMainFrost))
				{
					mainMagEffs_frost.insert(magEff);
					isValid = true;
					logger::info("PatchFrostSpells() - Found a magEff marked with keyword PoE_K_MagicEffectMainFrost"sv);
				}
				else
				{
					//TODO
				}
				if (!isValid) continue;

				/*
				//Frost spells MUST be set to NoDispelOnDeath and have a taper duration > 0
				//Otherwise, Frostborn won't trigger on death
				if (magEff->data.taperDuration <= 0.0 || magEff->data.taperWeight <= 0.0)
				{
					magEff->data.taperDuration = 0.1;
					magEff->data.taperWeight = 0.01;
					magEff->data.taperCurve = 1.0;
					logger::info("     > Updated taper settings: duration={}, weight={}, curve={}"sv, magEff->data.taperDuration, magEff->data.taperWeight, magEff->data.taperCurve);
				}
				if (!magEff->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kNoDeathDispel))
				{
					magEff->data.flags.set(RE::EffectSetting::EffectSettingData::Flag::kNoDeathDispel);
					logger::info("     > Set NoDeathDispel flag: "sv, magEff->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kNoDeathDispel) ? "True" : "False");
				}
				*/

				//Add secondary effects to spell
				RE::EffectSetting* magEffChilledToTheBone = nullptr;
				RE::EffectSetting* magEffPiercingCold = nullptr;
				RE::EffectSetting* magEffFrostborn = nullptr;

				auto castingType = magEff->data.castingType;
				auto deliveryType = magEff->data.delivery;

				if (castingType == RE::MagicSystem::CastingType::kConcentration && deliveryType == RE::MagicSystem::Delivery::kAimed)
				{
					magEffChilledToTheBone = MagEffChilledToTheBone_concAimed;
					magEffPiercingCold = MagEffPiercingCold_concAimed;
					magEffFrostborn = MagEffFrostborn_concAimed;
				}
				else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kAimed)
				{
					magEffChilledToTheBone = MagEffChilledToTheBone_FFAimed;
					magEffPiercingCold = MagEffPiercingCold_FFAimed;
					magEffFrostborn = MagEffFrostborn_FFAimed;
				}

				//ChilledToTheBone
				if (magEffChilledToTheBone && AddMagicEffect(spell, magEffChilledToTheBone, 40.0, eff))
				{
					logger::info("     > Added secondary effect ChilledToTheBone to the spell {}"sv, spell->GetName());
				}

				//PiercingCold
				float mag = eff->effectItem.magnitude * 0.25;
				if (magEffPiercingCold && AddMagicEffect(spell, magEffPiercingCold, mag, eff))
				{
					logger::info("     > Added secondary effect PiercingCold to the spell {}"sv, spell->GetName());
				}

				//Frostborn
				if (magEffFrostborn && AddMagicEffect(spell, magEffFrostborn, 0.0, eff))
				{
					logger::info("     > Added secondary effect Frostborn to the spell {}"sv, spell->GetName());
				}
			}

		}

	}

}