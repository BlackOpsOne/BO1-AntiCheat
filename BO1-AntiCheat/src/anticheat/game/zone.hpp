#include <string>
#include <vector>

struct ZoneEntry {
	std::string ff_name;
	std::string checksum;
};

struct MapZoneEntry {
	std::string map_name;
	std::vector<ZoneEntry> zone_entries;
};

namespace anticheat {
	namespace game {
		namespace zone {
			// initialize the zone files queue
			void InitializeZoneQueue();

			// checks zone/ for any extra folder that should not be there
			std::string CheckForExtraItemsInZone();

			// checks zone/<zone_name> for extra files that should not be there
			std::string GetExtraFilesInZone(std::string zone_name);

			// checks every fastfile for modification
			std::string GetModifiedFastFiles(std::string map_name);

			// make sure a .ff is in use
			bool IsLangUsingFF(std::string ff_path);

			// checks if the ff exists in the first place
			bool IsFFValid(std::string ff_path);
		}
	}
}