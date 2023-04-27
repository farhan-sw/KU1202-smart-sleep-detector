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
  Serial.begin(115200);

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

# FUNGSI MENDETEKSI APAKAH USER BERGERAK
bool isMoving(){
  
  if (abs(headAccel - lastHeadAccel) >= THRESHOLD_ACCELERATION) {
    return true;
  }
  return false;
  
}

# FUNGSI APAKAH USER MENUNDUK
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
      // Tindakan untuk mode 1
      digitalWrite(led1Pin, HIGH);
      servoMotor.write(0);
      digitalWrite(buzzer, LOW);
      break;
    case 2:
      // Tindakan untuk mode 2
      mainSSD(false);
      break;
    case 3:
      // Tindakan untuk mode 2
      mainSSD(true);
      break;
  }
}

# MAIN PROGRAM SSD
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
    }
    else if (isHeadBelowThreshold() == false && timeDiff > THRESHOLD_DURATION_MOVING * 1000)
    {
      swipe();
      digitalWrite(led1Pin, HIGH);
      Serial.println("Idle terdeteksi, gerakkan kepala anda");
      if(buzz){
        digitalWrite(buzzer, HIGH);
      }

    }
  }
  

  lastHeadAngle = headAngle;
  lastHeadAccel = headAccel;
  delay(100);
}

# FUNGSI PENGGERAK SERVO
void swipe() {
  pos += increment;
  if (pos >= 180 || pos <= 0) {
    increment = -increment;
  }
  servoMotor.write(pos);
}
