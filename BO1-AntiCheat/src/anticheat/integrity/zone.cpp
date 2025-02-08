#include "zone.hpp"

#include "../../game/game.hpp"

#include "../../game/process.hpp"

#include "checksums.h"

#include "../../utils/files.hpp"

#include "../../utils/strings.hpp"

#include <vector>

#include <map>

#include <filesystem>

using namespace std;

vector<ZoneEntry> global_zone_queue; // global files that the game loads no matter the map
vector<MapZoneEntry> map_zone_queue; // map specific files

namespace fs = std::filesystem;

namespace anticheat {
    namespace integrity {
        namespace zone {

            void InitializeZoneQueue()
            {
                map_zone_queue = {
                    // Kino Der Toten
                    {
                        "zombie_theater",
                        {
                            { "zone\\Common\\zombie_theater.ff", Checksums::ZOMBIE_THEATER },
                            { "zone\\Common\\zombie_theater_patch.ff", Checksums::ZOMBIE_THEATER_PATCH },
                            { "zone\\English\\en_zombie_theater.ff", Checksums::EN_ZOMBIE_THEATER },
                            { "zone\\Japanese\\ja_zombie_theater.ff", Checksums::JA_ZOMBIE_THEATER },
                            { "zone\\French\\fr_zombie_theater.ff", Checksums::FR_ZOMBIE_THEATER },
                            { "zone\\German\\ge_zombie_theater.ff", Checksums::GE_ZOMBIE_THEATER },
                            { "zone\\Italian\\it_zombie_theater.ff", Checksums::IT_ZOMBIE_THEATER },
                            { "zone\\Spanish\\sp_zombie_theater.ff", Checksums::SP_ZOMBIE_THEATER }
                        }
                    },

                    // Five
                    {
                        "zombie_pentagon",
                        {
                            { "zone\\Common\\zombie_pentagon.ff", Checksums::ZOMBIE_PENTAGON },
                            { "zone\\Common\\zombie_pentagon_patch.ff", Checksums::ZOMBIE_PENTAGON_PATCH },
                            { "zone\\English\\en_zombie_pentagon.ff", Checksums::EN_ZOMBIE_PENTAGON },
                            { "zone\\Japanese\\ja_zombie_pentagon.ff", Checksums::JA_ZOMBIE_PENTAGON },
                            { "zone\\French\\fr_zombie_pentagon.ff", Checksums::FR_ZOMBIE_PENTAGON },
                            { "zone\\German\\ge_zombie_pentagon.ff", Checksums::GE_ZOMBIE_PENTAGON },
                            { "zone\\Italian\\it_zombie_pentagon.ff", Checksums::IT_ZOMBIE_PENTAGON },
                            { "zone\\Spanish\\sp_zombie_pentagon.ff", Checksums::SP_ZOMBIE_PENTAGON }
                        }
                    },

                    // Dead Ops Arcade
                    {
                        "zombietron",
                        {
                            { "zone\\Common\\zombietron.ff", Checksums::ZOMBIETRON },
                            { "zone\\Common\\zombietron_patch.ff", Checksums::ZOMBIETRON_PATCH },
                            { "zone\\English\\en_zombietron.ff", Checksums::EN_ZOMBIETRON },
                            { "zone\\Japanese\\ja_zombietron.ff", Checksums::JA_ZOMBIETRON },
                            { "zone\\French\\fr_zombietron.ff", Checksums::FR_ZOMBIETRON },
                            { "zone\\German\\ge_zombietron.ff", Checksums::GE_ZOMBIETRON },
                            { "zone\\Italian\\it_zombietron.ff", Checksums::IT_ZOMBIETRON },
                            { "zone\\Spanish\\sp_zombietron.ff", Checksums::SP_ZOMBIETRON }
                        }
                    },

                    // Ascension
                    {
                        "zombie_cosmodrome",
                        {
                            { "zone\\Common\\zombie_cosmodrome.ff", Checksums::ZOMBIE_COSMODROME },
                            { "zone\\Common\\zombie_cosmodrome_patch.ff", Checksums::ZOMBIE_COSMODROME_PATCH },
                            { "zone\\English\\en_zombie_cosmodrome.ff", Checksums::EN_ZOMBIE_COSMODROME },
                            { "zone\\Japanese\\ja_zombie_cosmodrome.ff", Checksums::JA_ZOMBIE_COSMODROME },
                            { "zone\\French\\fr_zombie_cosmodrome.ff", Checksums::FR_ZOMBIE_COSMODROME },
                            { "zone\\German\\ge_zombie_cosmodrome.ff", Checksums::GE_ZOMBIE_COSMODROME },
                            { "zone\\Italian\\it_zombie_cosmodrome.ff", Checksums::IT_ZOMBIE_COSMODROME },
                            { "zone\\Spanish\\sp_zombie_cosmodrome.ff", Checksums::SP_ZOMBIE_COSMODROME }
                        }
                    },

                    // Call of The Dead
                    {
                        "zombie_coast",
                        {
                            { "zone\\Common\\zombie_coast.ff", Checksums::ZOMBIE_COAST },
                            { "zone\\Common\\zombie_coast_patch.ff", Checksums::ZOMBIE_COAST_PATCH },
                            { "zone\\English\\en_zombie_coast.ff", Checksums::EN_ZOMBIE_COAST },
                            { "zone\\Japanese\\ja_zombie_coast.ff", Checksums::JA_ZOMBIE_COAST },
                            { "zone\\French\\fr_zombie_coast.ff", Checksums::FR_ZOMBIE_COAST },
                            { "zone\\German\\ge_zombie_coast.ff", Checksums::GE_ZOMBIE_COAST },
                            { "zone\\Italian\\it_zombie_coast.ff", Checksums::IT_ZOMBIE_COAST },
                            { "zone\\Spanish\\sp_zombie_coast.ff", Checksums::SP_ZOMBIE_COAST }
                        }
                    },

                    // Shangri-La
                    {
                        "zombie_temple",
                        {
                            { "zone\\Common\\zombie_temple.ff", Checksums::ZOMBIE_TEMPLE },
                            { "zone\\Common\\zombie_temple_patch.ff", Checksums::ZOMBIE_TEMPLE_PATCH },
                            { "zone\\English\\en_zombie_temple.ff", Checksums::EN_ZOMBIE_TEMPLE },
                            { "zone\\Japanese\\ja_zombie_temple.ff", Checksums::JA_ZOMBIE_TEMPLE },
                            { "zone\\French\\fr_zombie_temple.ff", Checksums::FR_ZOMBIE_TEMPLE },
                            { "zone\\German\\ge_zombie_temple.ff", Checksums::GE_ZOMBIE_TEMPLE },
                            { "zone\\Italian\\it_zombie_temple.ff", Checksums::IT_ZOMBIE_TEMPLE },
                            { "zone\\Spanish\\sp_zombie_temple.ff", Checksums::SP_ZOMBIE_TEMPLE }
                        }
                    },

                    // Moon
                    {
                        "zombie_moon",
                        {
                            { "zone\\Common\\zombie_moon.ff", Checksums::ZOMBIE_MOON },
                            { "zone\\Common\\zombie_moon_patch.ff", Checksums::ZOMBIE_MOON_PATCH },
                            { "zone\\English\\en_zombie_moon.ff", Checksums::EN_ZOMBIE_MOON },
                            { "zone\\Japanese\\ja_zombie_moon.ff", Checksums::JA_ZOMBIE_MOON },
                            { "zone\\French\\fr_zombie_moon.ff", Checksums::FR_ZOMBIE_MOON },
                            { "zone\\German\\ge_zombie_moon.ff", Checksums::GE_ZOMBIE_MOON },
                            { "zone\\Italian\\it_zombie_moon.ff", Checksums::IT_ZOMBIE_MOON },
                            { "zone\\Spanish\\sp_zombie_moon.ff", Checksums::SP_ZOMBIE_MOON }
                        }
                    },

                    // Nacht Der Untoten
                    {
                        "zombie_cod5_prototype",
                        {
                            { "zone\\Common\\zombie_cod5_prototype.ff", Checksums::ZOMBIE_COD5_PROTOTYPE },
                            { "zone\\Common\\zombie_cod5_prototype_patch.ff", Checksums::ZOMBIE_COD5_PROTOTYPE_PATCH },
                            { "zone\\English\\en_zombie_cod5_prototype.ff", Checksums::EN_ZOMBIE_COD5_PROTOTYPE },
                            { "zone\\Japanese\\ja_zombie_cod5_prototype.ff", Checksums::JA_ZOMBIE_COD5_PROTOTYPE },
                            { "zone\\French\\fr_zombie_cod5_prototype.ff", Checksums::FR_ZOMBIE_COD5_PROTOTYPE },
                            { "zone\\German\\ge_zombie_cod5_prototype.ff", Checksums::GE_ZOMBIE_COD5_PROTOTYPE },
                            { "zone\\Italian\\it_zombie_cod5_prototype.ff", Checksums::IT_ZOMBIE_COD5_PROTOTYPE },
                            { "zone\\Spanish\\sp_zombie_cod5_prototype.ff", Checksums::SP_ZOMBIE_COD5_PROTOTYPE }
                        }
                    },

                    // Verruckt
                    {
                        "zombie_cod5_asylum",
                        {
                            { "zone\\Common\\zombie_cod5_asylum.ff", Checksums::ZOMBIE_COD5_ASYLUM },
                            { "zone\\Common\\zombie_cod5_asylum_patch.ff", Checksums::ZOMBIE_COD5_ASYLUM_PATCH },
                            { "zone\\English\\en_zombie_cod5_asylum.ff", Checksums::EN_ZOMBIE_COD5_ASYLUM },
                            { "zone\\Japanese\\ja_zombie_cod5_asylum.ff", Checksums::JA_ZOMBIE_COD5_ASYLUM },
                            { "zone\\French\\fr_zombie_cod5_asylum.ff", Checksums::FR_ZOMBIE_COD5_ASYLUM },
                            { "zone\\German\\ge_zombie_cod5_asylum.ff", Checksums::GE_ZOMBIE_COD5_ASYLUM },
                            { "zone\\Italian\\it_zombie_cod5_asylum.ff", Checksums::IT_ZOMBIE_COD5_ASYLUM },
                            { "zone\\Spanish\\sp_zombie_cod5_asylum.ff", Checksums::SP_ZOMBIE_COD5_ASYLUM }
                        }
                    },

                    // Shi No Numa
                    {
                        "zombie_cod5_sumpf",
                        {
                            { "zone\\Common\\zombie_cod5_sumpf.ff", Checksums::ZOMBIE_COD5_SUMPF },
                            { "zone\\Common\\zombie_cod5_sumpf_patch.ff", Checksums::ZOMBIE_COD5_SUMPF_PATCH },
                            { "zone\\English\\en_zombie_cod5_sumpf.ff", Checksums::EN_ZOMBIE_COD5_SUMPF },
                            { "zone\\Japanese\\ja_zombie_cod5_sumpf.ff", Checksums::JA_ZOMBIE_COD5_SUMPF },
                            { "zone\\French\\fr_zombie_cod5_sumpf.ff", Checksums::FR_ZOMBIE_COD5_SUMPF },
                            { "zone\\German\\ge_zombie_cod5_sumpf.ff", Checksums::GE_ZOMBIE_COD5_SUMPF },
                            { "zone\\Italian\\it_zombie_cod5_sumpf.ff", Checksums::IT_ZOMBIE_COD5_SUMPF },
                            { "zone\\Spanish\\sp_zombie_cod5_sumpf.ff", Checksums::SP_ZOMBIE_COD5_SUMPF }
                        }
                    },

                    // Der Riese
                    {
                        "zombie_cod5_factory",
                        {
                            { "zone\\Common\\zombie_cod5_factory.ff", Checksums::ZOMBIE_COD5_FACTORY },
                            { "zone\\Common\\zombie_cod5_factory_patch.ff", Checksums::ZOMBIE_COD5_FACTORY_PATCH },
                            { "zone\\English\\en_zombie_cod5_factory.ff", Checksums::EN_ZOMBIE_COD5_FACTORY },
                            { "zone\\Japanese\\ja_zombie_cod5_factory.ff", Checksums::JA_ZOMBIE_COD5_FACTORY },
                            { "zone\\French\\fr_zombie_cod5_factory.ff", Checksums::FR_ZOMBIE_COD5_FACTORY },
                            { "zone\\German\\ge_zombie_cod5_factory.ff", Checksums::GE_ZOMBIE_COD5_FACTORY },
                            { "zone\\Italian\\it_zombie_cod5_factory.ff", Checksums::IT_ZOMBIE_COD5_FACTORY },
                            { "zone\\Spanish\\sp_zombie_cod5_factory.ff", Checksums::SP_ZOMBIE_COD5_FACTORY }
                        }
                    },
                };

                global_zone_queue = {
                    // Common
                    { "zone\\Common\\code_post_gfx.ff", Checksums::CODE_POST_GFX },
                    { "zone\\Common\\code_post_gfx_mp.ff", Checksums::CODE_POST_GFX_MP },
                    { "zone\\Common\\code_pre_gfx.ff", Checksums::CODE_PRE_GFX },
                    { "zone\\Common\\code_pre_gfx_mp.ff", Checksums::CODE_PRE_GFX_MP },
                    { "zone\\Common\\frontend_patch.ff", Checksums::GAME_MOD_FRONTEND },
                    { "zone\\Common\\common.ff", Checksums::COMMON },
                    { "zone\\Common\\common_zombie.ff", Checksums::COMMON_ZOMBIE },
                    { "zone\\Common\\common_zombie_patch.ff", Checksums::COMMON_ZOMBIE_PATCH },

                    // English
                    { "zone\\English\\en_frontend.ff", Checksums::EN_FRONTEND },
                    { "zone\\English\\en_code_post_gfx.ff", Checksums::EN_CODE_POST_GFX },
                    { "zone\\English\\en_code_post_gfx_mp.ff", Checksums::EN_CODE_POST_GFX_MP },
                    { "zone\\English\\en_code_pre_gfx.ff", Checksums::EN_CODE_PRE_GFX },
                    { "zone\\English\\en_code_pre_gfx_mp.ff", Checksums::EN_CODE_PRE_GFX_MP },
                    { "zone\\English\\en_common.ff", Checksums::EN_COMMON },
                    { "zone\\English\\en_common_zombie.ff", Checksums::EN_COMMON_ZOMBIE },

                    // Japanese
                    { "zone\\Japanese\\ja_frontend.ff", Checksums::JA_FRONTEND },
                    { "zone\\Japanese\\ja_code_post_gfx.ff", Checksums::JA_CODE_POST_GFX },
                    { "zone\\Japanese\\ja_code_post_gfx_mp.ff", Checksums::JA_CODE_POST_GFX_MP },
                    { "zone\\Japanese\\ja_code_pre_gfx.ff", Checksums::JA_CODE_PRE_GFX },
                    { "zone\\Japanese\\ja_code_pre_gfx_mp.ff", Checksums::JA_CODE_PRE_GFX_MP },
                    { "zone\\Japanese\\ja_common.ff", Checksums::JA_COMMON },
                    { "zone\\Japanese\\ja_common_zombie.ff", Checksums::JA_COMMON_ZOMBIE },

                    // French
                    { "zone\\French\\fr_frontend.ff", Checksums::FR_FRONTEND },
                    { "zone\\French\\fr_code_post_gfx.ff", Checksums::FR_CODE_POST_GFX },
                    { "zone\\French\\fr_code_post_gfx_mp.ff", Checksums::FR_CODE_POST_GFX_MP },
                    { "zone\\French\\fr_code_pre_gfx.ff", Checksums::FR_CODE_PRE_GFX },
                    { "zone\\French\\fr_code_pre_gfx_mp.ff", Checksums::FR_CODE_PRE_GFX_MP },
                    { "zone\\French\\fr_common.ff", Checksums::FR_COMMON },
                    { "zone\\French\\fr_common_zombie.ff", Checksums::FR_COMMON_ZOMBIE },

                    // German
                    { "zone\\German\\ge_frontend.ff", Checksums::GE_FRONTEND },
                    { "zone\\German\\ge_code_post_gfx.ff", Checksums::GE_CODE_POST_GFX },
                    { "zone\\German\\ge_code_post_gfx_mp.ff", Checksums::GE_CODE_POST_GFX_MP },
                    { "zone\\German\\ge_code_pre_gfx.ff", Checksums::GE_CODE_PRE_GFX },
                    { "zone\\German\\ge_code_pre_gfx_mp.ff", Checksums::GE_CODE_PRE_GFX_MP },
                    { "zone\\German\\ge_common.ff", Checksums::GE_COMMON },
                    { "zone\\German\\ge_common_zombie.ff", Checksums::GE_COMMON_ZOMBIE },

                    // Italian
                    { "zone\\Italian\\it_frontend.ff", Checksums::IT_FRONTEND },
                    { "zone\\Italian\\it_code_post_gfx.ff", Checksums::IT_CODE_POST_GFX },
                    { "zone\\Italian\\it_code_post_gfx_mp.ff", Checksums::IT_CODE_POST_GFX_MP },
                    { "zone\\Italian\\it_code_pre_gfx.ff", Checksums::IT_CODE_PRE_GFX },
                    { "zone\\Italian\\it_code_pre_gfx_mp.ff", Checksums::IT_CODE_PRE_GFX_MP },
                    { "zone\\Italian\\it_common.ff", Checksums::IT_COMMON },
                    { "zone\\Italian\\it_common_zombie.ff", Checksums::IT_COMMON_ZOMBIE },

                    // Spanish
                    { "zone\\Spanish\\sp_frontend.ff", Checksums::SP_FRONTEND },
                    { "zone\\Spanish\\sp_code_post_gfx.ff", Checksums::SP_CODE_POST_GFX },
                    { "zone\\Spanish\\sp_code_post_gfx_mp.ff", Checksums::SP_CODE_POST_GFX_MP },
                    { "zone\\Spanish\\sp_code_pre_gfx.ff", Checksums::SP_CODE_PRE_GFX },
                    { "zone\\Spanish\\sp_code_pre_gfx_mp.ff", Checksums::SP_CODE_PRE_GFX_MP },
                    { "zone\\Spanish\\sp_common.ff", Checksums::SP_COMMON },
                    { "zone\\Spanish\\sp_common_zombie.ff", Checksums::SP_COMMON_ZOMBIE }
                };
            }

