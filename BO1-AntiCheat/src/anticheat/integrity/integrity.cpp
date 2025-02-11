#include "integrity.hpp"
#include "injections.hpp"

namespace anticheat {
	namespace integrity {
		bool Initialize()
		{
			return injections::Initialize();
		}

		void Cleanup()
		{
			injections::Cleanup();
		}
	}
}