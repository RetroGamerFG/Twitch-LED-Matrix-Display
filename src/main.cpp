//Code created by: Bailey "itsRetro" Manczko
//This program is intended for hobbyist, and is licensed under Apache-2.0.

//Twitch LED Matrix Display

//While the header and .cpp files can be used in other projects, this is the main.cpp designed while creating the project.
//It demonstrates how the file(s) can be used to display data to the LED. You can use this main file to get this project
//up and running just like in the README.md.

#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

#include <filesystem.h>
#include <twitch.h>
#include <shapes.h>

//::REMINDER::
//Be sure to set your Twitch API variables within the "twitch.h" defines

//WiFi credentials
#define SSID "WIFI_SSID_HERE"
#define password "WIFI_PASSWORD_HERE"

//Hardware definitions (only change if you know what you're doing!)
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 14   //D5
#define CS_PIN 13    //D7
#define DATA_PIN 15  //D8

//Object initializers
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola p = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
Twitch twitchInstance = Twitch(TWITCH_APP_TOKEN, TWITCH_CLIENT_ID, TWITCH_CLIENT_SECRET, TWITCH_USER_ID);

//other global variables
const unsigned long fetchInterval = 300000; //300,000 for 5 mins
unsigned long previousMillis = 0;

String lastOut; //string buffer
int page = 0; //page index

void setup()
{
  Serial.begin(115200);
  delay(1000);

  startLittleFS(); //starts file system

  //call begin() on display objects
  mx.begin();
  p.begin();

  p.setIntensity(1); //default intensity as 1 (personal preference)
  p.print("WiFi...");

  //initialize the WiFi
  WiFi.begin(SSID, password);
 
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  p.displayClear();
  p.print("Data...");

  //call begin() to start Twitch API calls, get initial data on success
  twitchInstance.begin();
  delay(2000);

  twitchInstance.updateData();
  delay(2000);

  lastOut = twitchInstance.printOutput(); //store data into global string

  p.displayClear();
  twitchInstance.printDebug();
}

void loop()
{
  unsigned long currentMillis = millis(); //get the current operating time using millis()

  //if the current time is past the fetch interval set above, initiate a call to update values
  if(currentMillis - previousMillis >= fetchInterval)
  {
    previousMillis = currentMillis; //assign current as new previous

    //ensure WiFI is connected before fetching new data
    if(WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Fetching new data...");
      twitchInstance.updateData();
    }
  }

  //wait for an animation to complete before choosing next page to display
  if(p.displayAnimate())
  {
    switch(page)
    {
      case 0: //Twitch logo and live count
        p.setTextAlignment(PA_RIGHT);
        p.print(twitchInstance.getLiveCount().c_str());
        drawTwitchLogo(mx, 0, 24);

        delay(5000); //hold for 5 seconds
        page++;
        break;

      case 1: //Scrolling display of live channels
        lastOut = twitchInstance.printOutput();

        p.displayClear();
        p.displayText(lastOut.c_str(), PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

        page++;
        break;

      default: //default (aka the reset)
        page = 0;
        break;
    }
  }
}