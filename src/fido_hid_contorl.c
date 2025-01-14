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


int fido_dev_ctapinit(void *handle)
{
    int ret = 0;
    uint8_t buf[65] = {0, };
    uint8_t nonce[8] = {0, };

    uint8_t *resp = NULL;   // response 
    uint8_t *respd = NULL;  // response data
    uint8_t *req = NULL;    // request
                            //
    fido_device_info_t *h_info;
    hid_device *h_hid;


    h_info = (fido_hid_info_t *)handle;
    h_hid = (hid_device *)h_info->handle;

    // generate nonce 
    srand((uint32_t)time(NULL));
    for( int x = 0; x < 8; x++ ) {
        nonce[x] = (uint8_t)rand();
    }

    req = &buf[1];
    memset(&req[0], 0xFF, 4);
    req[4] = 0x86;
    req[5] = 0;
    req[6] = 8;
    memcpy(&req[7], nonce, 8);


    ret = hid_write(h_hid, buf, 65);
    if(ret <= 0) {
        fprintf(stderr, "hid write fail (%d) \n", ret);
        goto endret;
    }


    memset(buf, 0, sizeof buf);
    resp = &buf[0];

    ret = hid_read_timeout(h_hid, buf, 64, -1);
    if(ret <= 0) {
        fprintf(stderr, "read timeout (%d) \n", ret);
        goto endret;
    }


    printf("reponse b[6] = 0x%x \n", resp[6]);
    if(resp[6] != 17) {
        ret = -1;
        goto endret;
    }

    respd = &buf[7];

    if(memcmp(nonce, &respd[0], 8) != 0) {
        fprintf(stderr, "invalid nonce !!! \n");
        return -1;
    }

    memcpy(&h_info->cid,  &respd[8], 4);
    printf("cid : 0x%x \n", h_info->cid);

    h_info->protocol_v = respd[12];
    h_info->major_v = respd[13];
    h_info->minor_v = respd[14];
    h_info->build_v = respd[15];
    h_info->capa_flags = respd[16];

    printf("protocol version : 0x%x \n", h_info->protocol_v);
    printf("major version : 0x%x \n", h_info->major_v);
    printf("minor version : 0x%x \n", h_info->minor_v);
    printf("build version : 0x%x \n", h_info->build_v);
    printf("capabilities flags : 0x%x \n", h_info->capa_flags);
    
endret:

    return ret;

}

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




