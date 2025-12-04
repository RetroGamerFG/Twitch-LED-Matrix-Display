#include <twitch.h>
#include <filesystem.h>

//initializer
Twitch::Twitch(String appToken, String clientID, String clientSecret, String userID)
{
    this->appToken = appToken;
    this->clientID = clientID;
    this->clientSecret = clientSecret;
    this->userID = userID;

    this->channels = std::vector<TwitchChannel>();
    this->startupValid = false;
}

//begin() - this should be the first method called after object initialization. Using <LittleFS.h> (stored as callable
    //functions in "filesystem.h"), an attempt to load the access key and refresh key will be performed. If the access key
    //is not found, futher calls are made.
//Inputs - none
//Output - none; flags the startupValid member on success (prevents other methods from running if failed to initialize)
void Twitch::begin()
{
    String accessKey = loadFromFS("/access_key.txt");
    String refreshKey = loadFromFS("/refresh_key.txt");

    if(accessKey == "null")
    {
        Serial.println("First boot? Getting new tokens...");

        if(!createTokens()) //if creating tokens fails, flag the startup invalid
        {
            startupValid = false;
        }
    }
    else
    {
        setAccessToken(accessKey);
        setRefreshToken(refreshKey);

        startupValid = true;
    }
}

//updateData() - this should be called whenever the output text should be updated. It includes calls to getLiveAccounts() and 
    //buildStreamInfoString() to create a new Arduino string stored as the 'lastBuiltString' member.
//Inputs - none
//Output - none
void Twitch::updateData()
{
    if(startupValid)
    {
        lastBuiltString = buildStreamInfoString();
    }
    else
    {
        lastBuiltString = "Twitch ::: Error preventing runtime...";
    }
}

//printOutput() - this should be called when text is about to be displayed to the LED matrix. Returns the string currently stored
    //in the 'lastBuiltString' member.
//Inputs - none
//Output - an Arduino string formatted as called from updateData()
String Twitch::printOutput()
{
    return lastBuiltString;
}

//getURLEncodedRefreshToken() - returns a URL-encoded version of the refreshToken
//Inputs - none
//Output - an Arduino String with the URL-encoded refreshToken
String Twitch::getURLEncodedRefreshToken()
{
    refreshToken.trim(); //force trim in event of whitespace leading/trailing

    String out = "";
    const char *hex = "0123456789ABCDEF";

    for (uint16_t i = 0; i < refreshToken.length(); i++) 
    {
        char c = refreshToken.charAt(i);

        if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~' ) 
        {
            out += c;
        } 
        else 
        {
            out += '%';
            out += hex[(c >> 4) & 0x0f];
            out += hex[c & 0x0f];
        }
    }
    return out;
}

//createTokens() - used during first boot, creates the access token and refresh token with valid credentials
//Inputs - none
//Output - a boolean based on operation success
bool Twitch::createTokens()
{
    WiFiClientSecure wifiClientInstance;
    HTTPClient httpClientInstance;

    wifiClientInstance.setInsecure();

    httpClientInstance.begin(wifiClientInstance, "https://id.twitch.tv/oauth2/token");
    httpClientInstance.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "client_id=" + clientID;
    postData += "&client_secret=" + clientSecret;
    postData += "&code=" + appToken;
    postData += "&grant_type=authorization_code";
    postData += "&redirect_uri=http://localhost:3000"; //change this if your redirect URL is different for any reason

    int code = httpClientInstance.POST(postData);

    String json = httpClientInstance.getString();
    httpClientInstance.end();

    if (code > 0 && code == HTTP_CODE_OK)
    {
        const size_t capacity = 4 * 1024;
        DynamicJsonDocument doc(capacity);
        DeserializationError error = deserializeJson(doc, json);

        if(error)
        {
            Serial.println("ERROR: Failed to retrieve refresh token...");
            return false;
        }

        this->refreshToken = doc["refresh_token"].as<String>();
        this->accessToken = doc["access_token"].as<String>();

        saveToFS(refreshToken, "/refresh_key.txt");
        saveToFS(accessToken, "/access_key.txt");
        return true;
    }
    else if (code == 400)
    {
        Serial.println("ERROR: Invalid authorization code provided...");
        return false;
    }

    return false;
}

