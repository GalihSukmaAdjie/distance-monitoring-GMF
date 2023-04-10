#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TFLI2C.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);  //Define software serial, 3 is TX, 2 is RX
char buff[4] = { 0x80, 0x06, 0x03, 0x77 };
unsigned char data[11] = { 0 };

TFLI2C tflI2C;

int16_t tfDist;  // distance in centimeters
int16_t tfAddr = TFL_DEF_ADR;

// pilih I2C BUS untuk sensor
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  // Serial.println(bus);
}

float distance, sen1, sen2;
String kon1, kon2;


void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  // Start I2C communication with the Multiplexer
  Wire.begin();
}

void loop() {
  // sensor 30 meter
  mySerial.print(buff);
  while (1) {
    if (mySerial.available() > 0)  //Determine whether there is data to read on the serial
    {
      delay(50);
      for (int i = 0; i < 11; i++) {
        data[i] = mySerial.read();
      }
      unsigned char Check = 0;
      for (int i = 0; i < 10; i++) {
        Check = Check + data[i];
      }
      Check = ~Check + 1;
      if (data[10] == Check) {
        if (data[3] == 'E' && data[4] == 'R' && data[5] == 'R') {
          distance = 999;
          Serial.println("Out of range");
        } else {
          distance = (data[3] - 0x30) * 100 + (data[4] - 0x30) * 10 + (data[5] - 0x30) * 1 + (data[7] - 0x30) * 0.1 + (data[8] - 0x30) * 0.01 + (data[9] - 0x30) * 0.001;
          // Serial.print("Lidar Jauh\t: ");
          // Serial.print(distance,1);
          // Serial.println(" cm");

          tf_luna();
        }
      } else {
        Serial.println("Data ERROR!");
      }
    }
    Serial.print("x0.txt=\"");
    Serial.print(distance);
    Serial.print("\"");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("x1.txt=\"");
    Serial.print(kon1);
    Serial.print("\"");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("x2.txt=\"");
    Serial.print(kon2);
    Serial.print("\"");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    
  }

  delay(20);
}

void tf_luna() {
  // sensor 8 meter
  // i2c pilih bus 2
  TCA9548A(2);
  if (tflI2C.getData(tfDist, tfAddr))  // If read okay...
  {
    sen1 = tfDist * 1;
    sen1 = sen1 / 100;
    kon1 = String(sen1) + " M";    
    // print the data...
  } else {
    kon1 = tflI2C.printStatus();
    Serial.print(kon1);    
  }  // else, print error.
  // i2c pilih bus 3
  TCA9548A(3);
  if (tflI2C.getData(tfDist, tfAddr))  // If read okay...
  {
    sen2 = tfDist * 1;
    sen2 = sen2 / 100;
    kon2 = String(sen2) + " M";
  } else {
    kon2 = tflI2C.printStatus();
    Serial.print(kon2);
  } //sen2 = 999;  // else, print error.
  
  Serial.println("__________________________________");
}