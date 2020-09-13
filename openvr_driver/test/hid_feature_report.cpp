#include <iostream>
#include <fstream>
#include <stdio.h>
#include "hidapi.h"
#include <Windows.h>

int main()
{
    struct hid_device_info *devs, *cur_dev;
    hid_device *handle = NULL;
    unsigned char data[2048];
    data[0] = 1;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;

    while (cur_dev) {
        if (cur_dev->vendor_id == 0xe502 && cur_dev->product_id == 0xcdab) {
            handle = hid_open_path(cur_dev->path);
            int readBytes = hid_get_input_report(handle, data, sizeof(data));
            if (readBytes == -1) {
                wprintf(hid_error(handle));
            } else {
                printf("Usage Page: %x\tUsage: %x\n", cur_dev->usage_page, cur_dev->usage);

                printf("Read %d bytes.", readBytes);
                std::ofstream myfile;
                myfile.open("C:\\Users\\Generic\\Desktop\\hid_report2.bin");
                for (int i = 0; i <= readBytes; i++) {
                    myfile << data[i];
                }
                myfile.close();
            }
        }

        cur_dev = cur_dev->next;
    }

    hid_free_enumeration(devs);

    return 0;
}
