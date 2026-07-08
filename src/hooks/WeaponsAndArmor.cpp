#include "WeaponsAndArmor.h"

using namespace RE;

namespace Hooks
{
	void WeaponsAndArmor::AddKeywordsToArmor()
	{

	}

	RE::TESForm* GetFormFromIdentifier(const std::string& identifier)
	{
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		auto delimiter = identifier.find('|');
		if (delimiter != std::string::npos) {
			// Handle the case when '|' is present in the identifier
			std::string modName = identifier.substr(0, delimiter);
			std::string modForm = identifier.substr(delimiter + 1);
			uint32_t    formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
			auto* mod = dataHandler->LookupModByName(modName.c_str());
			if (mod && mod->IsLight())
				formID = std::stoul(modForm, nullptr, 16) & 0xFFF;
			return dataHandler->LookupForm(formID, modName.c_str());
		}
		else {
			return RE::TESForm::LookupByEditorID(identifier);
		}
		return nullptr;
	}
}
