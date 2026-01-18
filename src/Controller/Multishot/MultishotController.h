#pragma once

namespace Controller
{
	namespace Multishot
	{
		class MultishotController : public REX::Singleton<MultishotController>
		{
		public:
			void AttemptMultishot(RE::Projectile::LaunchData& a_baseData, RE::ProjectileHandle* a_originalHandle);
		};
	}
}