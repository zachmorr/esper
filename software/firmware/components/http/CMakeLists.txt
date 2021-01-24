idf_component_register(SRCS "configuration.c" "application.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES netif dns storage datetime url spiffs esp_http_server ota
                    EMBED_FILES "src/configuration/selectwifi.html"
                                "src/configuration/connected.html"
                                "src/configuration/finished.html"
                                "src/application/homepage.html"
                                "src/application/blacklist.html"
                                "src/application/settings.html")