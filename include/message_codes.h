#ifndef MQTT_CODES_H
#define MQTT_CODES_H

    /* Simple decoder for the Wi-Fi driver status codes*/
    static const char* WIFI_StatusDecoder[8] = {
        "WL_IDLE_STATUS",
        "WL_NO_SSID_AVAIL",
        "WL_SCAN_COMPLETED",
        "WL_CONNECTED",
        "WL_CONNECT_FAILED",
        "WL_CONNECTION_LOST",
        "WL_DISCONNECTED",
        "WL_NO_SHIELD "
    };

#endif