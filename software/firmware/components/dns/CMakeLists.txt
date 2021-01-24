idf_component_register(SRCS "dns.c" "captive_dns.c"
                  INCLUDE_DIRS "."
                  PRIV_REQUIRES storage datetime gpio url)