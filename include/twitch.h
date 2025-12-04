//Code created by: Bailey "itsRetro" Manczko
//This program is intended for hobbyist, and is licensed under Apache-2.0.

//Twitch LED Matrix Display - 'twitch.h'

//This header file and companion .cpp file contains classes with methods that utilize the Twitch API in
//conjunction with the MD_MAX72XX LED matrix. See full usage instructions in the README.md

#ifndef TWITCH_H
#define TWITCH_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>

//Twitch API definitions (substitute your own values here)
#define TWITCH_CLIENT_ID "CLIENT_ID_HERE"
#define TWITCH_CLIENT_SECRET "CLIENT_SECRET_HERE"
#define TWITCH_USER_ID "USER_ID_HERE"
#define TWITCH_APP_TOKEN "APP_TOKEN_HERE"

class TwitchChannel
{
    public:
        TwitchChannel(String, String, String);

        String getChannelName() { return this->channelName; };
        void setChannelName(String name) { this->channelName = name; };

        String getStreamTitle() { return this->streamTitle; };
        void setStreamTitle(String title) { this->streamTitle = title; };

        String getViewerCount() { return this->viewerCount; };
        void setViewerCount(String count) { this->viewerCount = count; };

        String buildStreamInfoString();

    private:
        String channelName;
        String streamTitle;
        String viewerCount;
};

class Twitch
{
    public:
        Twitch(String, String, String, String);

        String getAccessToken() { return this->accessToken; }
        void setAccessToken(String token) { this->accessToken = token; }
        
        String getRefreshToken() { return this->refreshToken; }
        String getURLEncodedRefreshToken();
        void setRefreshToken(String token) { this->refreshToken = token; }

        void begin();
        void updateData();
        String printOutput();

        bool createTokens();
        bool renewAccessToken();

        void getLiveAccounts();
        String getLiveCount();
        void printDebug();
        
        String buildStreamInfoString();

    private:
        std::vector<TwitchChannel> channels;
        String clientID;
        String clientSecret;
        String userID;
        String appToken;
        String accessToken;
        String refreshToken;

        String lastBuiltString;
        bool startupValid;
};

#endif