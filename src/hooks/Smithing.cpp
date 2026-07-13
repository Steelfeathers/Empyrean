#include "Smithing.h"
#include "Data/ModObjectManager.h"

namespace Hooks
{
	inline RE::BGSKeyword* KeywordCraftingAutomaton;
	inline RE::BGSKeyword* KeywordCraftingSmithingForge;
	inline RE::BGSKeyword* KeywordVendorItemClutter;
	inline RE::BGSKeyword* KeywordVendorItemTool;
	inline RE::BGSKeyword* KeywordAutomaton;
	inline RE::BGSKeyword* KeywordFabricant;
	inline RE::BGSKeyword* KeywordAether;
	inline RE::BGSListForm* ListRaceKeywords;
	inline RE::BGSListForm* ListRaceWeights;
	inline RE::BGSListForm* ListFabricantActors;
	inline RE::BGSListForm* ListFabricantWeights;
	inline RE::BGSListForm* ListActiveUpgradeMagEffs;
	inline RE::BGSListForm* ListSpawnRaceMagEffs;
	inline RE::BGSListForm* ListSpawnRaceMagEffsAether;
	inline RE::BGSListForm* ListSpawnRaceMagEffsFabricant;
	inline RE::BGSListForm* ListSpawnUpgradeMagEffs;
	inline RE::EffectSetting* MagEffSpawnUpgradeStart;
	inline RE::EffectSetting* MagEffSpawnUpgradeSpacer;
	inline RE::EffectSetting* MagEffSpawnUpgradeEnd;
	inline bool loaded;

	void Smithing::LoadData()
	{
		KeywordCraftingAutomaton = Data::ModObject<RE::BGSKeyword>("IWP_K_DwarvenAutomaton"sv);
		KeywordCraftingSmithingForge = RE::TESForm::LookupByEditorID("CraftingSmithingForge")->As<RE::BGSKeyword>();
		KeywordVendorItemClutter = RE::TESForm::LookupByEditorID("VendorItemClutter")->As<RE::BGSKeyword>();
		KeywordVendorItemTool = RE::TESForm::LookupByEditorID("VendorItemTool")->As<RE::BGSKeyword>();

		KeywordFabricant = Data::ModObject<RE::BGSKeyword>("IWP_K_DwarvenAutomatonFabricant"sv);
		KeywordAether = Data::ModObject<RE::BGSKeyword>("IWP_K_DwarvenAutomatonAether"sv);

		ListRaceKeywords = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_RaceKeywords"sv);
		ListRaceWeights = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_RaceWeights"sv);
		ListSpawnRaceMagEffs = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_SpawnRaceMagEffs"sv);
		ListSpawnRaceMagEffsAether = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_SpawnRaceMagEffs_Aether"sv);

		ListFabricantActors = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_FabricantActors"sv);
		ListFabricantWeights = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_FabricantWeights"sv);
		ListSpawnRaceMagEffsFabricant = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_SpawnRaceMagEffs_Fabricant"sv);
		
		ListActiveUpgradeMagEffs = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_ActiveUpgradeMagEffs"sv);
		ListSpawnUpgradeMagEffs = Data::ModObject<RE::BGSListForm>("PoE_LIST_SMITH_DwarvenAutomaton_SpawnUpgradeMagEffs"sv);

		MagEffSpawnUpgradeStart = Data::ModObject<RE::EffectSetting>("PoE_MGEF_SMITH_SpawnDwarvenAutomaton_Upgrade_start"sv);
		MagEffSpawnUpgradeSpacer = Data::ModObject<RE::EffectSetting>("PoE_MGEF_SMITH_SpawnDwarvenAutomaton_Upgrade_spacer"sv);
		MagEffSpawnUpgradeEnd = Data::ModObject<RE::EffectSetting>("PoE_MGEF_SMITH_SpawnDwarvenAutomaton_Upgrade_end"sv);

		if (!KeywordCraftingSmithingForge || !KeywordFabricant || !KeywordAether ||
			!ListRaceKeywords || !ListRaceWeights || !ListSpawnRaceMagEffs || !ListSpawnRaceMagEffsAether || !ListFabricantActors || !ListFabricantWeights ||
			!ListSpawnRaceMagEffsFabricant || !ListSpawnUpgradeMagEffs || !ListActiveUpgradeMagEffs ||
			!MagEffSpawnUpgradeStart || !MagEffSpawnUpgradeSpacer || !MagEffSpawnUpgradeEnd) {
			logger::info("ERROR: Failed to fetch necessary data for crafted dwarven automatons!");
			loaded = false;
			return;
		}

		loaded = true;
	}

