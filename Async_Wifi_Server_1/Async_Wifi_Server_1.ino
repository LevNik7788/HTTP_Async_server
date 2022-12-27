#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"

IPAddress apIP(192, 168, 4, 1); //адрес сервера

AsyncWebServer server(80);

const char* ssid = "_";                 //"MGTS_GPON_A5AD"
const char* password =  "E457L56A";
String _ssidAP = "ESP_WiFi";   // SSID AP точки доступа
String _passwordAP = ""; // пароль точки доступа

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  byte tries = 11;
  WiFi.begin(ssid, password);

  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    // Если не удалось подключиться запускаем в режиме AP
    Serial.println("");
    Serial.println("точка доступа ESP_WiFi");
    StartAPMode();
  }
  else {
    // Иначе удалось подключиться отправляем сообщение
    // о подключении и выводим адрес IP
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hello, world");
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    int paramsNr = request->params();
    Serial.print ("Количество параметров =  ");
    Serial.println(paramsNr);
    String paramName = request -> getParam("name") -> name();
    Serial.print ("Имя параметра =  ");
    Serial.println(paramName);
    String paramVal = request -> getParam("name") -> value();
    Serial.print ("Значение параметра =  ");
    Serial.println(paramVal);

    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, GET: " + message + " " + paramVal);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE, true)) {
      message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();
}

void loop() {
}

bool StartAPMode()
{ // Отключаем WIFI
  WiFi.disconnect();
  // Меняем режим на режим точки доступа
  WiFi.mode(WIFI_AP);
  // Задаем настройки сети
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  // Включаем WIFI в режиме точки доступа с именем и паролем
  // хронящихся в переменных _ssidAP _passwordAP
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  return true;
}
