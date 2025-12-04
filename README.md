# Twitch-LED-Matrix-Display
Uses the MAX7219 and an ESP8266 to connect to the Twitch API to display followed channels that are currently live.

## Function
Connects to the official Twitch API to display information regarding followed channels. It's current functionality includes the channel name, stream category, and the live count.

## Project Scope
This was created using an ESP8266 / Wemos D1 mini, and an MAX7219 with four matrices for a 32x8 display. The code was programmed using the Arduino framework and compiled with Platformio. While it has not been tested with other boards, it should work with some modifications to support ESP32.

## Using the Twitch Class
Efforts were made so that the code can be used with other projects that may utilize additional functions of the LED matrix. Using the API requires the user to create a developer application, then validating authorization for an app token. These are then stored as part of initializing the Twitch class object.

When a Twitch object is created, the begin() method should be called, which will then attempt to retrieve an access token and refresh token if saved on the system. These use the LittleFS library to save and load from internal storage. If an access key is not found, an API call to generate the access token is made using createTokens(). If successful, further calls can be made with the Twitch instance.

Data should be called using updateData() within reasonable intervals. The default used in this project is 5 minutes. Retrieved data from the API is stored in a vector initialized as TwitchChannel objects, which store limited information used as described in the function section. As part of updateData(), the buildStreamInfoString() method is called to create an Arduino string stored within the Twitch instance. The Twitch API returns data as JSON, so the ArduinoJson library is utilized as a dependency to extract necessary values.

The formatted output is called using printOutput(). It is recommended NOT to directly use this with the displayText() or print() methods for the MD_MAX72XX or MD_Parola object instances, as testing has shown the text is not properly output. Call the method to another string variable, then use that with c_str() in place of the LED matrix methods.

During runtime, it is likely the access token will expire. When calls to updateData() are made, the renewAccessToken() method should be called automatically to generate new tokens. Using the Twitch API to get new tokens has presented issues due to RAM limitations of the ESP8266. While a fix has been implemented, it remains untested over long-term usage.

## Compile This Project
If you wish to run this project as-is, you'll need to update the main.cpp and twitch.h define fields. You'll need to update:
* SSID - your wifi's ssid
* password - your wifi's password
* TWITCH_CLIENT_ID - the client ID when creating an application in the Twitch Dev site
* TWITCH_CLIENT_SECRET - the client secret after creating the application
* TWITCH_USER_ID - your Twitch account ID. You'll need to find this yourself using a GET command or using a 3rd party site.
* TWITCH_APP_TOKEN - when you make your app, you'll need to get OAuth authorized. You'll want the code it returns such as in the localhost address.

Using a Wemos D1 Mini, I have the following pins from the board to the LED matrix:
* VCC = 5V
* GND = G
* DIN = D8
* CS = D7
* CLK = D5

This project was built using platformio. Copy over all the files, keeping them in their directories. Create a new project in Visual Studio Code with the Platformio extension installed. Be sure to select the right board. Then be sure you have a data folder made for LittleFS, and the platformio.ini file has the proper lib_deps and board_build.filesystem fields. Build and run, and it should work.

## Copyright and Usage Notice
This project is not associated with Twitch and does not attempt to make any infringement on its copyrights. Users are at their own risk and discretion in regards to the safety of their credentials.