            static vector<string> ValidCommonFiles()
            {
                return {
                    "code_post_gfx.ff", "code_post_gfx_mp.ff", "code_pre_gfx.ff",
                    "code_pre_gfx_mp.ff", "common.ff", "common_mp.ff",
                    "common_zombie.ff", "common_zombie_patch.ff", "creek_1.ff",
                    "cuba.ff", "flashpoint.ff", "frontend.ff",
                    /* game_mod related */
                    "frontend_patch.ff",
                    /* Custom-FX Tool */
                    "custom_fx.ff",
                    "fullahead.ff", "hue_city.ff",
                    "int_escape.ff", "khe_sanh.ff", "kowloon.ff",
                    "mp_area51.ff", "mp_array.ff", "mp_berlinwall2.ff",
                    "mp_cairo.ff", "mp_cosmodrome.ff", "mp_cracked.ff",
                    "mp_crisis.ff", "mp_discovery.ff", "mp_drivein.ff",
                    "mp_duga.ff", "mp_firingrange.ff", "mp_golfcourse.ff",
                    "mp_gridlock.ff", "mp_hanoi.ff", "mp_havoc.ff",
                    "mp_hotel.ff", "mp_kowloon.ff", "mp_mountain.ff",
                    "mp_nuked.ff", "mp_outskirts.ff", "mp_radiation.ff",
                    "mp_russianbase.ff", "mp_silo.ff", "mp_stadium.ff",
                    "mp_villa.ff", "mp_zoo.ff", "outro.ff",
                    "patch.ff", "patch_mp.ff", "patch_ui_mp.ff",
                    "pentagon.ff", "pow.ff", "rebirth.ff",
                    "river.ff", "so_narrative1_frontend.ff", "so_narrative2_frontend.ff",
                    "so_narrative3_frontend.ff", "so_narrative4_frontend.ff", "so_narrative5_frontend.ff",
                    "so_narrative6_frontend.ff", "terminal.ff", "ui_mp.ff",
                    "ui_viewer_mp.ff","underwaterbase.ff", "vorkuta.ff",
                    "wmd.ff", "wmd_sr71.ff", "zombietron.ff",
                    "zombietron_patch.ff", "zombie_coast.ff", "zombie_coast_patch.ff",
                    "zombie_cod5_asylum.ff", "zombie_cod5_asylum_patch.ff", "zombie_cod5_factory.ff",
                    "zombie_cod5_factory_patch.ff", "zombie_cod5_prototype.ff", "zombie_cod5_prototype_patch.ff",
                    "zombie_cod5_sumpf.ff", "zombie_cod5_sumpf_patch.ff", "zombie_cosmodrome.ff",
                    "zombie_cosmodrome_patch.ff", "zombie_moon.ff", "zombie_moon_patch.ff",
                    "zombie_pentagon.ff", "zombie_pentagon_patch.ff", "zombie_temple.ff",
                    "zombie_temple_patch.ff", "zombie_theater.ff", "zombie_theater_patch.ff"
                };
            }

