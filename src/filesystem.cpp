#include "filesystem.h"

//startLittleFS() - calls LittleFS.begin()
//Inputs - none
//Output - none
void startLittleFS()
{
    LittleFS.begin();
}

//saveToFS() - saves an Arduino string to a specified file path such as '/file.txt'
//Inputs -
    //value - a string to save within a file
    //path - a valid path with file extension
//Output - None; saves a file within /data if successful
void saveToFS(String value, String path)
{
    File file = LittleFS.open(path, "w");

    file.println(value);

    file.close();

    Serial.println("Wrote the following to " + path + ": " + value);
    Serial.println();
}

//loadFromFS() - loads an Arduino string that is stored within a specified file path such as '/file.txt'
//Inputs - path - a valid path with file extension
//Output - an Arduino string extracted from the file
String loadFromFS(String path)
{
    if(!LittleFS.exists(path))
    {
        return "null";
    }

    File file = LittleFS.open(path, "r");

    if(file)
    {
        String output = "";

        while(file.available())
        {
            output += file.readStringUntil('\n');
        }

        Serial.println("Loaded value from " + path + ": " + output);

        return output;
    }

    return "null";
}