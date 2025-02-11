#include <string>

namespace anticheat {
	namespace integrity {
		bool Initialize();
		void StartChecksThread();
		void StopChecksThread();
		void Cleanup();
	}
}