            static vector<string> ValidEnglishFiles()
            {
                return {
                    "en_code_post_gfx.ff", "en_code_post_gfx_mp.ff", "en_code_pre_gfx.ff",
                    "en_code_pre_gfx_mp.ff", "en_common.ff", "en_common_mp.ff",
                    "en_common_zombie.ff", "en_creek_1.ff", "en_cuba.ff",
                    "en_flashpoint.ff", "en_frontend.ff", "en_fullahead.ff",
                    "en_hue_city.ff", "en_int_escape.ff", "en_khe_sanh.ff",
                    "en_kowloon.ff", "en_mp_area51.ff", "en_mp_array.ff",
                    "en_mp_berlinwall2.ff", "en_mp_cairo.ff", "en_mp_cosmodrome.ff",
                    "en_mp_cracked.ff", "en_mp_crisis.ff", "en_mp_discovery.ff",
                    "en_mp_drivein.ff", "en_mp_duga.ff", "en_mp_firingrange.ff",
                    "en_mp_golfcourse.ff", "en_mp_gridlock.ff", "en_mp_hanoi.ff",
                    "en_mp_havoc.ff", "en_mp_hotel.ff", "en_mp_kowloon.ff",
                    "en_mp_mountain.ff", "en_mp_nuked.ff", "en_mp_outskirts.ff",
                    "en_mp_radiation.ff", "en_mp_russianbase.ff", "en_mp_silo.ff",
                    "en_mp_stadium.ff", "en_mp_villa.ff", "en_mp_zoo.ff",
                    "en_outro.ff", "en_patch.ff", "en_patch_mp.ff",
                    "en_pentagon.ff", "en_pow.ff", "en_rebirth.ff",
                    "en_river.ff", "en_so_narrative1_frontend.ff", "en_so_narrative2_frontend.ff",
                    "en_so_narrative3_frontend.ff", "en_so_narrative4_frontend.ff", "en_so_narrative5_frontend.ff",
                    "en_so_narrative6_frontend.ff", "en_terminal.ff", "en_ui_mp.ff",
                    "en_underwaterbase.ff", "en_vorkuta.ff", "en_wmd.ff",
                    "en_wmd_sr71.ff", "en_zombietron.ff", "en_zombie_coast.ff",
                    "en_zombie_cod5_asylum.ff", "en_zombie_cod5_factory.ff", "en_zombie_cod5_prototype.ff",
                    "en_zombie_cod5_sumpf.ff", "en_zombie_cosmodrome.ff", "en_zombie_moon.ff",
                    "en_zombie_pentagon.ff", "en_zombie_temple.ff", "en_zombie_theater.ff"
                };
            }

