idf_component_register(SRCS "flash.c"
                  INCLUDE_DIRS "."
                  EMBED_FILES "default_blacklist.txt"
                  PRIV_REQUIRES nvs_flash spiffs logging url error)