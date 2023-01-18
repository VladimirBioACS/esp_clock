#ifndef FS_MANAGER_H
#define FS_MANAGER_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "json_lib.h"

class FileSystem{
    private:
        const int buff_size = 1024;

    public:
        bool read_file(const char* path);
        bool write_file(const char* path, char buff[]);
        bool config_reset();
        
        StaticJsonDocument<1024> get_config(const char* path);
};

#endif