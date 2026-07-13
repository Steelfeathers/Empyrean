#include "TwoHanded.h"
#include "Data/Lookup.h"
#include "Data/ModObjectManager.h"
#include "RE/Offset.h"
#include "Settings/INI/INISettings.h"
#include "Utils/MagicUtils.h"

namespace Hooks
{
	void TwoHanded::ProcessCombatHit(RE::Actor* a_this, RE::HitData* a_hitData)
	{
		const auto aggressor = a_hitData->aggressor.get().get();
		const auto victim = a_hitData->target.get().get();

		if (!aggressor)
			return;
		if (!victim || victim->IsDead())
			return;

		if (!a_hitData->weapon)
			return;

		//Not a 2H bladed weapon
		const auto KeywordWeapBladed2H = Data::ModObject<RE::BGSKeyword>("KeywordWeapBladed2H"sv);
		if (!a_hitData->weapon->HasKeyword(KeywordWeapBladed2H))
			return;

		const auto KeywordWeapBleed = Data::ModObject<RE::BGSKeyword>("KeywordWeapBleed"sv);

		Utils::MagicUtils::RemoveOldestEffectStackWithKeyword(victim, KeywordWeapBleed);

		/*
		int bleedStackCount = 0;

		const auto activeEffects = victim->GetActiveEffectList();
		if (!activeEffects)
			return;

		float maxElapsedTime = 0.0;
		RE::ActiveEffect* oldestBleedEff = nullptr;

		for (const auto& activeEffect : *activeEffects) {
			if (auto mgef = activeEffect ? activeEffect->GetBaseObject() : nullptr; mgef) {
				if (activeEffect->flags.all(RE::ActiveEffect::Flag::kDispelled))
					continue;

				if (mgef->HasKeyword(KeywordWeapBleed))
				{
					bleedStackCount += 1;

					float time = activeEffect->elapsedSeconds;
					if (time >= maxElapsedTime)
					{
						maxElapsedTime = time;
						oldestBleedEff = activeEffect;
					}
				}
			}
		}

		//Don't stack up bleeding 
		if (bleedStackCount > 1 && oldestBleedEff)
		{
			oldestBleedEff->Dispel(true);
			//logger::info("     > TwoHanded: Dispelled oldeset bleed stack"sv);
		}
		*/
	}
}