            static vector<string> ValidJapaneseFiles()
            {
                return {
                    "ja_code_post_gfx.ff", "ja_code_post_gfx_mp.ff", "ja_code_pre_gfx.ff",
                    "ja_code_pre_gfx_mp.ff", "ja_common.ff", "ja_common_mp.ff",
                    "ja_common_zombie.ff", "ja_creek_1.ff", "ja_cuba.ff",
                    "ja_flashpoint.ff", "ja_frontend.ff", "ja_fullahead.ff",
                    "ja_hue_city.ff", "ja_int_escape.ff", "ja_khe_sanh.ff",
                    "ja_kowloon.ff", "ja_mp_area51.ff", "ja_mp_array.ff",
                    "ja_mp_berlinwall2.ff", "ja_mp_cairo.ff", "ja_mp_cosmodrome.ff",
                    "ja_mp_cracked.ff", "ja_mp_crisis.ff", "ja_mp_discovery.ff",
                    "ja_mp_drivein.ff", "ja_mp_duga.ff", "ja_mp_firingrange.ff",
                    "ja_mp_golfcourse.ff", "ja_mp_gridlock.ff", "ja_mp_hanoi.ff",
                    "ja_mp_havoc.ff", "ja_mp_hotel.ff", "ja_mp_kowloon.ff",
                    "ja_mp_mountain.ff", "ja_mp_nuked.ff", "ja_mp_outskirts.ff",
                    "ja_mp_radiation.ff", "ja_mp_russianbase.ff", "ja_mp_silo.ff",
                    "ja_mp_stadium.ff", "ja_mp_villa.ff", "ja_mp_zoo.ff",
                    "ja_outro.ff", "ja_patch.ff", "ja_patch_mp.ff",
                    "ja_pentagon.ff", "ja_pow.ff", "ja_rebirth.ff",
                    "ja_river.ff", "ja_so_narrative1_frontend.ff", "ja_so_narrative2_frontend.ff",
                    "ja_so_narrative3_frontend.ff", "ja_so_narrative4_frontend.ff", "ja_so_narrative5_frontend.ff",
                    "ja_so_narrative6_frontend.ff", "ja_terminal.ff", "ja_ui_mp.ff",
                    "ja_underwaterbase.ff", "ja_vorkuta.ff", "ja_wmd.ff",
                    "ja_wmd_sr71.ff", "ja_zombietron.ff", "ja_zombie_coast.ff",
                    "ja_zombie_cod5_asylum.ff", "ja_zombie_cod5_factory.ff", "ja_zombie_cod5_prototype.ff",
                    "ja_zombie_cod5_sumpf.ff", "ja_zombie_cosmodrome.ff", "ja_zombie_moon.ff",
                    "ja_zombie_pentagon.ff", "ja_zombie_temple.ff", "ja_zombie_theater.ff"
                };
            }

