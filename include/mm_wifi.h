#ifndef MM_WIFI_H
#define MM_WIFI_H


#include "esp_err.h"

#define WIFI_SSID "Tilins_nightclub"
#define WIFI_PASSWORD "engineers123"


esp_err_t mm_wifi_init(void);

#endif // MM_WIFI_H