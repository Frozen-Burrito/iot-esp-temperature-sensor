idf_component_register(SRCS "http_client.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_http_client
                                esp-tls
                                esp_common
                                esp_event
                                esp_netif)