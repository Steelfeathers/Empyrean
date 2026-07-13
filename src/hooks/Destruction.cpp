#include "Destruction.h"
#include "Data/Lookup.h"
#include "Data/ModObjectManager.h"
#include "RE/Offset.h"
#include "Settings/INI/INISettings.h"
#include "Utils/MagicUtils.h"
using namespace RE;

namespace Hooks
{
	//inline RE::BGSKeyword* KeywordMagicEffectMainFire;
	inline RE::BGSKeyword* KeywordImmolate;
	/*
	inline RE::BGSPerk* PerkCatchFire;
	inline RE::BGSPerk* PerkImmolate;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_ConcAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_ConcTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFContact;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFire_FFTargetLoc;
	*/

	//inline RE::BGSKeyword* KeywordMagicEffectMainFrost;
	//inline RE::BGSKeyword* KeywordPiercingCold;
	inline RE::BGSKeyword* KeywordFrostborn;
	/*
	inline RE::BGSPerk* PerkFrostborn;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_ConcAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_ConcTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFAimed;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFContact;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFTarget;
	inline std::unordered_set<RE::Effect*> secondaryMagEffsFrost_FFTargetLoc;

	inline RE::BGSPerk* PerkForceOfNature;
	inline RE::SpellItem* SpellForceOfNature_FireIce;
	inline RE::SpellItem* SpellForceOfNature_IceShock;
	inline RE::SpellItem* SpellForceOfNature_ShockFire;
	inline RE::BGSKeyword* KeywordSpellChainFire;
	inline RE::BGSKeyword* KeywordSpellChainFrost;
	inline RE::BGSKeyword* KeywordSpellChainShock;
	inline RE::BGSKeyword* KeywordSpellChainCooldown_FireIce;
	inline RE::BGSKeyword* KeywordSpellChainCooldown_IceShock;
	inline RE::BGSKeyword* KeywordSpellChainCooldown_ShockFire;
	
	inline RE::BGSKeyword* MagicDamageFire;
	inline RE::BGSKeyword* MagicDamageFrost;
	inline RE::BGSKeyword* MagicDamageShock;
	*/
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Destruction::LoadData()
	{

		//Cache the data needed for patching spells, to prevent needing to look it up for every spell we loop through

		//Grab keywords and secondary magEffs for Fire spells
		//KeywordMagicEffectMainFire = Data::ModObject<RE::BGSKeyword>("PoE_K_MagicEffectMainFire"sv);
		//PerkCatchFire = Data::ModObject<RE::BGSPerk>("PoE_PERK_DES_CatchFire"sv);
		KeywordImmolate = Data::ModObject<RE::BGSKeyword>("PoE_K_Immolate"sv);
		//PerkImmolate = Data::ModObject<RE::BGSPerk>("PoE_PERK_DES_Immolate"sv);

		/*
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
		*/

		//Grab keywords and secondary magEffs for Frost spells
		//KeywordMagicEffectMainFrost = Data::ModObject<RE::BGSKeyword>("PoE_K_MagicEffectMainFrost"sv);
		//KeywordPiercingCold = Data::ModObject<RE::BGSKeyword>("PoE_K_PiercingCold"sv);
		KeywordFrostborn = Data::ModObject<RE::BGSKeyword>("PoE_K_Frostborn"sv);
		//PerkFrostborn = Data::ModObject<RE::BGSPerk>("PoE_PERK_DES_Frostborn"sv);
		
		/*
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

		//Grab the ForceOfNature spells
		PerkForceOfNature = Data::ModObject<RE::BGSPerk>("PoE_PERK_DES_ForceOfNature"sv);
		SpellForceOfNature_FireIce = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES_SpellChaining_FireIce"sv);
		SpellForceOfNature_IceShock = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES_SpellChaining_IceShock"sv);
		SpellForceOfNature_ShockFire = Data::ModObject<RE::SpellItem>("PoE_SPELL_DES_SpellChaining_ShockFire"sv);
		KeywordSpellChainFire = Data::ModObject<RE::BGSKeyword>("PoE_K_SpellChaining_Fire"sv);
		KeywordSpellChainFrost = Data::ModObject<RE::BGSKeyword>("PoE_K_SpellChaining_Frost"sv);
		KeywordSpellChainShock = Data::ModObject<RE::BGSKeyword>("PoE_K_SpellChaining_Shock"sv);
		KeywordSpellChainCooldown_FireIce = Data::ModObject<RE::BGSKeyword>("PoE_K_SpellChaining_FireFrost_cool"sv);
		KeywordSpellChainCooldown_IceShock = Data::ModObject<RE::BGSKeyword>("PoE_K_SpellChaining_FrostElectric_cool"sv);
		KeywordSpellChainCooldown_ShockFire = Data::ModObject<RE::BGSKeyword>("PoE_K_SpellChaining_ShockFire_cool"sv);
		*/
	}

	/*
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
		//auto* newMagEff = SplitMagicEffect(magEff);

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
	*/
	
	//------------------------------------------------------------------------
	void Destruction::ProcessMagicEffectAdded_DES(RE::Actor* a_target, RE::Actor* a_caster, RE::ActiveEffect* a_effect, RE::MagicItem* a_spell)
	{
		auto magEff = a_effect->GetBaseObject();
		
		//If hit with a fire spell...
		if (magEff->HasKeyword(KeywordImmolate))
		//if (magEff->HasKeyword(KeywordMagicEffectMainFire))
		{
			//logger::info("{} was hit by a fire spell: {}"sv, a_target->GetName(), a_spell->GetName());

			//Prevent Immolate stacking
			//if (a_caster->HasPerk(PerkImmolate))
			{
				logger::info("Preventing Immolate stacking..."sv);
				Utils::MagicUtils::RemoveOldestEffectStackWithKeyword(a_target, KeywordImmolate);
			}
			/*
			//Try to apply ForceOfNature spell combos
			if (a_caster->HasPerk(PerkForceOfNature))
			{
				//Fire/Ice
				if (a_target->HasMagicEffectWithKeyword(KeywordSpellChainFrost, nullptr) && !a_target->HasMagicEffectWithKeyword(KeywordSpellChainCooldown_FireIce, nullptr))
				{
					a_caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->CastSpellImmediate(SpellForceOfNature_FireIce, false, a_target, 1.0f, false, 0.0f, a_caster);
					logger::info("   > Applied ForceOfNature Fire/Ice spell!"sv);
				}
			}
			*/
		}

		//If hit with a frost spell...
		if (magEff->HasKeyword(KeywordFrostborn))
		//if (magEff->HasKeyword(KeywordMagicEffectMainFrost))
		{
			//Prevent frostborn stacking
			//if (a_caster->HasPerk(PerkFrostborn))
			{
				logger::info("Preventing Frostborn stacking..."sv);
				Utils::MagicUtils::RemoveOldestEffectStackWithKeyword(a_target, KeywordFrostborn);
			}
		}


	}
}