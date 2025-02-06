#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace anticheat {
	namespace integrity {
		namespace config {
			struct ConfigDvar {
				std::string name;
				std::string expected_line = ""; // sometimes a dvar is in the vanilla config
			};

			void InitializeConfigQueue();
			std::string GetCheatingCommands();
		} // config
	} // integrity
} // anticheat