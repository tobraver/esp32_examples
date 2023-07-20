#include "prefs.h"

void examples(void)
{
    const prefs_t hprefs = {
        .part_name = "nvs",
        .namespace = "config",
    };

    prefs_init(hprefs);
    
    char* key = "conf_code";

    prefs_write_block(g_config_prefs, key, &config, sizeof(conf_out_t));

    prefs_read_block(g_config_prefs, key, config, sizeof(conf_code_t));
}

