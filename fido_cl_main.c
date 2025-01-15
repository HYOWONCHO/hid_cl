#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <hidapi/hidapi.h>

#include "debugs.h"
#include "inc/fido_hid.h"



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

    uint8_t tests[16] = { [0 ... 8] = 0x55, };
    hid_ctlv_t hh;
#if 0
    //hh.message[0] = tests;
    //memcpy(hh.message, tests, 16);
    //printf("hhmessage : %p , tests :%p \n", hh.message, tests);

    for(int x = 0; x < 16; x++) {
        printf("(%p) : 0x%x , (%p) : 0x%x \n", 
                &hh.message[x], hh.message[x],
                &tests[x], tests[x]);


    printf("\n");
#endif
   // ERR_JUMP_VAL(7);

    if((ret = fido_hid_init()) != 0) {
        printf("fiod_hid_init() failed (%d) \n", ret);
        goto endret;
    }


    handle = fido_hid_open( G310_HID_VID, G310_HID_PID, NULL);

    if(handle == NULL) {
        printf("fido_hid_open fail (%p) \n", handle);
        goto endret;
    }


    // Get the HID devce  informatoin 
    hid_info.handle = handle;
    hid_info.dev_info = &dev_info;

    printf("assigned handle id : %p\n", hid_info.handle);

    fido_hid_info((void *)&hid_info);

    // Transmit and request the CTAP init
    dev_info.handle = handle;
    fido_dev_ctapinit((void *)&dev_info);

endret:
    //err_printf("end return value : %d", ret);

    //fido_hid_deinit();
    return ret;
}
