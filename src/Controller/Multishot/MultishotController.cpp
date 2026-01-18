#include "MultishotController.h"

#include "Settings/INI/INISettings.h"

namespace Controller::Multishot
{
	void MultishotController::AttemptMultishot(RE::Projectile::LaunchData& a_baseData, 
		RE::ProjectileHandle* a_originalHandle)
	{
		if (!a_originalHandle) {
			return;
		}

		auto* shooter = a_baseData.shooter ? a_baseData.shooter->As<RE::Actor>() : nullptr;
		auto* ammo = a_baseData.ammoSource;
		auto* root = shooter ? shooter->GetCurrent3D() : nullptr;
		auto* fireNode = root ? root->GetObjectByName(RE::FixedStrings::GetSingleton()->npcRMagicNode) : nullptr;
		if (!fireNode || !ammo) {
			return;
		}

		RE::Projectile::ProjectileRot angles{};
		RE::NiPoint3 shotOrigin{};

		// Credit: PowerOfThree's Extender source code
		if (shooter->IsPlayerRef()) {
			angles.z = shooter->GetAimHeading();

			float tiltUpAngle;
			if (ammo->IsBolt()) {
				tiltUpAngle = "f1PBoltTiltUpAngle:Combat"_ini.value();
			}
			else {
				tiltUpAngle = RE::PlayerCamera::GetSingleton()->IsInFirstPerson() ? "f1PArrowTiltUpAngle:Combat"_ini.value() : "f3PArrowTiltUpAngle:Combat"_ini.value();
			}
			angles.x = shooter->GetAngleX() - (RE::deg_to_rad(tiltUpAngle));

			shotOrigin = fireNode->world.translate;
			shooter->Unk_117(shotOrigin);
		}
		else {
			shotOrigin = fireNode->world.translate;
			shooter->Unk_A0(fireNode, angles.x, angles.z, shotOrigin);
		}

#ifndef NDEBUG
		if (!shooter->IsPlayerRef()) {
			LOG_DEBUG("Multishot: Fired by non-player in debug, disabled."sv);
			return;
		}

		float spreadBy = 15.0f;
		int shotCount = 2;

		auto maxCount = Settings::INI::GetSetting<long>(Settings::INI::MARKSMAN_MULTISHOT_MAXPROJECTILES).value_or(5);
		auto minCone = Settings::INI::GetSetting<float>(Settings::INI::MARKSMAN_MULTISHOT_MINCONE).value_or(15.0f);
		auto maxCone = Settings::INI::GetSetting<float>(Settings::INI::MARKSMAN_MULTISHOT_MAXCONE).value_or(180.0f);

		if (spreadBy < minCone) {
			spreadBy = minCone;
		}
		else if (spreadBy > maxCone) {
			spreadBy = maxCone;
		}
		if (shotCount > maxCount) {
			shotCount = maxCount;
		}

		for (int i = 0; i < shotCount; ++i) {
			RE::ProjectileHandle handle{};
			float angleOffset = spreadBy * (i - (shotCount - 1) / 2.0f);
			RE::Projectile::ProjectileRot modifiedAngles = angles;
			modifiedAngles.z += RE::deg_to_rad(angleOffset);
			RE::Projectile::LaunchData additionalShotData = a_baseData;
			additionalShotData.origin = shotOrigin;
			additionalShotData.angleX = modifiedAngles.x;
			additionalShotData.angleZ = modifiedAngles.z;
			RE::Projectile::Launch(&handle, additionalShotData);

			handle.get()->power = a_originalHandle->get()->power;
			handle.get()->weaponDamage = a_originalHandle->get()->weaponDamage;
		}
#endif
	}
}