#ifndef MM_WIFI_H
#define MM_WIFI_H


#include "esp_err.h"
#define WIFI_PHX_SSID"SETUP-E0C4_2.4Ghz"
#define WIFI_PHX_PASSWORD "career7455cheer"
#define WIFI_HOTSPOT_SSID "johnm_iphone"
#define WIFI_HOTSPOT_PASSWORD "Redsox12"
#define WIFI_SAN_SSID "Tilins_nightclub"
#define WIFI_SAN_PASSWORD "engineers123"

#define WIFI_SSID WIFI_SAN_SSID
#define WIFI_PASSWORD WIFI_SAN_PASSWORD

esp_err_t mm_wifi_init(void);

#endif // MM_WIFI_H