	void Smithing::CreateStasisCubeFromAutomaton(RE::Actor* automaton)
	{
		if (!loaded) return;

		if (!automaton) {
			logger::info("Warning! CreateStasisCubeFromAutomaton() failed, automaton is null");
			return;
		}
		
		logger::info("CreateStasisCubeFromAutomaton() - {}"sv, automaton->GetDisplayFullName());

		RE::BSTArray<RE::Effect> effects;
		float weight = 4.0;
		float value = 150.0;

		//Check if this is a Fabricant (handled differently)
		if (automaton->HasKeyword(KeywordFabricant))
		{
			logger::info("  > Target is Fabricant"sv);

			int indexActor = 0;
			for (auto* form : ListFabricantActors->forms)
			{
				const auto actorBase = form->As<RE::TESNPC>();
				if (!actorBase)
				{
					logger::info("  > Warning! ActorBase is null?");
				}
				if (automaton->GetActorBase()->formID == actorBase->formID) {
					break;
				}
				indexActor += 1;
			}
			if (indexActor >= ListSpawnRaceMagEffs->forms.size()) {
				logger::info("  > Warning! Could not find this automaton in the fabricant list, aborting...");
				return;
			}

			//Grab the spawn magEff for this automaton and add it to the effects list
			RE::EffectSetting* automatonMagEff = ListSpawnRaceMagEffsFabricant->forms[indexActor]->As<RE::EffectSetting>();
			if (!automatonMagEff) {
				logger::info("  > Warning! The spawn magic effect for this fabricant is missing or incorrect somehow, aborting...");
				return;
			}

			RE::Effect eff;
			eff.baseEffect = automatonMagEff;
			eff.SetMagnitude(10.0);
			eff.SetDuration(1.0);
			effects.push_back(eff);

			//Fetch the cube weight
			if (indexActor < ListFabricantWeights->forms.size())
			{
				auto glo = ListFabricantWeights->forms[indexActor]->As<RE::TESGlobal>();
				if (glo) weight = glo->value;
			}

			//Pass in the effects to create the potion
			CreatePotionFromEffects(automatonMagEff, effects, "Clutter\\Dwemer\\DweLexiconCubeCorrupt01.nif", weight, value + 50.0);
			logger::info("  > Successfully created a stasis cube!"sv);

			return; //Don't do anything else with Fabricants
		}
		

		//Find which basic race (spider, sphere, centurion, etc) this automaton is
		int indexRace = 0;
		for (auto* form : ListRaceKeywords->forms)
		{
			const auto keyword = form->As<RE::BGSKeyword>();
			if (automaton->HasKeyword(keyword)) {
				break;
			}
			indexRace += 1;
		}
		if (indexRace >= ListSpawnRaceMagEffs->forms.size()) {
			logger::info("  > Warning! This automaton does not have one of the race keywords in the list, aborting...");
			return;
		}

		//Grab the spawn magEff for this automaton and add it to the effects list
		RE::EffectSetting* automatonMagEff = ListSpawnRaceMagEffs->forms[indexRace]->As<RE::EffectSetting>();
		std::string cubeModel = "Clutter\\Dwemer\\DwePuzzleCube.nif";

		//Different spawn magEff and cube model if Aether version
		if (automaton->HasKeyword(KeywordAether))
		{
			automatonMagEff = ListSpawnRaceMagEffsAether->forms[indexRace]->As<RE::EffectSetting>();
			cubeModel = "Clutter\\Dwemer\\DweLexiconCubeRunes01.nif";
			logger::info("  > Target is Aetherium version"sv);
		}

		if (!automatonMagEff) {
			logger::info("  > Warning! The spawn magic effect for this automaton is missing or incorrect somehow, aborting...");
			return;
		}

		RE::Effect eff;
		eff.baseEffect = automatonMagEff;
		eff.SetMagnitude(10.0);
		eff.SetDuration(1.0);
		effects.push_back(eff);

		//Check for any active upgrades on this automaton and cache them
		int indexUpgrade = 0;
		std::vector<RE::EffectSetting*> upgradeMagEffsToApply;
		for (auto* form : ListActiveUpgradeMagEffs->forms)
		{
			const auto activeUpgradeMagEff = form->As<RE::EffectSetting>();
			if (automaton->HasMagicEffect(activeUpgradeMagEff)) {
				RE::EffectSetting* spawnUpgradeMagEff = ListSpawnUpgradeMagEffs->forms[indexUpgrade]->As<RE::EffectSetting>();
				if (!spawnUpgradeMagEff) continue;

				upgradeMagEffsToApply.push_back(spawnUpgradeMagEff);
				logger::info("  > Found an upgrade on this automaton: {}"sv, activeUpgradeMagEff->GetName());
			}
			indexUpgrade += 1;
		}

		//Apply any spawn magEffs to the effects list
		if (upgradeMagEffsToApply.size() > 0 && upgradeMagEffsToApply.size() < 100)
		{
			//Start with a ( in the description
			RE::Effect effStart;
			effStart.baseEffect = MagEffSpawnUpgradeStart;
			effects.push_back(effStart);

			indexUpgrade = 0;
			for (auto* upgradeMagEff : upgradeMagEffsToApply)
			{
				RE::Effect effUp;
				effUp.baseEffect = upgradeMagEff;
				effUp.SetDuration(1.0);
				effects.push_back(effUp);
				indexUpgrade += 1;

				//Spacer | between upgrade descriptions
				if (indexUpgrade < upgradeMagEffsToApply.size())
				{
					RE::Effect effSpacer;
					effSpacer.baseEffect = MagEffSpawnUpgradeSpacer;
					effects.push_back(effSpacer);
				}
			}
			//End with a ) in the description
			RE::Effect effEnd;
			effEnd.baseEffect = MagEffSpawnUpgradeEnd;
			effects.push_back(effEnd);
		}

		//Fetch the cube weight
		if (indexRace < ListRaceWeights->forms.size())
		{
			auto glo = ListRaceWeights->forms[indexRace]->As<RE::TESGlobal>();
			if (glo) weight = glo->value;
		}
		
		//Pass in the effects to create the potion
		CreatePotionFromEffects(automatonMagEff, effects, cubeModel, weight, value + 100.0);
		logger::info("  > Successfully created a stasis cube!"sv);
	}