            static vector<string> ValidFrenchFiles()
            {
                return {
                    "fr_code_post_gfx.ff", "fr_code_post_gfx_mp.ff", "fr_code_pre_gfx.ff",
                    "fr_code_pre_gfx_mp.ff", "fr_common.ff", "fr_common_mp.ff",
                    "fr_common_zombie.ff", "fr_creek_1.ff", "fr_cuba.ff",
                    "fr_flashpoint.ff", "fr_frontend.ff", "fr_fullahead.ff",
                    "fr_hue_city.ff", "fr_int_escape.ff", "fr_khe_sanh.ff",
                    "fr_kowloon.ff", "fr_mp_area51.ff", "fr_mp_array.ff",
                    "fr_mp_berlinwall2.ff", "fr_mp_cairo.ff", "fr_mp_cosmodrome.ff",
                    "fr_mp_cracked.ff", "fr_mp_crisis.ff", "fr_mp_discovery.ff",
                    "fr_mp_drivein.ff", "fr_mp_duga.ff", "fr_mp_firingrange.ff",
                    "fr_mp_golfcourse.ff", "fr_mp_gridlock.ff", "fr_mp_hanoi.ff",
                    "fr_mp_havoc.ff", "fr_mp_hotel.ff", "fr_mp_kowloon.ff",
                    "fr_mp_mountain.ff", "fr_mp_nuked.ff", "fr_mp_outskirts.ff",
                    "fr_mp_radiation.ff", "fr_mp_russianbase.ff", "fr_mp_silo.ff",
                    "fr_mp_stadium.ff", "fr_mp_villa.ff", "fr_mp_zoo.ff",
                    "fr_outro.ff", "fr_patch.ff", "fr_patch_mp.ff",
                    "fr_pentagon.ff", "fr_pow.ff", "fr_rebirth.ff",
                    "fr_river.ff", "fr_so_narrative1_frontend.ff", "fr_so_narrative2_frontend.ff",
                    "fr_so_narrative3_frontend.ff", "fr_so_narrative4_frontend.ff", "fr_so_narrative5_frontend.ff",
                    "fr_so_narrative6_frontend.ff", "fr_terminal.ff", "fr_ui_mp.ff",
                    "fr_underwaterbase.ff", "fr_vorkuta.ff", "fr_wmd.ff",
                    "fr_wmd_sr71.ff", "fr_zombietron.ff", "fr_zombie_coast.ff",
                    "fr_zombie_cod5_asylum.ff", "fr_zombie_cod5_factory.ff", "fr_zombie_cod5_prototype.ff",
                    "fr_zombie_cod5_sumpf.ff", "fr_zombie_cosmodrome.ff", "fr_zombie_moon.ff",
                    "fr_zombie_pentagon.ff", "fr_zombie_temple.ff", "fr_zombie_theater.ff"
                };
            }