//renewAccessToken() - when the access token expires (usually code 401), this uses the refresh token to update both
//Inputs - none
//Output - a boolean based on operation success
bool Twitch::renewAccessToken()
{
    HTTPClient httpClientInstance;
    WiFiClientSecure wifiClientInstance;

    wifiClientInstance.setBufferSizes(512, 512);
    wifiClientInstance.setInsecure();

    httpClientInstance.begin(wifiClientInstance, "https://id.twitch.tv/oauth2/token");
    httpClientInstance.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData;
    postData.reserve(256);
    postData +=
        "client_id=" + clientID +
        "&client_secret=" + clientSecret +
        "&grant_type=refresh_token&refresh_token=" + getURLEncodedRefreshToken();

    Serial.println(postData);
    Serial.println();

    int code = httpClientInstance.POST(postData);

    Serial.println("renewAccessToken() HTTP Code: " + String(code));

    String json = httpClientInstance.getString();
    Serial.println(json);
    Serial.println();
    httpClientInstance.end();

    if (code > 0 && code == HTTP_CODE_OK)
    {
        // Allocate a dynamic JSON document. Adjust size as needed for your responses.
        const size_t capacity = 4 * 1024; // 4 KB
        DynamicJsonDocument doc(capacity);
        DeserializationError error = deserializeJson(doc, json);

        if(error)
        {
            Serial.print("ERROR: Failed to deserialize JSON response" + json);
            return false;
        }

        this->refreshToken = doc["refresh_token"].as<String>();
        this->accessToken = doc["access_token"].as<String>();

        saveToFS(refreshToken, "/refresh_key.txt");
        saveToFS(accessToken, "/access_key.txt");
        return true;
    }

    return false;
}

//getLiveAccounts() - retrieves a user's followed accounts that are live, stores as TwitchChannel objects
//Inputs - none
//Output - updates the "channels" vector with TwitchChannel object(s)
void Twitch::getLiveAccounts()
{
    channels.clear(); //empties the current TwitchChannel vector to update for new instances (or none if no channels are live)

    WiFiClientSecure wifiClientInstance;
    HTTPClient httpClientInstance;

    String followed_url = "https://api.twitch.tv/helix/streams/followed?user_id=";
    followed_url += userID;

    String auth_header = "Bearer ";
    auth_header += accessToken;

    wifiClientInstance.setInsecure();

    httpClientInstance.begin(wifiClientInstance, followed_url);
    httpClientInstance.addHeader("Client-Id", clientID);
    httpClientInstance.addHeader("Authorization", auth_header);
    httpClientInstance.addHeader("Accept", "application/json");

    int code = httpClientInstance.GET();

    String json = httpClientInstance.getString();
    httpClientInstance.end();

    if (code > 0 && code == HTTP_CODE_OK)
    {
        const size_t capacity = 4 * 1024; // 4 KB
        DynamicJsonDocument doc(capacity);
        DeserializationError error = deserializeJson(doc, json);

        if(error)
        {
            Serial.print("ERROR: Failed to deserialize JSON response" + json);
            return;
        }

        JsonArray data = doc["data"].as<JsonArray>();

        for (JsonObject item : data)
        {
            TwitchChannel instance = TwitchChannel(item["user_name"], item["game_name"], item["viewer_count"]);
            channels.push_back(instance);
        }

        Serial.println("Found " + String(channels.size()) + " live followed channels.");
        Serial.println();
    }
    else if (code == 401)
    {
        Serial.println("Access Token expired, refreshing...");
        Serial.println();
            
        if(renewAccessToken())
        {
            updateData();
        }
        else
        {
            Serial.println("Critical Failure: Could not refresh Access Token.");
            Serial.println();

            startupValid = false;
        }
    }
}

//buildStreamInfoString() - after getting live channels, creates an Arduino string that displays on the LED matrix
//Inputs - none; dep
//Output - an Arduino string formatted based on the TwitchChannel.buildStreamInfoString() method
String Twitch::buildStreamInfoString()
{
    getLiveAccounts();

    if(channels.size() == 0)
    {
        return "Twitch ::: No followed channels are live right now...";
    }

    String formattedOutput = "Twitch";

    for (TwitchChannel channel : channels)
    {
        formattedOutput += " ::: " + channel.buildStreamInfoString();
    }

    return formattedOutput;
}

//getLiveCount() - returns the size of the channels vector as an Arduino string
//Inputs - none
//Output - an Arduino string representing the count of live channels
String Twitch::getLiveCount()
{
    return String(channels.size());
}

//printDebug() - used for debugging, prints all Twitch instance members to serial
//Inputs - none
//Output - none; serial output only
void Twitch::printDebug()
{
    Serial.println("Twitch Instance Debug Info:");
    Serial.println("Client ID: " + clientID);
    Serial.println("Client Secret: " + clientSecret);
    Serial.println("User ID: " + userID);
    Serial.println("Access Token: " + accessToken);
    Serial.println("Refresh Token: " + refreshToken);
    Serial.println();
}

//initializer
TwitchChannel::TwitchChannel(String channelName, String streamTitle, String viewerCount)
{
    this->channelName = channelName;
    this->streamTitle = streamTitle;
    this->viewerCount = viewerCount;
}

//buildStreamInfoString() - builds and returns an Arduino string based on members within the TwitchChannel object
//Inputs - none
//Output - an Arduino string formatted based on criteria specified
String TwitchChannel::buildStreamInfoString()
{
    return this->channelName + " is streaming " + this->streamTitle + " with " + this->viewerCount + " viewers";
}