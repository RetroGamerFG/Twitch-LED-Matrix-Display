//Code created by: Bailey "itsRetro" Manczko
//This program is intended for hobbyist, and is licensed under Apache-2.0.

//Twitch LED Matrix Display - 'filesystem.h'

//This header file and companion .cpp file was made to work with the <LittleFS.h> package. While in practice this is
//completely optional, the 'twitch.cpp' file makes use of its functions due to the requirement of having to store the
//Twitch API credentials during reboot.

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <LittleFS.h>
#include <Arduino.h> //needed for String class

void startLittleFS();
void saveToFS(String, String);
String loadFromFS(String);

#endif