            static vector<string> ValidGermanFiles()
            {
                return {
                    "ge_code_post_gfx.ff", "ge_code_post_gfx_mp.ff", "ge_code_pre_gfx.ff",
                    "ge_code_pre_gfx_mp.ff", "ge_common.ff", "ge_common_mp.ff",
                    "ge_common_zombie.ff", "ge_creek_1.ff", "ge_cuba.ff",
                    "ge_flashpoint.ff", "ge_frontend.ff", "ge_fullahead.ff",
                    "ge_hue_city.ff", "ge_int_escape.ff", "ge_khe_sanh.ff",
                    "ge_kowloon.ff", "ge_mp_area51.ff", "ge_mp_array.ff",
                    "ge_mp_berlinwall2.ff", "ge_mp_cairo.ff", "ge_mp_cosmodrome.ff",
                    "ge_mp_cracked.ff", "ge_mp_crisis.ff", "ge_mp_discovery.ff",
                    "ge_mp_drivein.ff", "ge_mp_duga.ff", "ge_mp_firingrange.ff",
                    "ge_mp_golfcourse.ff", "ge_mp_gridlock.ff", "ge_mp_hanoi.ff",
                    "ge_mp_havoc.ff", "ge_mp_hotel.ff", "ge_mp_kowloon.ff",
                    "ge_mp_mountain.ff", "ge_mp_nuked.ff", "ge_mp_outskirts.ff",
                    "ge_mp_radiation.ff", "ge_mp_russianbase.ff", "ge_mp_silo.ff",
                    "ge_mp_stadium.ff", "ge_mp_villa.ff", "ge_mp_zoo.ff",
                    "ge_outro.ff", "ge_patch.ff", "ge_patch_mp.ff",
                    "ge_pentagon.ff", "ge_pow.ff", "ge_rebirth.ff",
                    "ge_river.ff", "ge_so_narrative1_frontend.ff", "ge_so_narrative2_frontend.ff",
                    "ge_so_narrative3_frontend.ff", "ge_so_narrative4_frontend.ff", "ge_so_narrative5_frontend.ff",
                    "ge_so_narrative6_frontend.ff", "ge_terminal.ff", "ge_ui_mp.ff",
                    "ge_underwaterbase.ff", "ge_vorkuta.ff", "ge_wmd.ff",
                    "ge_wmd_sr71.ff", "ge_zombietron.ff", "ge_zombie_coast.ff",
                    "ge_zombie_cod5_asylum.ff", "ge_zombie_cod5_factory.ff", "ge_zombie_cod5_prototype.ff",
                    "ge_zombie_cod5_sumpf.ff", "ge_zombie_cosmodrome.ff", "ge_zombie_moon.ff",
                    "ge_zombie_pentagon.ff", "ge_zombie_temple.ff", "ge_zombie_theater.ff"
                };
            }

