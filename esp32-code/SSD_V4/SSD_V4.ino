// -------------------------------------------------------------------------------------------------------------

// IMPORT LIBRARY
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

// BUAT STRUKTUR DATA
int timerTidur = 5;
int timerStandby = 10;
int mode = 1;               // Variabel untuk menyimpan mode saat ini
String incomingData = "";

// BUAT PRINT
String send = "";
String kode = "";
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

// ALOKASI PIN
const int buttonPin = 12;    // Pin untuk push button
const int buzzer = 13;
const int led1Pin = 2;

// VARIABEL AWAL
// Variabel Input
int buttonState = 0;        // Variabel untuk menyimpan status button
int lastbuttonState = 0;
bool isSleep = false;

// Variabel Gyro
#define THRESHOLD_ANGLE -2.5
#define THRESHOLD_ACCELERATION 1

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

void updateVariables() {
  if (incomingData.startsWith("T")) { // Jika incomingData diawali dengan "T"
    int newTimerTidur = incomingData.substring(1).toInt(); // Ambil nilai setelah huruf "T" dan ubah ke tipe data integer
    if (newTimerTidur != timerTidur) { // Jika nilai baru berbeda dengan nilai sebelumnya
      timerTidur = newTimerTidur; // Perbarui nilai variabel timerTidur
      Serial.print("Timer tidur diubah menjadi "); // Tampilkan pesan ke Serial Monitor
      Serial.println(timerTidur);
    }
  }
  
  if (incomingData.startsWith("S")) { // Jika incomingData diawali dengan "S"
    int newTimerStandby = incomingData.substring(1).toInt(); // Ambil nilai setelah huruf "S" dan ubah ke tipe data integer
    if (newTimerStandby != timerStandby) { // Jika nilai baru berbeda dengan nilai sebelumnya
      timerStandby = newTimerStandby; // Perbarui nilai variabel timerStandby
      Serial.print("Timer standby diubah menjadi "); // Tampilkan pesan ke Serial Monitor
      Serial.println(timerStandby);
    }
  }
  
  if (incomingData.startsWith("M")) { // Jika incomingData diawali dengan "M"
    int newMode = incomingData.substring(1).toInt(); // Ambil nilai setelah huruf "M" dan ubah ke tipe data integer
    if (newMode != mode) { // Jika nilai baru berbeda dengan nilai sebelumnya
      mode = newMode; // Perbarui nilai variabel mode
      Serial.print("Mode diubah menjadi "); // Tampilkan pesan ke Serial Monitor
      Serial.println(mode);
    }
  }
  
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

// FUNGSI PENGGERAK SERVO
void swipe() {
  pos += increment;
  if (pos >= 180 || pos <= 0) {
    increment = -increment;
  }
  servoMotor.write(pos);
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

  // Selalu update nilai string yang baru diterima oleh bluetooth
  if (Serial.available() > 0) {
    incomingData = Serial.readString();  // Read the incoming data from the serial buffer
    updateVariables(); // Jalankan fungsi updateVariables() untuk memperbarui nilai variabel sesuai dengan incomingData
    Serial.println("Received data: " + incomingData);  // Print the incoming data to the serial console
    // Serial.print("timerTidur : ");
    // Serial.print(timerTidur);
    // Serial.print(" timerStandby : ");
    // Serial.print(timerStandby);
    // Serial.print(" mode : ");
    // Serial.println(mode);
  }
  
  // PROGRAM MENENTUKAN MODE
  if (buttonState == HIGH) {   // Jika button ditekan
    mode = (mode % 3) + 1; // Tambah mode
    delay(200); // Tunggu 200 millisecond untuk debouncing
  }

  // Gunakan switch case untuk mengatur tindakan berdasarkan mode
  switch (mode) {
    case 1:
      // Bunyikan Buzzer 1 kali
      buzzerOn(1);

      // Tindakan untuk mode 1, Reset nilai
      digitalWrite(led1Pin, HIGH);
      servoMotor.write(0);
      digitalWrite(buzzer, LOW);

      break;
    case 2:
      // Bunyikan Buzzer 2 kali
      buzzerOn(2);
      // Tindakan untuk mode 2
      mainSSD(false);
      break;
    case 3:
      // Bunyikan Buzzer 3 kali
      buzzerOn(3);

      // Tindakan untuk mode 2
      mainSSD(true);
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
  // Serial.print("AccelX:");
  // Serial.print(a.acceleration.x);
  // Serial.print(",");
  // Serial.print("AccelY:");
  // Serial.print(a.acceleration.y);
  // Serial.print(",");
  // Serial.print("AccelZ:");
  // Serial.print(a.acceleration.z);
  // Serial.print(", ");
  // Serial.print("GyroX:");
  // Serial.print(g.gyro.x);
  // Serial.print(",");
  // Serial.print("GyroY:");
  // Serial.print(g.gyro.y);
  // Serial.print(",");
  // Serial.print("GyroZ:");
  // Serial.print(g.gyro.z);
  // Serial.print(",");
  // Serial.print("headAngle:");
  // Serial.print(headAngle);
  // Serial.print(",");
  // Serial.print("headAccel:");
  // Serial.print(headAccel);
  // Serial.println(" ");

  delay(10);
  
  // INISIASI WAKTU SEKARANG
  unsigned long currentTime = millis();
  unsigned long timeDiff = currentTime - lastUpdateTime;
  
  moving = isMoving();
  
  if (moving) {
    lastUpdateTime = currentTime;
    send ="Kepala terdeteksi bergerak";
    kode = "K0";

    digitalWrite(led1Pin, HIGH);
    servoMotor.write(0);
  }
  
  if (!moving) {

    digitalWrite(led1Pin, LOW);
    digitalWrite(buzzer, LOW);    

    if (isHeadBelowThreshold() == true && timeDiff > timerTidur * 1000){
      swipe();
      digitalWrite(led1Pin, HIGH);
      send = "Menunduk terdeteksi, gerakkan kepala anda";
      kode = "K1";

      // Jika Buzz di atur On
      if(buzz){
        digitalWrite(buzzer, HIGH);
      }
      //Update value isSleep
      isSleep = true;

    }
    else if (isHeadBelowThreshold() == false && timeDiff > timerStandby * 1000)
    {
      swipe();
      digitalWrite(led1Pin, HIGH);
      send = "Idle terdeteksi, gerakkan kepala anda";
      kode = "K2";

      // JIka BUzz on
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

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.print("Nilai "); // Tampilkan pesan ke Serial Monitor
      Serial.println(send);
      Serial.println("Received data: " + kode);
  }
  
  lastHeadAngle = headAngle;
  lastHeadAccel = headAccel;
  delay(100);
}

}



