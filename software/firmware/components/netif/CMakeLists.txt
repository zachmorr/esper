idf_component_register(SRCS "station.c" "accesspoint.c"
                  INCLUDE_DIRS "."
                  PRIV_REQUIRES storage gpio)