#ifndef WEB_INTERFACE_MANAGER_H
#define WEB_INTERFACE_MANAGER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "fs_manager.h"
#include "WiFi.h"
#include "json_lib.h"
#include "firmware_version.h"

#define DELAY_MS      uint32_t(150)

const char control_panel[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML><html>

<head>
  <title>ESP Clock Remote</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" integrity="sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z" crossorigin="anonymous">
  <script src="https://code.jquery.com/jquery-3.5.1.min.js" integrity="sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=" crossorigin="anonymous"></script>
  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js" integrity="sha384-B4gt1jrGC7Jh4AgTPSdUtOBvfO8shuf57BaghqFfPlYxofvL8/KUEfYiJOMMV+rV" crossorigin="anonymous"></script>
  <link href="https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css" rel="stylesheet">
  <script src="https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js"></script>
  <script src="https://kit.fontawesome.com/1fa6a854ed.js" crossorigin="anonymous"></script>
  <link rel="stylesheet" href="/style.css">
  <script src="/script.js" type="text/javascript"></script>
</head>

<body>
  <div class="msg-container">
	<h1> <span><i class="far fa-edit"></i></span> ESP Clock Remote <b class="msg-highlight" style="color:#4781f6">Control Panale</b></h1>
  </div>
    <br>

  %BUTTONPLACEHOLDER%

<script>
function togglePin(element)
{
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/trig_pin?pin=" + element.id, true); 
  xhr.send();
}
</script>

</body>
</html>

)rawliteral";


const char info[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML><html>

<head>
  <title>ESP Clock Remote Info</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" integrity="sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z" crossorigin="anonymous">
  <script src="https://code.jquery.com/jquery-3.5.1.min.js" integrity="sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=" crossorigin="anonymous"></script>
  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js" integrity="sha384-B4gt1jrGC7Jh4AgTPSdUtOBvfO8shuf57BaghqFfPlYxofvL8/KUEfYiJOMMV+rV" crossorigin="anonymous"></script>
  <link href="https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css" rel="stylesheet">
  <script src="https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js"></script>
  <script src="https://kit.fontawesome.com/1fa6a854ed.js" crossorigin="anonymous"></script>
  <link rel="stylesheet" href="/style.css">
  <script src="/script.js" type="text/javascript"></script>
</head>

<body>
  <div class="msg-container">
	<h1> <span><i class="far fa-edit"></i></span> ESP Clock Remote <b class="msg-highlight" style="color:#4781f6">Device Info</b></h1>
  </div>
    <br>
  %INFO%
</body>
</html>

)rawliteral";

void interface_menu();

#endif