            static vector<string> ValidItalianFiles()
            {
                return {
                    "it_code_post_gfx.ff", "it_code_post_gfx_mp.ff", "it_code_pre_gfx.ff",
                    "it_code_pre_gfx_mp.ff", "it_common.ff", "it_common_mp.ff",
                    "it_common_zombie.ff", "it_creek_1.ff", "it_cuba.ff",
                    "it_flashpoint.ff", "it_frontend.ff", "it_fullahead.ff",
                    "it_hue_city.ff", "it_int_escape.ff", "it_khe_sanh.ff",
                    "it_kowloon.ff", "it_mp_area51.ff", "it_mp_array.ff",
                    "it_mp_berlinwall2.ff", "it_mp_cairo.ff", "it_mp_cosmodrome.ff",
                    "it_mp_cracked.ff", "it_mp_crisis.ff", "it_mp_discovery.ff",
                    "it_mp_drivein.ff", "it_mp_duga.ff", "it_mp_firingrange.ff",
                    "it_mp_golfcourse.ff", "it_mp_gridlock.ff", "it_mp_hanoi.ff",
                    "it_mp_havoc.ff", "it_mp_hotel.ff", "it_mp_kowloon.ff",
                    "it_mp_mountain.ff", "it_mp_nuked.ff", "it_mp_outskirts.ff",
                    "it_mp_radiation.ff", "it_mp_russianbase.ff", "it_mp_silo.ff",
                    "it_mp_stadium.ff", "it_mp_villa.ff", "it_mp_zoo.ff",
                    "it_outro.ff", "it_patch.ff", "it_patch_mp.ff",
                    "it_pentagon.ff", "it_pow.ff", "it_rebirth.ff",
                    "it_river.ff", "it_so_narrative1_frontend.ff", "it_so_narrative2_frontend.ff",
                    "it_so_narrative3_frontend.ff", "it_so_narrative4_frontend.ff", "it_so_narrative5_frontend.ff",
                    "it_so_narrative6_frontend.ff", "it_terminal.ff", "it_ui_mp.ff",
                    "it_underwaterbase.ff", "it_vorkuta.ff", "it_wmd.ff",
                    "it_wmd_sr71.ff", "it_zombietron.ff", "it_zombie_coast.ff",
                    "it_zombie_cod5_asylum.ff", "it_zombie_cod5_factory.ff", "it_zombie_cod5_prototype.ff",
                    "it_zombie_cod5_sumpf.ff", "it_zombie_cosmodrome.ff", "it_zombie_moon.ff",
                    "it_zombie_pentagon.ff", "it_zombie_temple.ff", "it_zombie_theater.ff"
                };
            }

            static vector<string> ValidSpanishFiles()
            {
                return {
                    "sp_code_post_gfx.ff", "sp_code_post_gfx_mp.ff", "sp_code_pre_gfx.ff",
                    "sp_code_pre_gfx_mp.ff", "sp_common.ff", "sp_common_mp.ff",
                    "sp_common_zombie.ff", "sp_creek_1.ff", "sp_cuba.ff",
                    "sp_flashpoint.ff", "sp_frontend.ff", "sp_fullahead.ff",
                    "sp_hue_city.ff", "sp_int_escape.ff", "sp_khe_sanh.ff",
                    "sp_kowloon.ff", "sp_mp_area51.ff", "sp_mp_array.ff",
                    "sp_mp_berlinwall2.ff", "sp_mp_cairo.ff", "sp_mp_cosmodrome.ff",
                    "sp_mp_cracked.ff", "sp_mp_crisis.ff", "sp_mp_discovery.ff",
                    "sp_mp_drivein.ff", "sp_mp_duga.ff", "sp_mp_firingrange.ff",
                    "sp_mp_golfcourse.ff", "sp_mp_gridlock.ff", "sp_mp_hanoi.ff",
                    "sp_mp_havoc.ff", "sp_mp_hotel.ff", "sp_mp_kowloon.ff",
                    "sp_mp_mountain.ff", "sp_mp_nuked.ff", "sp_mp_outskirts.ff",
                    "sp_mp_radiation.ff", "sp_mp_russianbase.ff", "sp_mp_silo.ff",
                    "sp_mp_stadium.ff", "sp_mp_villa.ff", "sp_mp_zoo.ff",
                    "sp_outro.ff", "sp_patch.ff", "sp_patch_mp.ff",
                    "sp_pentagon.ff", "sp_pow.ff", "sp_rebirth.ff",
                    "sp_river.ff", "sp_so_narrative1_frontend.ff", "sp_so_narrative2_frontend.ff",
                    "sp_so_narrative3_frontend.ff", "sp_so_narrative4_frontend.ff", "sp_so_narrative5_frontend.ff",
                    "sp_so_narrative6_frontend.ff", "sp_terminal.ff", "sp_ui_mp.ff",
                    "sp_underwaterbase.ff", "sp_vorkuta.ff", "sp_wmd.ff",
                    "sp_wmd_sr71.ff", "sp_zombietron.ff", "sp_zombie_coast.ff",
                    "sp_zombie_cod5_asylum.ff", "sp_zombie_cod5_factory.ff", "sp_zombie_cod5_prototype.ff",
                    "sp_zombie_cod5_sumpf.ff", "sp_zombie_cosmodrome.ff", "sp_zombie_moon.ff",
                    "sp_zombie_pentagon.ff", "sp_zombie_temple.ff", "sp_zombie_theater.ff"
                };
            }

            static vector<string> GetLanguageValidFiles()
            {
                string lang = game::GetGameLanguage();
                vector<string> lang_files;

                if (lang == "english")
                {
                    lang_files = ValidEnglishFiles();
                }
                else if (lang == "japanese")
                {
                    lang_files = ValidJapaneseFiles();
                }
                else if (lang == "french")
                {
                    lang_files = ValidFrenchFiles();
                }
                else if (lang == "austrian")
                {
                    lang_files = ValidGermanFiles();
                }
                else if (lang == "italian")
                {
                    lang_files = ValidItalianFiles();
                }
                else if (lang == "spanish")
                {
                    lang_files = ValidSpanishFiles();
                }

                return lang_files;
            }

