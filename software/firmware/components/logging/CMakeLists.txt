idf_component_register(SRCS "logging.c"
                  INCLUDE_DIRS "."
                  PRIV_REQUIRES dns url flash datetime spiffs esp_http_server error json)