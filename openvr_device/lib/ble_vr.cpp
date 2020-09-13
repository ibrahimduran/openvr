#include "sdkconfig.h"
#include "ble_vr.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"

#include "ble_hid.h"

BleVRConnectionStatus::BleVRConnectionStatus()
{
}

void BleVRConnectionStatus::onConnect(BLEServer* pServer)
{
    this->connected = true;
    BLE2902* desc = (BLE2902*) this->characteristic->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902));
    desc->setNotifications(true);
}

void BleVRConnectionStatus::onDisconnect(BLEServer* pServer)
{
    this->connected = false;
    BLE2902* desc = (BLE2902*) this->characteristic->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902));
    desc->setNotifications(false);
}

BleVR::BleVR(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) : hid(0)
{
    this->size = 13;
    this->data = new uint8_t[this->size];
    memset(this->data, 0, this->size);

    this->deviceName = deviceName;
    this->deviceManufacturer = deviceManufacturer;
    this->batteryLevel = batteryLevel;
    this->connectionStatus = new BleVRConnectionStatus();
}

void BleVR::begin(void)
{
    xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleVR::end(void)
{
}

bool BleVR::isConnected(void) {
return this->   connectionStatus->connected;
}

void BleVR::setBatteryLevel(uint8_t level) {
    this->batteryLevel = level;
    if (hid != 0)
        this->hid->setBatteryLevel(this->batteryLevel);
}


void BleVR::setGripPressed(bool pressed)
{
    this->data[0] = pressed ? (this->data[0] | 0b00000100) : (this->data[0] & 0b11111011);
}

void BleVR::setSystemPressed(bool pressed)
{
    this->data[0] = pressed ? (this->data[0] | 0b00000001) : (this->data[0] & 0b11111110);
}

void BleVR::setApplicationPressed(bool pressed)
{
    this->data[0] = pressed ? (this->data[0] | 0b00000010) : (this->data[0] & 0b11111101);
}

void BleVR::setRotation(float x, float y, float z)
{
    memcpy(&this->data[1], &x, 4);
    memcpy(&this->data[5], &y, 4);
    memcpy(&this->data[9], &z, 4);
    this->characteristic->setValue(this->data, this->size);
}

void BleVR::setRotationX(float value)
{
    memcpy(&this->data[1], &value, 4);
}

void BleVR::setRotationY(float value)
{
    memcpy(&this->data[5], &value, 4);
}

void BleVR::setRotationZ(float value)
{
    memcpy(&this->data[9], &value, 4);
}

void BleVR::notify()
{
    this->characteristic->notify();
}

void BleVR::taskServer(void* pvParameter) {
  BleVR* instance = (BleVR *) pvParameter; //static_cast<BleGamepad *>(pvParameter);

  BLEDevice::init(instance->deviceName);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(instance->connectionStatus);

  instance->hid = new BLEHIDDevice(pServer);
  instance->characteristic = instance->hid->inputReport(1); // <-- input REPORTID from report map
  instance->characteristic->setValue(instance->data, instance->size);
  instance->connectionStatus->characteristic = instance->characteristic;

  instance->hid->manufacturer()->setValue(instance->deviceManufacturer);

  instance->hid->pnp(0x01, 0x02e5, 0xabcd, 0x0110); // sig, vid, pid, version
  instance->hid->hidInfo(0x00, 0x01); // country, flags

  BLESecurity *pSecurity = new BLESecurity();

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  instance->hid->reportMap((uint8_t*) ReportDescriptor, sizeof(ReportDescriptor));
  instance->hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->addServiceUUID(instance->hid->hidService()->getUUID());
  pAdvertising->start();
  instance->hid->setBatteryLevel(instance->batteryLevel);

//   ESP_LOGD(LOG_TAG, "Advertising started!");
  vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}
