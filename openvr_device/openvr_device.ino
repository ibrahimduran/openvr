#include <Wire.h>
#include <EEPROM.h>

#include "deps/MPU6050_light/src/MPU6050_light.h"
#include "deps/MPU6050_light/src/MPU6050_light.cpp"

#include "lib/config.h"
#include "lib/ble_vr.h"
#include "lib/ble_vr.cpp"

BleVR ble = BleVR("OpenVR Kontrolcu", "OpenVR", 100);

MPU6050 mpu(Wire);

int timer = 0;
int timer2 = 0;
int sentBytes = 0;

bool clientWasConnected = false;
bool applicationButtonWasPressed = false;
bool systemButtonWasPressed = false;
bool gripButtonWasPressed = false;

float offsetGyroX = 0.0f;
float offsetGyroY = 0.0f;
float offsetGyroZ = 0.0f;

float Rx = 0.0f;
float Ry = 0.0f;
float Rz = 0.0f;

float Qx = 0.0f;
float Qy = 0.0f;
float Qz = 0.0f;
float Qw = 0.0f;

uint8_t * buffer = new uint8_t[48];

void calibrate() {
    Serial.print("Calculating offset values");

    mpu.calcGyroOffsets();

    for (int i = 0; i < 1000; i++) {
        mpu.update();
        delay(10);
        offsetGyroX = (offsetGyroX + mpu.getAngleX()) / (i == 0 ? 1 : 2);
        offsetGyroY = (offsetGyroY + mpu.getAngleY()) / (i == 0 ? 1 : 2);
        offsetGyroZ = (offsetGyroZ + mpu.getAngleZ()) / (i == 0 ? 1 : 2);

        if (i % 100 == 0) {
            Serial.print(".");
        }

        if (i % 10 == 0) {
            digitalWrite(PIN_LED_STATUS, i / 10 % 2 == 1 ? LOW : HIGH);
        }
    }

    EEPROM.writeFloat(0, offsetGyroX);
    EEPROM.writeFloat(4, offsetGyroY);
    EEPROM.writeFloat(8, offsetGyroZ);
    EEPROM.writeFloat(12, mpu.getGyroXoffset());
    EEPROM.writeFloat(16, mpu.getGyroYoffset());
    EEPROM.writeFloat(20, mpu.getGyroZoffset());

    Serial.printf("Gyro Offsets\tX: %f, Y: %f, Z: %f\n", offsetGyroX, offsetGyroY, offsetGyroZ);
}

void setup() {
    pinMode(PIN_LED_STATUS, OUTPUT);
    pinMode(PIN_BTN_GRIP, INPUT_PULLDOWN);
    pinMode(PIN_BTN_SYSTEM, INPUT_PULLDOWN);
    pinMode(PIN_BTN_APPLICATION, INPUT_PULLDOWN);

    offsetGyroX = EEPROM.readFloat(0);
    offsetGyroY = EEPROM.readFloat(4);
    offsetGyroZ = EEPROM.readFloat(8);

    mpu.setGyroOffsets(EEPROM.readFloat(12), EEPROM.readFloat(16), EEPROM.readFloat(20));

    Serial.begin(9600);
    delay(1000);

    Serial.println("Initializing sensors");
    Wire.begin();
    mpu.begin();

    /**
     * Iniitialize bluetooth
     */
    Serial.printf("Initializing bluetooth server\n");
    ble.begin();

    Serial.println("Setup complete");
}

bool connected = false;

void loop() {
    if (digitalRead(PIN_BTN_APPLICATION) == HIGH && digitalRead(PIN_BTN_SYSTEM) == HIGH) {
        digitalWrite(PIN_LED_STATUS, HIGH);
        delay(3000);
        digitalWrite(PIN_LED_STATUS, LOW);
        calibrate();
    }

    if (ble.isConnected()) {
        if (!connected) {
            Serial.println("Bluetooth connected");
            connected = true;
        }
    } else {
        if (connected) {
            Serial.println("Bluetooth disconnected");
            connected = false;
        }
    }

    if (!connected) {
        return;
    }

    // bleGamepad.setAxes(joyX - 128, joyY - 128);
    mpu.update();

    Rx = mpu.getAngleX() - offsetGyroX;
    Ry = mpu.getAngleY() - offsetGyroY;
    Rz = mpu.getAngleZ() - offsetGyroZ;

    if (digitalRead(PIN_BTN_GRIP) == HIGH) {
        if (!gripButtonWasPressed) {
            uint8_t data = 1;
            ble.setGripPressed(true);
            gripButtonWasPressed = true;
            Serial.println("Grip pressed!");
        }
    } else {
        if (gripButtonWasPressed) {
            uint8_t data = 0;
            ble.setGripPressed(false);
            gripButtonWasPressed = false;
            Serial.println("Grip released!");
        }
    }

    if (digitalRead(PIN_BTN_APPLICATION) == HIGH) {
        if (!applicationButtonWasPressed) {
            uint8_t data = 1;
            ble.setApplicationPressed(true);
            applicationButtonWasPressed = true;
        }
    } else {
        if (applicationButtonWasPressed) {
            uint8_t data = 0;
            ble.setApplicationPressed(false);
            applicationButtonWasPressed = false;
        }
    }

    if (digitalRead(PIN_BTN_SYSTEM) == HIGH) {
        if (!systemButtonWasPressed) {
            uint8_t data = 1;
            ble.setSystemPressed(true);
            systemButtonWasPressed = true;
        }
    } else {
        if (systemButtonWasPressed) {
            uint8_t data = 0;
            ble.setSystemPressed(false);
            systemButtonWasPressed = false;
        }
    }

    ble.setRotation(Rx, Ry, Rz);

    ble.notify();
    delay(50);
}
