#include "fs_manager.h"


StaticJsonDocument<1024> FileSystem::get_config(const char* path)
{

  StaticJsonDocument<1024> json_config;
  File configFile = SPIFFS.open(path, "r");

  if(!configFile)
  {
    log_e("failed to load JSON config\n"); // debug message
    configFile.close();
    return json_config;
  }

  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  deserializeJson(json_config, buf.get());

  return json_config;
}


bool FileSystem::read_file(const char* path)
{
    File file = SPIFFS.open(path, "r");
    size_t size = file.size();
    
    if(!file){
        log_e("Error opening file: %s\n", path);
        return false;
    }

    log_i("Config file exist: %s\n", path);
    log_i("File size: %i (bytes)\n", size);

    file.close();
    return true;
}


bool FileSystem::write_file(const char* path, char buff[])
{
    File f = SPIFFS.open(path, "w");
    size_t size = f.size();

    if(!f)
    {
        f.close();
        log_e("File not found\n"); // debug msg
        return false;
    }

    if(strlen(buff) == 0)
    { // needs debug
        log_e("Config is empty\n"); // debug msg
        return false;
    }

    if(strlen(buff) > buff_size)
    { // needs debug
        log_e("Config size is too big\n"); // debug msg
        return false;
    }

    if (size > buff_size)
    {
        f.close();
        log_e("File size is too big\n"); // debug msg
        return false;
    }

    if(f.print(buff)){
        log_i("Config written succsessfully\n"); // debug msg
        return true;
    }

    log_e("Write failed\n"); // debug msg
    return false;   
}


bool FileSystem::config_reset()
{
    StaticJsonDocument<32> config_rst;
    StaticJsonDocument<64> auth_creds_rst;

    File config = SPIFFS.open("/config.json", "w");

    if(!config)
    {
      log_e("File does not exist\n"); // debug msg
      config.close();
      return false;
    }

    config_rst["config_flag"] = 1;
    config_rst["wifi_manager_state"] = 1;

    serializeJson(config_rst, config);
    config.close();

    File auth_creds = SPIFFS.open("/auth_creds.json", "w");

    if(!auth_creds)
    {
      log_e("File does not exist!\n");
      config.close();
      return false;
    }

    auth_creds_rst["username"] = (String)"Signalio";
    auth_creds_rst["password"] = (String)"root1234";

    serializeJson(auth_creds_rst, auth_creds);
    auth_creds.close();
    
    return true;
}