	void Smithing::CreatePotionFromEffects(RE::EffectSetting* mainSpawnMagEff, RE::BSTArray<RE::Effect> effects, std::string modelName, float weight, float value)
	{
		RE::CreatedObjPtr<RE::AlchemyItem> out;

		auto* com = RE::BGSCreatedObjectManager::GetSingleton();
		com->AddPotion(out, effects);

		std::string name = std::format("Automaton Stasis Cube: {}", mainSpawnMagEff->GetName());
		out.get()->SetFullName(name.c_str());
		out.get()->weight = weight;
		//out.get()->data.flags.reset(RE::AlchemyItem::AlchemyFlag::kCostOverride);
		//out.get()->data.costOverride = value;
		out->SetModel(modelName.c_str());
		out.get()->AddKeyword(KeywordCraftingAutomaton);
		out.get()->AddKeyword(KeywordCraftingSmithingForge);
		out.get()->AddKeyword(KeywordVendorItemClutter);
		out.get()->AddKeyword(KeywordVendorItemTool);

		// Important: Increment refs by 1 for each item created (see `51354` + `0x2EC` in 1.6)
		com->IncrementRef(out.get());
		RE::PlayerCharacter::GetSingleton()->AddObjectToContainer(out.get(), nullptr, 1, nullptr);

		RE::SendHUDMessage::ShowHUDMessage(std::format("Added {}", name).c_str());
	}

}