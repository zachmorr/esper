idf_component_register(SRCS "configuration.c" "application.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES netif dns storage datetime url spiffs esp_http_server ota
                    EMBED_FILES "src/configuration/provwifi.html"
                                "src/configuration/provsettings.html"
                                "src/configuration/provfinished.html"
                                "src/application/home.html"
                                "src/application/blacklist.html"
                                "src/application/settings.html")