/*======================================================================================================
  _____ _            _    _____                      _       
 / ____| |          | |  |  __ \                    | |      
| |    | | ___   ___| | _| |__) |___ _ __ ___   ___ | |_ ___ 
| |    | |/ _ \ / __| |/ /  _  // _ \ '_ ` _ \ / _ \| __/ _ \
| |____| | (_) | (__|   <| | \ \  __/ | | | | | (_) | ||  __/
 \_____|_|\___/ \___|_|\_\_|  \_\___|_| |_| |_|\___/ \__\___|
                                                             
 @author     Volodymyr Noha (SignalIO)
 @license    BSD (see LICENSE.txt)  

 @description:
 Simple Wi-Fi remote for the desktop clock, based on ESP32 MCU. This firmware configure all basic 
 interfaces, which is needed for device functioning. The access to the device can be obtained with 
 the Wi-Fi interface and control the GPIO ports, dedicated to the specific clock button, via embedded
 WEB-server (or via API HTTP endpoint with some 3d-party web frontend). The firmware also supports
 simple Wi-Fi and OTA managers for network settings configuration and remote firmware updates.

 @section  HISTORY
 
 v1.0.1  - First version  
======================================================================================================*/


/*======================================================================================================
---------------------------------------------Includes Section------------------------------------------
======================================================================================================*/
#include <Arduino.h>
#include <esp_task_wdt.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>

#include "wifi_manager.h"
#include "web_interface_manager.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


/*======================================================================================================
---------------------------------------------Constants Section-----------------------------------------
======================================================================================================*/
#define GPIO_UP       uint32_t(18)
#define GPIO_DWN      uint32_t(19)
#define GPIO_ALM      uint32_t(21)
#define GPIO_SET      uint32_t(22)

#define GPIO_BITMASK  ( \
                       (1ULL << GPIO_UP) | (1ULL << GPIO_DWN) | \
                       (1ULL << GPIO_ALM) | (1ULL << GPIO_SET)  \
                      )

#define CPU_FREQ      uint32_t(80) // MHz


/*======================================================================================================
---------------------------------------------Variables Section-----------------------------------------
======================================================================================================*/
uint32_t cpu_freq;
const char* host = "remote";


/*======================================================================================================
---------------------------------------------Classes Section-------------------------------------------
======================================================================================================*/
WifiManager wifiManager;


/*======================================================================================================
---------------------------------------Function Prototypes Section-------------------------------------
======================================================================================================*/

/* Restart MCU after 1 second */
void mcu_reset(void);


/*======================================================================================================
------------------------------------Functions Implementation Section------------------------------------
======================================================================================================*/

void mcu_reset(void)
{
  delay(5000);
  ESP.restart();
}

/* Main function */
void setup() 
{
  cpu_freq = 0;

  Serial.begin(115200);

  Serial.println("Clock remote started");
  Serial.print("Firmware version: ");
  Serial.println(FIRMWARE_VERSION);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  /* 
  CLK setup:
      240, 160, 80    <<< For all XTAL types
      40, 20, 10      <<< For 40MHz XTAL
      26, 13          <<< For 26MHz XTAL
      24, 12          <<< For 24MHz XTAL
  */
  setCpuFrequencyMhz(CPU_FREQ);

  gpio_config_t io_config;

  io_config.pin_bit_mask = GPIO_BITMASK;
  io_config.mode = GPIO_MODE_OUTPUT_OD; // Output open drain
  io_config.pull_up_en = GPIO_PULLUP_ENABLE;
  io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_config.intr_type = GPIO_INTR_DISABLE;

  gpio_config(&io_config);

  /* Clear GPIO`s */
  GPIO.out_w1ts = (1ULL << GPIO_UP);
  GPIO.out_w1ts = (1ULL << GPIO_DWN);
  GPIO.out_w1ts = (1ULL << GPIO_ALM);
  GPIO.out_w1ts = (1ULL << GPIO_SET);

  if(!SPIFFS.begin())
  {
    Serial.println("SPIFFS Init error");
    mcu_reset();
  }

  else
  {
    Serial.println("SPIFFS inited");
  }


  bool wifi_state = wifiManager.wifi_connect();

  if(wifi_state)
  {
     Serial.println("Wi-Fi connected");

     wifiManager.printCurrentNet();
     wifiManager.printWifiData();

     if (!MDNS.begin("remote"))
     {
      Serial.println("Failed to start mDNS");
      mcu_reset();
     }

     else
     {
        Serial.println("mDNS responder started");
        Serial.println("Config server started\nmDNS address: http://remote.local/");
        Serial.println("Setup Done");

        interface_menu();
     }
  }

  else
  {
      Serial.println("Wi-Fi not connected");
      Serial.println("Start scanning for networks");

      wifiManager.scan_networks();
      wifiManager.manager();
  }
}

/* infinite loop task */
void loop() 
{
  // empty  
}