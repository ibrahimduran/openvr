#define WIFI_SSID "ASUS_46_EX"
#define WIFI_PASS "P141264444"

#define SERVER_HOST "192.168.1.34"
#define SERVER_PORT 8888

#define PIN_LED_STATUS LED_BUILTIN
#define PIN_BTN_GRIP 15
#define PIN_BTN_APPLICATION 18
#define PIN_BTN_SYSTEM 19

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
