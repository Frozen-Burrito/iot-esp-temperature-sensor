set(SOURCES "dht22.c")

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "."
                       REQUIRES freertos
                                driver
                                esp_common)