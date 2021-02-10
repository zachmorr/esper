idf_component_register(SRCS "storage.c" "logging.c"
                  INCLUDE_DIRS "."
                  PRIV_REQUIRES dns url nvs_flash spiffs esp_http_server)