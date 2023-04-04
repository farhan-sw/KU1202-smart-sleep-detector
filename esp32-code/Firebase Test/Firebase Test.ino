/*
project by makerindo
minggu 7 sepetember 2021
menulis data integer dan float di firebase
*/

// https://smart-sleep-detector-v1-1-default-rtdb.asia-southeast1.firebasedatabase.app/
// AIzaSyDe_nlzn_BVJYdLVwK0-4wgMVNnt3wuSYs

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Farhan.sw"
#define WIFI_PASSWORD "ikandisawah"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDe_nlzn_BVJYdLVwK0-4wgMVNnt3wuSYs"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smart-sleep-detector-v1-1-default-rtdb.asia-southeast1.firebasedatabase.app" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup(){
  Serial.begin(115200);
  setupWifi();
}

void loop(){
  updateDatabase();

}

void updateDatabase(){

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 200 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    sentData("Sensor", "headAngle", count);
    count++;

    sentData("Sensor", "headAccel", count+2);
    
  }
  
}

void setupWifi(){

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void sentData(String path, String name, int data){
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, (path + "/" + name), data)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
}
