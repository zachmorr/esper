idf_component_register(SRCS "station.c" "accesspoint.c" "wifi.c"
                  INCLUDE_DIRS "."
                  PRIV_REQUIRES flash gpio error)