/*

Auto Feeder Controller for M5StickC

Copyright 2020 Makoto Nishikawa

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/
#include <M5StickC.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#define TEXTBUFFER 4096

int run_auto_feeder(void);

const char* wifi_ssid     = "yourssid";
const char* wifi_password = "yourpassword"; 
const char* stop_command_cgi = "http://yoursite/cgi-bin/afc4m5stickc_stop.cgi";
const char* afc_command_url = "http://yoursite/afc_command";


WiFiMulti wifiMulti;
HTTPClient http;
char strbuffer[TEXTBUFFER];

void setup() {
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.Axp.ScreenBreath(15);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.fillScreen(BLACK);

  pinMode(26, OUTPUT);
  digitalWrite(26, LOW);
  
  Serial.begin(115200);
  while (!Serial);
  delay(100);

  // WifiSetup
  wifiMulti.addAP(wifi_ssid, wifi_password);
 
}


void loop() {
  // wait for WiFi connection
  if((wifiMulti.run() == WL_CONNECTED)) {

      HTTPClient http;
      //Get auto feeder command information 
      http.begin(afc_command_url);
      Serial.print("Connecting...\n");

      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if(httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          //M5.Lcd.printf("[HTTP] GET... code: %d\n", httpCode);
          Serial.print("HTTP code: ");
          Serial.print(httpCode, DEC);
          Serial.print("\n");

          // file found at server
          if(httpCode == HTTP_CODE_OK) {
              String payload = http.getString();
              payload.toCharArray(strbuffer, TEXTBUFFER);
              Serial.print(strbuffer);
              Serial.print("\n");
              // Check command
              if(payload.indexOf("run") == 0){
                M5.Lcd.printf("Receive RUN command.\n");
                // Run auto feeder
                run_auto_feeder();
              }
          }
      } else {
          M5.Lcd.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          Serial.print("HTTP error\n");
      }

      http.end();
  }

  // Wait
  delay(10000);
  
}


int run_auto_feeder()
{
   M5.Lcd.printf("Run auto feeder....\n");
    // PUSH auto feeder button
    digitalWrite(26, HIGH);

   HTTPClient http_stop;

   //Get auto feeder command information 
   http_stop.begin(stop_command_cgi);
   Serial.print("STOP auto feeder\n");

   // start connection and send HTTP header
   int httpCode_stop = http_stop.GET();

      // httpCode will be negative on error
      if(httpCode_stop > 0) {
          Serial.print("HTTP stop code success\n");
      } else {
          Serial.print("HTTP stop code error\n");
      }
      http_stop.end();

    // Wait
    delay(10000);
    // RELEASE auto feeder button
    digitalWrite(26, LOW);
    
    M5.Lcd.fillScreen(BLACK);

}
