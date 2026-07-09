#include "Destruction.h"
#include "Data/Lookup.h"
#include "Data/ModObjectManager.h"
#include "RE/Offset.h"
#include "Settings/INI/INISettings.h"
#include "Utils/MagicUtils.h"
using namespace RE;

namespace Hooks
{
	inline RE::BGSKeyword* KeywordMagicEffectMainFire;
	inline RE::BGSKeyword* KeywordImmolate;
	inline RE::BGSPerk* PerkCatchFire;
	inline RE::BGSPerk* PerkImmolate;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_ConcAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_ConcTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFContact;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFTargetLoc;

	inline RE::BGSKeyword* KeywordMagicEffectMainFrost;
	inline RE::BGSKeyword* KeywordPiercingCold;
	inline RE::BGSKeyword* KeywordFrostborn;
	inline RE::BGSPerk* PerkFrostborn;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_ConcAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_ConcTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFContact;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFTargetLoc;
	
	inline RE::BGSKeyword* MagicDamageFire;
	inline RE::BGSKeyword* MagicDamageFrost;
	inline RE::BGSKeyword* MagicDamageShock;
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Destruction::LoadData()
	{
		//auto keywordMagicDamageFrost = RE::TESForm::LookupByEditorID("MagicDamageFrost")->As<RE::BGSKeyword>();

		//Fire keywords and reference secondary effects
		KeywordMagicEffectMainFire = Data::ModObject<RE::BGSKeyword>("PoE_K_MagicEffectMainFire"sv);
		PerkCatchFire = Data::ModObject<RE::BGSPerk>("PoE_PERK_DES_CatchFire"sv);
		KeywordImmolate = Data::ModObject<RE::BGSKeyword>("PoE_K_Immolate"sv);
		PerkImmolate = Data::ModObject<RE::BGSPerk>("PoE_PERK_DES_Immolate"sv);

		const auto refSpellFire_ConcAimed = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFire_ConcAimed"sv);
		for (auto* refEff : refSpellFire_ConcAimed->effects)
		{
			secondaryMagEffsFire_ConcAimed.insert(refEff);
		}
		const auto refSpellFire_FFAimed = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFire_FFAimed"sv);
		for (auto* refEff : refSpellFire_FFAimed->effects)
		{
			secondaryMagEffsFire_FFAimed.insert(refEff);
		}

		//Frost keywords and reference secondary effects
		KeywordMagicEffectMainFrost = Data::ModObject<RE::BGSKeyword>("PoE_K_MagicEffectMainFrost"sv);
		KeywordPiercingCold = Data::ModObject<RE::BGSKeyword>("PoE_K_PiercingCold"sv);
		KeywordFrostborn = Data::ModObject<RE::BGSKeyword>("PoE_K_Frostborn"sv);
		PerkFrostborn = Data::ModObject<RE::BGSPerk>("PoE_PERK_DES_Frostborn"sv);
		
		const auto refSpellFrost_ConcAimed = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFrost_ConcAimed"sv);
		for (auto* refEff : refSpellFrost_ConcAimed->effects)
		{
			secondaryMagEffsFrost_ConcAimed.insert(refEff);
		}
		const auto refSpellFrost_ConcTarget = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFrost_ConcTarget"sv);
		for (auto* refEff : refSpellFrost_ConcTarget->effects)
		{
			secondaryMagEffsFrost_ConcTarget.insert(refEff);
		}
		const auto refSpellFrost_FFAimed = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFrost_FFAimed"sv);
		for (auto* refEff : refSpellFrost_FFAimed->effects)
		{
			secondaryMagEffsFrost_FFAimed.insert(refEff);
		}
		const auto refSpellFrost_FFContact = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFrost_FFContact"sv);
		for (auto* refEff : refSpellFrost_FFContact->effects)
		{
			secondaryMagEffsFrost_FFContact.insert(refEff);
		}
		const auto refSpellFrost_FFTarget = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFrost_FFTarget"sv);
		for (auto* refEff : refSpellFrost_FFTarget->effects)
		{
			secondaryMagEffsFrost_FFTarget.insert(refEff);
		}
		const auto refSpellFrost_FFTargetLoc = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES__SecondaryEffectsFrost_FFTargetLoc"sv);
		for (auto* refEff : refSpellFrost_FFTargetLoc->effects)
		{
			secondaryMagEffsFrost_FFTargetLoc.insert(refEff);
		}
	}

	void Destruction::PatchSpell(RE::SpellItem* spell, RE::Effect* eff, RE::EffectSetting* magEff)
	{
		if (magEff->HasKeyword(KeywordMagicEffectMainFire))
		{
			PatchFireSpell(spell, eff, magEff);
		}
		else if (magEff->HasKeyword(KeywordMagicEffectMainFrost))
		{
			PatchFrostSpell(spell, eff, magEff);
		}
	}

