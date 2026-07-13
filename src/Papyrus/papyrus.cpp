#include "papyrus.h"
#include "Hooks/Pickpocket.h"
#include "hooks/Smithing.h"
#include "Utils/ArmorUtils.h"

namespace Papyrus
{
	std::vector<int> GetVersion(STATIC_ARGS) {
		return { Plugin::VERSION[0], Plugin::VERSION[1], Plugin::VERSION[2] };
	}

	//----------------------------------------------------------------------------------------------------
	static void UpdateRacesAllowPickpocket(STATIC_ARGS)
	{
		Hooks::Pickpocket::SetRacesAllowPickpocket();
	}

	//----------------------------------------------------------------------------------------------------
	std::vector< RE::TESObjectARMO*> GetAllEquippedArmor(STATIC_ARGS, RE::Actor* a_actor)
	{
		return Utils::ArmorUtils::GetEquippedArmor(a_actor);
	}

	//----------------------------------------------------------------------------------------------------
	static void CreateStasisCubeFromAutomaton(STATIC_ARGS, RE::Actor* automaton)
	{
		Hooks::Smithing::CreateStasisCubeFromAutomaton(automaton);
	}

	//----------------------------------------------------------------------------------------------------
	/*
	static void FindAllReferencesOfTypeAndShowVFX(STATIC_ARGS, RE::TESObjectREFR* a_ref, const RE::TESForm* a_formOrList, float a_radius, RE::BGSReferenceEffect* a_vfx)
	{
		if (!a_formOrList) {
			logger::error("FindAllReferencesOfTypeAndShowFX() failed, a_formOrList is NONE");
			return;
		}

		if (const auto TES = RE::TES::GetSingleton(); TES) {
			const auto list = a_formOrList->As<RE::BGSListForm>();

			TES->ForEachReferenceInRange(a_ref, a_radius, [&](RE::TESObjectREFR* b_ref) {
				if (const auto base = b_ref->GetBaseObject(); base && b_ref->Is3DLoaded()) {
					if (list && list->HasForm(base) || a_formOrList == base) {
						b_ref->ApplyArtObject(a_vfx->data.artObject);
						b_ref->ApplyEffectShader(a_vfx->data.effectShader);
					}
				}
				return RE::BSContainer::ForEachResult::kContinue;
			});
		}
	}
	

	void StopVisualEffect(RE::BGSReferenceEffect* effect, RE::TESObjectREFR* target) {
		if (effect && target) {
			if (static auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
				if (auto artObject = effect->data.artObject; artObject) {
					using StopArtObject_t = void(*)(RE::ProcessLists*, RE::TESObjectREFR*, RE::BGSArtObject*);
					static REL::Relocation<StopArtObject_t> StopArtObject{ REL::ID(41396) }; //REL::VariantID(40382, 41396, 0x7048E0)
					StopArtObject(processLists, target, artObject);
				}
				if (auto effectShader = effect->data.effectShader; effectShader) {
					using StopEffectShader_t = void(*)(RE::ProcessLists*, RE::TESObjectREFR*, RE::TESEffectShader*);
					static REL::Relocation<StopEffectShader_t> StopEffectShader{ REL::ID(41395) }; //REL::VariantID(40381, 41395, 0x7047D0)
					StopEffectShader(processLists, target, effectShader);
				}
			}
		}
	}
	*/

	//----------------------------------------------------------------------------------------------------
	void Bind(VM& a_vm) {
		logger::info("  >Binding GetVersion..."sv);
		BIND(GetVersion);
		logger::info("  >Binding UpdateRacesAllowPickpocket..."sv);
		BIND(UpdateRacesAllowPickpocket);
		logger::info("  >Binding GetAllEquippedArmor..."sv);
		BIND(GetAllEquippedArmor);
		logger::info("  >Binding CreateStasisCubeFromAutomaton..."sv);
		BIND(CreateStasisCubeFromAutomaton);
	}

	bool RegisterFunctions(VM* a_vm) {
		logger::info("Binding papyrus functions in utility script {}..."sv, script);
		Bind(*a_vm);
		logger::info("Finished binding functions."sv);
		return true;
	}
}
