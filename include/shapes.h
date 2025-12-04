//Code created by: Bailey "itsRetro" Manczko
//This program is intended for hobbyist, and is licensed under Apache-2.0.

//Twitch LED Matrix Display - 'shapes.h'

//This header file and companion .cpp file is used for pre-defined shapes that can be added to the LED matrix.
//There is only one that is utilized in the main.cpp, the Twitch logo.

#ifndef SHAPES_H
#define SHAPES_H

#include <MD_MAX72xx.h>

void drawTwitchLogo(MD_MAX72XX&, int, int);

#endif