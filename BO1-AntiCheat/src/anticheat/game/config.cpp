#include "config.hpp"

#include "game.hpp"
#include "../utils/strings.hpp"
#include "../utils/files.hpp"

namespace anticheat {
	namespace game {
		namespace config {
			int last_config_size = 0;
			std::vector<ConfigDvar> cheating_commands;

			// tells the anticheat the commands to search for and how to search for them.
			void InitializeConfigQueue()
			{
				ConfigDvar cl_noprint = { "cl_noprint" };
				ConfigDvar magic_chest_movable = { "magic_chest_movable" };
				ConfigDvar ai_disableSpawn = { "ai_disableSpawn" };
				ConfigDvar difficultyEasy = { "difficultyEasy" };
				ConfigDvar difficultyMedium = { "difficultyMedium" };
				ConfigDvar difficultyHard = { "difficultyHard" };
				ConfigDvar difficultyFu = { "difficultyFu" };
				ConfigDvar zombie_taunt_freq = { "zombie_taunt_freq" };
				ConfigDvar zombie_reachin_freq = { "zombie_reachin_freq" };
				ConfigDvar scr_force_weapon = { "scr_force_weapon" };
				ConfigDvar scr_force_quantum_bomb_result = { "scr_force_quantum_bomb_result" };

				// damage indicator icons
				ConfigDvar cg_hudDamageIconTime = { "cg_hudDamageIconTime", "seta cg_hudDamageIconTime \"2000\"" };

				ConfigDvar grenadeBounceRestitutionMax = { "grenadeBounceRestitutionMax" };
				ConfigDvar grenadeBumpFreq = { "grenadeBumpFreq" };
				ConfigDvar grenadeBumpMag = { "grenadeBumpMag" };
				ConfigDvar grenadeBumpMax = { "grenadeBumpMax" };
				ConfigDvar grenadeCurveMax = { "grenadeCurveMax" };
				ConfigDvar grenadeFrictionHigh = { "grenadeFrictionHigh" };
				ConfigDvar grenadeFrictionLow = { "grenadeFrictionLow" };
				ConfigDvar grenadeFrictionMaxThresh = { "grenadeFrictionMaxThresh" };
				ConfigDvar grenadeRestThreshold = { "grenadeRestThreshold" };
				ConfigDvar grenadeRollingEnabled = { "grenadeRollingEnabled" };
				ConfigDvar grenadeWobbleFreq = { "grenadeWobbleFreq" };
				ConfigDvar grenadeWobbleFwdMag = { "grenadeWobbleFwdMag" };
				ConfigDvar grenadeWobbleSideDamp = { "grenadeWobbleSideDamp" };
				ConfigDvar grenadeWobbleSideMag = { "grenadeWobbleSideMag" };

				cheating_commands = {
					cl_noprint,
					magic_chest_movable,
					ai_disableSpawn,
					difficultyEasy,
					difficultyMedium,
					difficultyHard,
					difficultyFu,
					zombie_taunt_freq,
					zombie_reachin_freq,
					scr_force_weapon,
					scr_force_quantum_bomb_result,
					cg_hudDamageIconTime,
					grenadeBounceRestitutionMax,
					grenadeBumpFreq,
					grenadeBumpMag,
					grenadeBumpMax,
					grenadeCurveMax,
					grenadeFrictionHigh,
					grenadeFrictionLow,
					grenadeFrictionMaxThresh,
					grenadeRestThreshold,
					grenadeRollingEnabled,
					grenadeWobbleFreq,
					grenadeWobbleFwdMag,
					grenadeWobbleSideDamp,
					grenadeWobbleSideMag
				};
			}

			// reads the config for cheating commands every map load
			std::string GetCheatingCommands()
			{
				std::string config_path = game::GetGamePath() + "\\players\\config.cfg";

				// check if the config exists
				if (!std::filesystem::exists(config_path))
				{
					return "";
				}

				// check the size of the config and store it, prevents constant file readers
				int config_bytes = utils::files::SizeInBytes(config_path);

				// something went wrong with reading the file
				if (config_bytes == -1)
				{
					return "";
				}

				// nothing has changed
				if (config_bytes == last_config_size)
				{
					return "";
				}

				// set the last config size - we're reading it.
				last_config_size = config_bytes;

				std::ifstream file(config_path);
				if (!file.is_open())
				{
					return "";
				}

				std::vector<std::string> config_commands;

				// go through each line
				std::string line;
				while (std::getline(file, line)) {
					std::string line_lower = utils::strings::ToLower(line);

					for (ConfigDvar dvar : cheating_commands)
					{
						std::string dvar_name = dvar.name;
						std::string dvar_name_lower = utils::strings::ToLower(dvar.name);
						bool contains_command = utils::strings::Contains(line_lower, dvar_name_lower);

						if (contains_command)
						{
							// we have to make sure the default values arent tampered with:
							if (dvar.expected_line != "")
							{
								if (line_lower != utils::strings::ToLower(dvar.expected_line))
								{
									config_commands.push_back(dvar_name + " (Value changed)");
								}
							}
							// for lines that should not exist at all
							else {
								config_commands.push_back(dvar_name);
							}
						}
					}
				}

				file.close();
				return utils::strings::FormatVector(config_commands);
			}
		} // config
	} // game
} // anticheat