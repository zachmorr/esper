idf_component_register(SRCS "url.c"
                    INCLUDE_DIRS "."
                    EMBED_FILES "default_blacklist.txt"
                    PRIV_REQUIRES spiffs)