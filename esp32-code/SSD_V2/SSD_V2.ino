// SETUP WIFI DAN DATABASE
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
// -------------------------------------------------------------------------------------------------------------

// IMPORT LIBRARY
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

// ALOKASI PIN
const int buttonPin = 12;    // Pin untuk push button
const int buzzer = 13;
const int led1Pin = 2;

// VARIABEL AWAL
// Variabel Input
int buttonState = 0;        // Variabel untuk menyimpan status button
int mode = 1;               // Variabel untuk menyimpan mode saat ini
int lastbuttonState = 0;
bool isSleep = false;

// Variabel Gyro
#define THRESHOLD_ANGLE -2.5
#define THRESHOLD_ACCELERATION 1
#define THRESHOLD_DURATION_MOVING 10
#define THRESHOLD_DURATION_IDLE 5

// Setup Servo
Servo servoMotor;
int servoPin = 14;
int pos = 0;
int increment = 40;

// SETUP VARIABEL GYRO
int headAngle = 0;
int headAccel = 0;
int lastHeadAngle = 0;
int lastHeadAccel = 0;
unsigned long lastUpdateTime = 0;
bool moving = true;

Adafruit_MPU6050 mpu;


// SETUP
void setup() {
  Serial.begin(9600);
  // SETUP WIFI DAN DATABASE
  setupWifi();

  // INISIASI PIN INPUT DAN OUTPUT
  pinMode(buttonPin, INPUT);  // Inisialisasi buttonPin sebagai input
  pinMode(led1Pin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // SETUP SERVO
  servoMotor.attach(servoPin);

  // INISIASI GYRO SENSOR
  // Memastikan Koneksi
  if (!mpu.begin()) {
    Serial.println("Gagal terkoneksi MPU6050");
    while (1) {
      delay(10);
    }
  }
  // Inisiasi Parameter
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
  

}

// FUNGSI MENDETEKSI APAKAH USER BERGERAK
bool isMoving(){
  
  if (abs(headAccel - lastHeadAccel) >= THRESHOLD_ACCELERATION) {
    return true;
  }
  return false;
  
}

// FUNGSI APAKAH USER MENUNDUK
bool isHeadBelowThreshold() {
  if (headAngle <= THRESHOLD_ANGLE) {
    return true;
  }
  return false;
}

// LOOP
void loop() {
  buttonState = digitalRead(buttonPin);  // Baca status button
  
  // PROGRAM MENENTUKAN MODE
  if (buttonState == HIGH) {   // Jika button ditekan
    mode = (mode % 3) + 1; // Tambah mode
    delay(200); // Tunggu 200 millisecond untuk debouncing
  }
  Serial.println(mode);

  // Gunakan switch case untuk mengatur tindakan berdasarkan mode
  switch (mode) {
    case 1:
      // Bunyikan Buzzer 1 kali
      buzzerOn(1);

      // Tindakan untuk mode 1
      digitalWrite(led1Pin, HIGH);
      servoMotor.write(0);
      digitalWrite(buzzer, LOW);

      // UPDATE DATABASE
      updateDatabase();

      break;
    case 2:
      // Bunyikan Buzzer 2 kali
      buzzerOn(2);
      // Tindakan untuk mode 2
      mainSSD(false);

      // UPDATE DATABASE
      updateDatabase();

      break;
    case 3:
      // Bunyikan Buzzer 3 kali
      buzzerOn(3);

      // Tindakan untuk mode 2
      mainSSD(true);

      // UPDATE DATABASE
      updateDatabase();

      break;
  }
}

// MAIN PROGRAM SSD
void  mainSSD(bool buzz) {
  
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  headAngle = a.acceleration.z;
  headAccel = sqrt(sq(g.gyro.x) + sq(g.gyro.y) + sq(g.gyro.z));

  /* Print out the values */
  Serial.print("AccelX:");
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print("AccelY:");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print("AccelZ:");
  Serial.print(a.acceleration.z);
  Serial.print(", ");
  Serial.print("GyroX:");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print("GyroY:");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print("GyroZ:");
  Serial.print(g.gyro.z);
  Serial.print(",");
  Serial.print("headAngle:");
  Serial.print(headAngle);
  Serial.print(",");
  Serial.print("headAccel:");
  Serial.print(headAccel);
  Serial.println(" ");

  delay(10);
  
  // INISIASI WAKTU SEKARANG
  unsigned long currentTime = millis();
  unsigned long timeDiff = currentTime - lastUpdateTime;
  
  moving = isMoving();
  
  if (moving) {
    lastUpdateTime = currentTime;
    Serial.println("Kepala terdeteksi bergerak");
    digitalWrite(led1Pin, HIGH);
    servoMotor.write(0);
  }
  
  if (!moving) {

    digitalWrite(led1Pin, LOW);
    digitalWrite(buzzer, LOW);    

    if (isHeadBelowThreshold() == true && timeDiff > THRESHOLD_DURATION_IDLE * 1000){
      swipe();
      digitalWrite(led1Pin, HIGH);
      Serial.println("Menunduk terdeteksi, gerakkan kepala anda");
      if(buzz){
        digitalWrite(buzzer, HIGH);
      }
      //Update value isSleep
      isSleep = true;

    }
    else if (isHeadBelowThreshold() == false && timeDiff > THRESHOLD_DURATION_MOVING * 1000)
    {
      swipe();
      digitalWrite(led1Pin, HIGH);
      Serial.println("Idle terdeteksi, gerakkan kepala anda");
      if(buzz){
        digitalWrite(buzzer, HIGH);
      }
      //Update value isSleep
      isSleep = true;      
    }

    else{
      
      //Update value isSleep
      isSleep = false;  

    }
  }
  

  lastHeadAngle = headAngle;
  lastHeadAccel = headAccel;
  delay(100);
}

// FUNGSI PENGGERAK SERVO
void swipe() {
  pos += increment;
  if (pos >= 180 || pos <= 0) {
    increment = -increment;
  }
  servoMotor.write(pos);
}

// PROSEDUR MEMBUNYIKAN BUZZER SEBANYAK N KALI
void buzzerOn(int n){
  while(lastbuttonState != mode){
    for (int i=0; i<n; i++){
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }

    lastbuttonState = mode;
  }
}

// FUNGSI MENGUPDATE DATABASE
void updateDatabase(){
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // DATA YANG AKAN DIKIRIM
    // sentData("Sensor", "headAngle", headAngle);
    // sentData("Sensor", "headAccel", headAccel);
    // sentData("Sensor", "mode", mode);

    // UPLOAD KONDISI TIDUR/TIDAK
    if (Firebase.RTDB.setInt(&fbdo, "Sensor/isSleep", isSleep)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // UPLOAD KONIDSI MENUNDUK ATAU TIDA
    if (Firebase.RTDB.setInt(&fbdo, "Sensor/headAngle", headAngle)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // UPLOAD SENSOR TOMBOL
    if (Firebase.RTDB.setInt(&fbdo, "Sensor/mode", mode)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }



  }
}

// FUNGSI MENGIRIM DATA KE DATABASE
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

void setupWifi(){

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while ((WiFi.status() != WL_CONNECTED) and (millis() < 5000)){
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
