// D:\Experiments\openvr_device\lib\ble_hid.h

char ReportDescriptor[55] = {
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    //   USAGE (Joystick)
    0xa1, 0x01,                    //   COLLECTION (Application)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x33,                    //     USAGE (Rx)
    0x09, 0x34,                    //     USAGE (Ry)
    0x09, 0x35,                    //     USAGE (Rz)
    0x17, 0x01, 0x00, 0x00, 0x80,  //     LOGICAL_MINIMUM (-2147483647)
    0x27, 0xff, 0xff, 0xff, 0x7f,  //     LOGICAL_MAXIMUM (2147483647)
    0x75, 0x20,                    //     REPORT_SIZE (32)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

// static const uint8_t _hidReportDescriptor[] = {
//   USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
//   USAGE(1),            0x05, // USAGE (Gamepad)
//   COLLECTION(1),       0x01, // COLLECTION (Application)
//   USAGE(1),            0x01, //   USAGE (Pointer)
//   COLLECTION(1),       0x00, //   COLLECTION (Physical)
//   REPORT_ID(1),        0x01, //     REPORT_ID (1)
//   // ------------------------------------------------- Buttons (1 to 14)
//   USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
//   USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
//   USAGE_MAXIMUM(1),    0x0e, //     USAGE_MAXIMUM (Button 14)
//   LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
//   LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
//   REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
//   REPORT_COUNT(1),     0x0e, //     REPORT_COUNT (14)
//   HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;14 button bits
//   // ------------------------------------------------- Padding
//   REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
//   REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
//   HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;2 bit padding
//   // ------------------------------------------------- X/Y position, Z/rZ position
//   USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
//   USAGE(1),            0x30, //     USAGE (X)
//   USAGE(1),            0x31, //     USAGE (Y)
//   USAGE(1),            0x32, //     USAGE (Z)
//   USAGE(1),            0x35, //     USAGE (rZ)
//   LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
//   LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
//   REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
//   REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
//   HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;4 bytes (X,Y,Z,rZ)

//   USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
//   USAGE(1),            0x33, //     USAGE (rX) Left Trigger
//   USAGE(1),            0x34, //     USAGE (rY) Right Trigger
//   LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
//   LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
//   REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
//   REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
//   HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;2 bytes rX, rY

//   USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
//   USAGE(1),            0x39, //     USAGE (Hat switch)
//   USAGE(1),            0x39, //     USAGE (Hat switch)
//   LOGICAL_MINIMUM(1),  0x01, //     LOGICAL_MINIMUM (1)
//   LOGICAL_MAXIMUM(1),  0x08, //     LOGICAL_MAXIMUM (8)
//   REPORT_SIZE(1),      0x04, //     REPORT_SIZE (4)
//   REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
//   HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;1 byte Hat1, Hat2

//   END_COLLECTION(0),         //     END_COLLECTION
//   END_COLLECTION(0)          //     END_COLLECTION
// };
