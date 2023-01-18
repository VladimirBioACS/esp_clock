#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "json_lib.h"
#include "fs_manager.h"
#include "message_codes.h"

class WifiManager
{
private:
    int status = WL_IDLE_STATUS;
    int WIFI_MANAGER_PORT = 80;
    int WL_MAX_ATTEMPT_CONNECTION = 5;

    long rssi;

    byte bssid[6];
    byte mac[6];

    char mac_str[32] = "";
    char bssid_str[32] = "";

    const char* access_point_ssid = "esp32-AP";
    const char* access_point_password = "root1234";


    void wifi_data_to_hex(byte arr[], unsigned int len, char buff[]);

public:
    /* Establishes a connection to a Wi-Fi network, which is specified in the configuration file 'wifi_creds.json' in SPIFFS. Return type: bool */
    bool wifi_connect(void);  

    /* Prints current network info: SSID; BSSID; RSSI. Return type: void*/           
    void printCurrentNet(void);

     /* Prints current network data: IP Address; MAC Address. Return type: void*/          
    void printWifiData(void); 

    /* Starts the Wi-Fi manager for network configuration. The access to the Wi-Fi manager can be obtained via device IP-address. The device IP-address can be found in debug logs in Serial monitor. Return type: void*/          
    void manager(void);

    /* Performs basic network scanning and prints the list of all available networks into Serial monitor. Return type: void*/                
    void scan_networks(void);            
};


#endif