	void Destruction::PatchFireSpell(RE::SpellItem* spell, RE::Effect* eff, RE::EffectSetting* magEff)
	{
		auto secondaryRefEffs = std::unordered_set<RE::Effect*>{};

		auto castingType = magEff->data.castingType;
		auto deliveryType = magEff->data.delivery;

		bool validType = false;
		if (castingType == RE::MagicSystem::CastingType::kConcentration && deliveryType == RE::MagicSystem::Delivery::kAimed)
		{
			secondaryRefEffs = secondaryMagEffsFire_ConcAimed;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kConcentration && deliveryType == RE::MagicSystem::Delivery::kTargetActor)
		{
			secondaryRefEffs = secondaryMagEffsFire_ConcTarget;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kAimed)
		{
			secondaryRefEffs = secondaryMagEffsFire_FFAimed;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kTargetActor)
		{
			secondaryRefEffs = secondaryMagEffsFire_FFTarget;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kTouch)
		{
			secondaryRefEffs = secondaryMagEffsFire_FFContact;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kTargetLocation)
		{
			secondaryRefEffs = secondaryMagEffsFire_FFTargetLoc;
			validType = true;
		}

		if (!validType) return;

		for (auto* refEff : secondaryRefEffs)
		{
			float mag = refEff->effectItem.magnitude;
			AddMagicEffect(spell, refEff->baseEffect, mag, eff);
		}
		logger::info("Patched fire spell {}"sv, spell->GetName());
	}

	void Destruction::PatchFrostSpell(RE::SpellItem* spell, RE::Effect* eff, RE::EffectSetting* magEff)
	{
		auto secondaryRefEffs = std::unordered_set<RE::Effect*>{};

		auto castingType = magEff->data.castingType;
		auto deliveryType = magEff->data.delivery;

		bool validType = false;
		if (castingType == RE::MagicSystem::CastingType::kConcentration && deliveryType == RE::MagicSystem::Delivery::kAimed)
		{
			secondaryRefEffs = secondaryMagEffsFrost_ConcAimed;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kConcentration && deliveryType == RE::MagicSystem::Delivery::kTargetActor)
		{
			secondaryRefEffs = secondaryMagEffsFrost_ConcTarget;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kAimed)
		{
			secondaryRefEffs = secondaryMagEffsFrost_FFAimed;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kTargetActor)
		{
			secondaryRefEffs = secondaryMagEffsFrost_FFTarget;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kTouch)
		{
			secondaryRefEffs = secondaryMagEffsFrost_FFContact;
			validType = true;
		}
		else if (castingType == RE::MagicSystem::CastingType::kFireAndForget && deliveryType == RE::MagicSystem::Delivery::kTargetLocation)
		{
			secondaryRefEffs = secondaryMagEffsFrost_FFTargetLoc;
			validType = true;
		}

		if (!validType) return;

		for (auto* refEff : secondaryRefEffs)
		{
			float mag = refEff->baseEffect->HasKeyword(KeywordPiercingCold) ? eff->effectItem.magnitude * 0.25 : refEff->effectItem.magnitude;
			AddMagicEffect(spell, refEff->baseEffect, mag, eff);
		}
		logger::info("Patched frost spell {}"sv, spell->GetName());
	}

	bool Destruction::AddMagicEffect(RE::MagicItem* a_spell, RE::EffectSetting* a_effSetting, float a_mag, RE::Effect* a_refEff)
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

	/*
	bool Destruction::SplitMagicEffect(RE::EffectSetting* origEffect)
	{
		auto baseEff = new RE::EffectSetting();
		baseEff->Copy(origEffect);
		
	}
	*/

	//------------------------------------------------------------------------
	void Destruction::ProcessMagicEffectAdded_DES(RE::Actor* a_target, RE::Actor* a_caster, RE::ActiveEffect* a_effect, RE::SpellItem* a_spell)
	{
		auto magEff = a_effect->GetBaseObject();
		
		if (magEff->HasKeyword(KeywordMagicEffectMainFire))
		{
			logger::info("{} was hit by a fire spell: {}"sv, a_target->GetName(), a_spell->GetName());
			if (a_caster->HasPerk(PerkCatchFire))
			{
				
				//logger::info("     > Caster has CatchFire, extended spell duration"sv, a_target->GetName());
			}
		}

		//Prevent frostborn stacking
		if (magEff->HasKeyword(KeywordMagicEffectMainFrost) && a_caster->HasPerk(PerkFrostborn))
		{
			Utils::MagicUtils::RemoveOldestEffectStackWithKeyword(a_target, KeywordFrostborn);
		}

		//Prevent Immolate stacking
		if (magEff->HasKeyword(KeywordMagicEffectMainFire) && a_caster->HasPerk(PerkImmolate))
		{
			Utils::MagicUtils::RemoveOldestEffectStackWithKeyword(a_target, KeywordImmolate);
		}

	}
}