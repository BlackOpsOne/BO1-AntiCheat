#include <string>
#include <vector>

struct CheatingProcess
{
	std::string cheat;
	std::string value;
};

namespace anticheat {
	namespace integrity {
		namespace processes {
			bool Initialize();
			void FilterWindowTitles();
			void FilterProcessNames();
			bool AlreadyDetected(std::string target_cheat);
			std::string GetBlacklistedPrograms();
		}
	}
}