            std::string CheckForExtraItemsInZone()
            {
                vector<string> valid_zones = { "Common", "English", "Japanese", "French", "German", "Italian", "Spanish" };
                string zone_folder = game::GetGamePath() + "\\zone\\";
                vector<string> extra_zones;

                for (const auto& entry : std::filesystem::directory_iterator(zone_folder)) {
                    std::string entry_name = entry.path().filename().string();

                    if (!game::process::IsGameOpen()) {
                        return "";
                    }

                    // convert to lowercase so we dont have issues with lowercase names
                    std::string lower_entry_name = utils::strings::ToLower(entry_name);
                    bool is_valid = false;

                    for (const auto& zone : valid_zones) {
                        if (lower_entry_name == utils::strings::ToLower(zone)) {
                            is_valid = true;
                            break;
                        }
                    }

                    if (!is_valid) {
                        extra_zones.push_back(entry_name);
                    }
                }

                return utils::strings::FormatVector(extra_zones);
            }

            std::string GetExtraFilesInZone(std::string zone_name)
            {
                string zone_lower = utils::strings::ToLower(zone_name);
                vector<string> valid_files = (zone_lower == "common" ? ValidCommonFiles() : GetLanguageValidFiles());
                string zone_dir = game::GetGamePath() + "\\zone\\" + zone_name;

                // check if the folder exists
                if (!fs::exists(zone_dir))
                {
                    return "";
                }

                if (!game::process::IsGameOpen())
                {
                    return "";
                }

                vector<string> extra_files;
                for (const auto& entry : filesystem::directory_iterator(zone_dir))
                {
                    string file_name = entry.path().filename().string();

                    // we don't want custom_fx to be there if the tool isnt being used
                    if (zone_lower == "common" && file_name == "custom_fx.ff" && !game::IsCustomFxToolLoaded())
                    {
                        extra_files.push_back("zone\\Common\\custom_fx.ff");
                        continue;
                    }

                    // could cause a problem mid check if the game is closed suddenly
                    if (!game::process::IsGameOpen())
                    {
                        return "";
                    }

                    if (find(valid_files.begin(), valid_files.end(), file_name) == valid_files.end())
                    {
                        extra_files.push_back(file_name);
                    }
                }

                return utils::strings::FormatVector(extra_files);
            }

            std::string GetModifiedFastFiles(std::string map_name)
            {
                vector<string> modified_fastfiles;

                if (!game::process::IsGameOpen())
                {
                    return "";
                }

                // scan the global ff files
                for (ZoneEntry zone_entry : global_zone_queue)
                {
                    std::string ff_name = zone_entry.ff_name;
                    std::string ff_hash = zone_entry.checksum;
                    string full_ff_path = game::GetGamePath() + "\\" + ff_name;

                    if (!IsFFValid(ff_name))
                    {
                        continue;
                    }

                    std::cout << "Scanning FF: " << ff_name << "\n";

                    string file_hash = utils::files::GetMD5(full_ff_path);
                    if (file_hash != ff_hash)
                    {
                        modified_fastfiles.push_back(ff_name);
                    }
                }

                // get the map ff files list
                std::vector<ZoneEntry> map_entries = {};
                for (MapZoneEntry map_zone : map_zone_queue)
                {
                    std::string ff_name = map_zone.map_name;
                    if (map_zone.map_name == map_name)
                    {
                        map_entries = map_zone.zone_entries;
                        break;
                    }
                }

                if (map_entries.size() == 0)
                {
                    // somehow we didnt find the map files?
                    return utils::strings::FormatVector(modified_fastfiles);
                }

                // scan the map ff files
                for (ZoneEntry zone_entry : map_entries)
                {
                    std::string ff_name = zone_entry.ff_name;
                    std::string ff_hash = zone_entry.checksum;
                    string full_ff_path = game::GetGamePath() + "\\" + ff_name;

                    if (!IsFFValid(ff_name))
                    {
                        continue;
                    }

                    std::cout << "\nScanning FF: " << ff_name << "\n";

                    string file_hash = utils::files::GetMD5(full_ff_path);
                    if (file_hash != ff_hash)
                    {
                        modified_fastfiles.push_back(ff_name);
                    }
                }

                return utils::strings::FormatVector(modified_fastfiles);
            }

            bool IsLangUsingFF(std::string ff_path)
            {
                if (utils::strings::ContainsIgnoreCase(ff_path, "zone\\Common"))
                {
                    return true;
                }
                else {
                    string expected_folder = "zone\\" + game::GetLanguageZoneName();

                    // check if it has the zone folder
                    if (utils::strings::ContainsIgnoreCase(ff_path, expected_folder))
                    {
                        return true;
                    }
                }

                return false;
            }

            bool IsFFValid(std::string ff_name)
            {
                string fastfile = game::GetGamePath() + "\\" + ff_name;

                // we can't scan an ff that doesn't exist
                if (!fs::exists(fastfile))
                {
                    return false;
                }

                // make sure we're only scanning files that are in use by the language
                if (!IsLangUsingFF(ff_name))
                {
                    return false;
                }

                // could cause a problem mid check if the game is closed suddenly
                if (!game::process::IsGameOpen())
                {
                    return false;
                }

                return true;
            }
        }
    }
}