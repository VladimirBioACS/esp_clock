#include "wifi_manager.h"
 
FileSystem fileSystem_wifi;


void WifiManager::wifi_data_to_hex(byte arr[], unsigned int len, char buff[]){
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (arr[i] >> 4) & 0x0F;
    byte nib2 = (arr[i] >> 0) & 0x0F;
    buff[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buff[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buff[len*2] = '\0';
}


bool WifiManager::wifi_connect()
{
  StaticJsonDocument<80U> wifi_creds = fileSystem_wifi.get_config("/wifi_creds.json");

  const char* ssid =  wifi_creds["ssid"];     
  const char* password = wifi_creds["password"]; 

  int counter = 0;
 
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(ssid, password);

    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);

    /*============================================================================
    Wifi Status code decoder. 
    !!!WILL NOT WORK WITH DEFAULT Wifi LIB!!!
    error codes in default lib include 255 for shield identification
    ============================================================================*/

    Serial.print("Status: ");
    Serial.print(status);
    Serial.print(" : ");
    Serial.println(WIFI_StatusDecoder[status]);

    ++counter; 

    delay(3000);

    if(WL_MAX_ATTEMPT_CONNECTION == counter)
    {
        break;
    }
  }

  if(status == WL_CONNECTED)
  {
    return true;
  }

  else
  {
    return false;
  }
}


void WifiManager::printCurrentNet()
{

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  wifi_data_to_hex(bssid, 6, bssid_str);

  Serial.print("BSSID: ");
  Serial.println(bssid_str);

  rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);
}


void WifiManager::printWifiData()
{

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  WiFi.macAddress(mac);
  wifi_data_to_hex(mac, 6, mac_str);

  Serial.print("MAC address: ");
  Serial.println(mac_str);
}


void WifiManager::scan_networks()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int network_num = WiFi.scanNetworks();

  if(network_num == 0)
  {
    Serial.println("No networks found");
  }

  if(network_num > 0)
  {
    Serial.print("Networks found: ");
    Serial.println(network_num);

    for (size_t i = 0; i < network_num; i++)
    {
      Serial.print(WiFi.SSID(i));
      Serial.print(": ");
      Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      Serial.print(": RSSI : ");
      Serial.println(WiFi.RSSI(i));
      delay(10);
    }
    
  }
  delay(1000);
}


void WifiManager::manager()
{
  AsyncWebServer wifi_server(WIFI_MANAGER_PORT);
  WiFi.softAP(access_point_ssid, access_point_password);

  Serial.println("local wifi manager started");

  Serial.print("SoftAP name: ");
  Serial.println(access_point_ssid);

  IPAddress ip = WiFi.softAPIP();

  Serial.print("Use local ip to connect to Wi-Fi manager: ");
  Serial.println(ip);

  wifi_server.on("/wifi_save_creds", HTTP_POST, [](AsyncWebServerRequest *request){
    DynamicJsonDocument p_wifi(255);
    char wifi_param_buff[255];

    int param_parsed = request -> params();
    Serial.println(param_parsed);

    for(size_t i = 0; i < param_parsed; i++)
    {
        AsyncWebParameter* parameter = request->getParam(i);
        p_wifi[parameter -> name()] = parameter -> value();
    }
    serializeJson(p_wifi, wifi_param_buff);

    bool save_flag = fileSystem_wifi.write_file("/wifi_creds.json", wifi_param_buff);
    if(!save_flag){
        request -> send(SPIFFS, "/static/wifi/wifi_fail.html");
    }
    request -> send(SPIFFS, "/static/wifi/wifi_success.html");

  });

  wifi_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/static/wifi/wifi.html");
  });

  while(1)
  {
    wifi_server.begin();
  }
}