#include "web_interface_manager.h"
#include <AsyncElegantOTA.h>

const char* auth_creds_path = "/auth_creds.json";
const char* wifi_creds_path = "/wifi_creds.json";

const char* auth_login;
const char* auth_password;

const char* PARAM_INPUT_ONE = "pin";

AsyncWebServer server(80);
FileSystem webFS;


void reboot(AsyncWebServerRequest *request)
{
    if(!request->authenticate(auth_login, auth_password))
    {
        return request->requestAuthentication();
    }
    ESP.restart();
}


void button_press_emulate(int pin)
{
  GPIO.out_w1tc = (1ULL << (uint32_t)pin);
  delay(DELAY_MS);
  GPIO.out_w1ts = (1ULL << (uint32_t)pin);
  delay(DELAY_MS);
}


bool creds_validator(StaticJsonDocument<255> auth_cred)
{

    if(auth_cred["old_username"] == auth_login && auth_cred["old_password"] == auth_password)
    {
        return true;
    }

    return false;
}


void not_found(AsyncWebServerRequest *request) 
{
    request->send(SPIFFS, "/static/interface/404.html");
}


void change_wifi_network(AsyncWebServerRequest *request)
{
    if(!request->authenticate(auth_login, auth_password))
    {
        return request->requestAuthentication();
    }

    DynamicJsonDocument wifi_net_p(255);
    char wifi_net_param_buff[255];

    int wifi_net_param_parsed = request -> params();

    for (size_t i = 0; i < wifi_net_param_parsed; i++)
    {
        AsyncWebParameter* wifi_net_parameter = request -> getParam(i);
        wifi_net_p[wifi_net_parameter -> name()] = wifi_net_parameter -> value();
    }

    serializeJson(wifi_net_p, wifi_net_param_buff);
    bool wifi_net_update_flag = webFS.write_file(wifi_creds_path, wifi_net_param_buff);

    if(!wifi_net_update_flag)
    {
        request->send(SPIFFS, "/static/interface/fail.html"); 
    }

    request->send(SPIFFS, "/static/interface/success.html");
}


void update_auth_creds(AsyncWebServerRequest *request)
{
        if(!request->authenticate(auth_login, auth_password))
        {
            return request->requestAuthentication();
        }

        DynamicJsonDocument auth_p(255);
        char auth_param_buff[255];

        int auth_param_parsed = request -> params();

        for (size_t i = 0; i < auth_param_parsed; i++)
        {
            AsyncWebParameter* parameter = request->getParam(i);
            auth_p[parameter -> name()] = parameter -> value();
        }

        serializeJson(auth_p, auth_param_buff);
        bool creds_validation_flag = creds_validator(auth_p);

        if(creds_validation_flag)
        {
            bool update_creds_flag = webFS.write_file(auth_creds_path, auth_param_buff);

            if(!update_creds_flag)
            {
                request->send(SPIFFS, "/static/interface/fail.html"); 
            }

        }

        request->send(SPIFFS, "/static/interface/success.html");
}


String processor(const String& var)
{
  if(var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";

    buttons += "<p><button class=\"btn btn-light\" type=\"button\" onclick=\"togglePin(this)\" id=\"18\"> UP BUTTON </button></p>";
    buttons += "<p><button class=\"btn btn-light\" type=\"button\" onclick=\"togglePin(this)\" id=\"19\"> DOWN BUTTON </button></p>";
    buttons += "<p><button class=\"btn btn-light\" type=\"button\" onclick=\"togglePin(this)\" id=\"21\"> ALM BUTTON </button></p>";
    buttons += "<p><button class=\"btn btn-light\" type=\"button\" onclick=\"togglePin(this)\" id=\"22\"> SET BUTTON </button></p>";

    return buttons;
  }

  return String();
}


String info_processor(const String& var)
{
  if(var == "INFO")
  {
    String infos = " ";
    
    infos += "<p>\"Firmware Version: \"" + String(FIRMWARE_VERSION) + " V</p>";
    infos += "<p>\"MCU model: \"" + String(ESP.getChipModel()) + "</p>";
    infos += "<p>\"Free space for firmware:  \"" + String(ESP.getFreeSketchSpace()) + " bytes</p>";
    infos += "<p>\"Heap size:  \"" + String(ESP.getHeapSize()) + " bytes</p>";
    infos += "<p>\"Free heap:   \"" + String(ESP.getFreeHeap()) + " bytes</p>";
    infos += "<p>\"SDK version:    \"" + String(ESP.getSdkVersion()) + " V</p>";

    return infos;
  }

  return String();
}


void interface_menu()
{
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    StaticJsonDocument<255> confs = webFS.get_config(auth_creds_path);

    auth_login = confs["username"];
    auth_password = confs["password"];
    
    server.on("/change-auth-creds", HTTP_POST, update_auth_creds);


    server.on("/change-wifi-network", HTTP_POST, change_wifi_network);


    server.on("/device-reboot", HTTP_POST, reboot);


    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        if(!request->authenticate(auth_login, auth_password)){
            return request->requestAuthentication();
        }
        request->send(SPIFFS, "/static/interface/index.html");
    });


    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        if(!request->authenticate(auth_login, auth_password)){
            return request->requestAuthentication();
        }
        request->send(SPIFFS, "/css/style.css", "text/css");
    });


    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        if(!request->authenticate(auth_login, auth_password)){
            return request->requestAuthentication();
        }
        request->send(SPIFFS, "/js/script.js", "text/javascript");
    });


    server.on("/control_panel", HTTP_GET, [](AsyncWebServerRequest *request){
        if(!request->authenticate(auth_login, auth_password)){
            return request->requestAuthentication();
        }
        request->send_P(200, "text/html", control_panel, processor);
    });

    server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request){
        if(!request->authenticate(auth_login, auth_password)){
            return request->requestAuthentication();
        }
        request->send_P(200, "text/html", info, info_processor);
    });


    server.on("/trig_pin", HTTP_GET, [] (AsyncWebServerRequest *request){
        String inputMessage1;
        // GET input1 value
        if(!request->authenticate(auth_login, auth_password)){
            return request->requestAuthentication();
        }
        if (request->hasParam(PARAM_INPUT_ONE)){
            inputMessage1 = request->getParam(PARAM_INPUT_ONE)->value();
            button_press_emulate(inputMessage1.toInt());
        }
        request->send(200, "text/plain", "{\"status\":\"OK\"}");
    });


    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
        if(!request->authenticate(auth_login, auth_password)){
            return request->requestAuthentication();
        }
        request->send(SPIFFS, "/static/interface/settings.html");
    });


    AsyncElegantOTA.begin(&server, auth_login, auth_password);
    
    server.onNotFound(not_found);

    while(1)
    {
        server.begin();
    }
}