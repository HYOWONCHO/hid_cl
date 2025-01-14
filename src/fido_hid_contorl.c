#include <hidapi/hidapi.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>
#include <string.h>

#include "../inc/fido_hid.h"

#ifndef true
#define true 1
#endif

#ifndef false 
#define false 0 
#endif


static bool gn_hidstatus = true;

int fido_hid_status(void)
{
    return gn_hidstatus;
}


int fido_hid_init(void)
{
    int ret = -1;

    
    ret = hid_init();
    if(ret != 0) {
        fprintf(stderr, "hidapi library initialize failed !!! \n");
        goto endret;
    }

    ret = 0;
    return ret;

endret:
    return ret;
}



int fido_hid_deinit(void)
{
    int ret = -1;

    ret = hid_exit();
    if(ret < 0) {
        fprintf(stderr, "hidapi library de-initialize failed \n");
        goto errdone;
    }
  
    ret = 0;
    gn_hidstatus = false;
    return ret;

errdone:
    return ret;
}

void fido_hid_info(void *handle)
{
    hid_device *h = NULL;
    fido_hid_info_t *fhi = NULL;
    wchar_t wstr[MAX_STR] = {0, };
    int ret = 0;

    if(handle == NULL) {
       fprintf(stderr, "Invalid handle !!! \n") ;
       goto endret;
    }


    fhi = (fido_hid_info_t *)handle;
    h = fhi->handle;


    ret = hid_get_manufacturer_string(h, wstr, MAX_STR);
    memcpy((void *)fhi->str_mf, (const void *)wstr, MAX_STR);
    printf("Manufacture String : %ls \n", fhi->str_mf);

    ret = hid_get_product_string(h, wstr, MAX_STR);
    memcpy((void *)fhi->str_pt, (const void *)wstr, MAX_STR);
    printf("Product String : %ls \n", fhi->str_pt);


    ret = hid_get_serial_number_string(h, wstr, MAX_STR);
    memcpy((void *)fhi->str_sn, (const void *)wstr, MAX_STR);
    printf("Serial Number : %ls \n", fhi->str_sn);


    ret = hid_get_indexed_string(h, 1, wstr, MAX_STR);
    memcpy((void *)fhi->str_is, (const void *)wstr, MAX_STR);
    printf("Indexed String : %ls \n", fhi->str_is);

endret:

    return;
}


void* fido_hid_open(uint16_t vid, uint16_t pid, void *attribute)
{

    hid_device *h = NULL;
    printf("vid : 0x%x , pid : 0x%x , attribute : %p \n", vid, pid, (const wchar_t *)attribute);
    // open the device using the VID and PID
    h = hid_open(vid, pid, attribute);
    printf("handle id : %p \n", h);
    if(h == NULL) {
        fprintf(stderr, "Unable to open device \n");
        goto endret;
    }

    return h;



endret:
    fido_hid_deinit();
    return NULL;
}




