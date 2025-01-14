#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>

#include "inc/fido_hid.h"

#include <hidapi/hidapi.h>


#define G310_HID_VID    0x311F
#define G310_HID_PID    0x4A2A

int fido_hid_init();
int fido_hid_deinit();
void* fido_hid_open(uint16_t vid, uint16_t pid, void *attribute);
void fido_hid_info(void *handle);


int main(int argc, char *argv[])
{

    int ret = 0;

    hid_device *handle = NULL;
    fido_device_info_t dev_info;
    fido_hid_info_t hid_info;

    if((ret = fido_hid_init()) != 0) {
        printf("fiod_hid_init() failed (%d) \n", ret);
        goto endret;
    }


    handle = fido_hid_open( G310_HID_VID, G310_HID_PID, NULL);

    if(handle == NULL) {
        printf("fido_hid_open fail (%p) \n", handle);
        goto endret;
    }


    hid_info.handle = handle;
    hid_info.dev_info = &dev_info;

    printf("assigned handle id : %p\n", hid_info.handle);

    fido_hid_info((void *)&hid_info);

    dev_info.handle = handle;
    fido_dev_ctapinit((void *)&dev_info);

endret:

    //fido_hid_deinit();
    return ret;
}
