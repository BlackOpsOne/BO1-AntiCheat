#include <string>

namespace anticheat {
	namespace integrity {
		namespace zone {
			// initialize the zone files queue
			void InitializeZoneQueue();

			// checks zone/ for any extra folder that should not be there
			std::string CheckForExtraItemsInZone();

			// checks zone/<zone_name> for extra files that should not be there
			std::string GetExtraFilesInZone(std::string zone_name);

			// checks every fastfile for modification
			std::string GetModifiedFastFiles();

			// make sure a .ff is in use
			bool IsFFInUse(std::string ff_path);
		}
	}
}