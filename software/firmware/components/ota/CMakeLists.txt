idf_component_register(SRCS "ota.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES gpio nvs_flash app_update esp_http_client esp_https_ota
                    EMBED_TXTFILES server_certs/ca_cert.pem)