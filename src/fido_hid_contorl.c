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

#define HID_PACKET              (0x80)
#define HID_CTAP_PING           (HID_PACKET | 0x01)
#define HID_CTAP_MSG            (HID_PACKET | 0x03)
#define HID_CTAP_LOCK           (HID_PACKET | 0x04)
#define HID_CTAP_INIT           (HID_PACKET | 0x06)
#define HID_CTAP_WINK           (HID_PACKET | 0x08)
#define HID_CTAP_CBOR           (HID_PACKET | 0x10)
#define HID_CTAP_CANCEL         (HID_PACKET | 0x11)
#define HID_CTAP_KEEPALIVE      (HID_PACKET | 0x3B)
#define HID_CTAP_ERROR          (HID_PACKET | 0x3F)

#define HID_USER_UAF            (HID_PACKET | 0x41)
#define HID_USER_FINGERPRINT    (HID_PACKET | 0x51)
#define HID_USER_UTIL           (HID_PACKET | 0x52)
#define HID_USER_BULK           (HID_PACKET | 0x53)
#define HID_USER_OTP            (HID_PACKET | 0x55)

#define HID_PACKET_MAX          (64)
#define HID_PACKET1_DATA_SIZE   (57)
#define HID_PACKET2_DATA_SIZE   (59)
#define HID_SEQUENCE_MAX        (127)
#define HID_BCNT_MAX            (7550) // (HID_PACKET_MAX - 7) + (HID_PACKET_MAX - 5) * HID_SEQUENCE_MAX
#define HID_TIMEOUT             (5000)
#define HID_TIMEOUT_INFINITE    (INFINITE)

#define CAPABILITY_WINK    (0x01) // If set to 1, authenticator implements CTAPHID_WINK function
#define CAPABILITY_CBOR    (0x04) // If set to 1, authenticator implements CTAPHID_CBOR function 
#define CAPABILITY_NMSG    (0x08) // If set to 1, authenticator DOES NOT implement CTAPHID_MSG function

static bool gn_hidstatus = true;


static int fido_hid_devicereq(hid_device *handle, uint8_t cmd, const uint8_t *message, int length, uint8_t *response)
{
    int ret = 0;

    int leftcnt= 0;
    int rightcnt = 0;
    int pos = 0;
    int recvlen = 0;
    uint8_t reqbuf[HID_PACKET_MAX + 1] = {0, };
    uint8_t respbuf[HID_PACKET_MAX + 1] = {0, };

    int reqcnt = 1;
    int respcnt = 0;

    if (length < HID_BCNT_MAX) {
        ret = -1;
        fprintf(stderr, "invalid message length \n");
        goto endret;
    }


    memcpy(&requf[reqcnt], &cid, 4);
    reqcnt += 4;
    reqbuf[reqcnt++] = cmd;
    reqbuf[reqcnt++] = (length & 0xFF00) >> 8;
    reqbuf[reqcnt++] = length & 0xFF;

    if(message != NULL) {
        memcpy(&reqbuf[reqcnt++], message, ((length > HID_PACKET1_DATA_SIZE) ? HID_PACKET1_DATA_SIZE : length));
    }

    ret = hid_write(handle, reqbuf, HID_PACKET_MAX +1);
    if(ret <= 0) {
        fprintf(stderr, "hid write fail (%ls) \n",   hid_error(handle));
        goto endret;
    }

    pos = HID_PACKET1_DATA_SIZE;
    leftcnt = length - HID_PACKET1_DATA_SIZE;
    rightcnt = 0;

    while ( leftcnt > 0 ) {
        reqcnt = 5;
        reqbuf[reqcnt++] = rightcnt;
        memset(&reqbuf[reqcnt], &message[pos], ((leftcnt > HID_PACKET2_DATA_SIZE) ? HID_PACKET2_DATA_SIZE : left_cnt));
        ret = hid_write(handle, reqbuf, HID_PACKET_MAX +1);

        if(ret <= 0) {
            fprintf(stderr, "hid write fail (%ls) \n",   hid_error(handle));
            goto endret;
        }

        pos += HID_PACKET2_DATA_SIZE;
        leftcnt -= HID_PACKET2_DATA_SIZE;
        rightcnt++;
    }

    if( response == NULL) {
        fprintf(stdout, " Response don't existence \n");
        ret = 0;
        goto endret;
    }

    // receive
    do {
        respcnt = 0;
        ret = hid_read_timeout(handle, respbuf, HID_PACKET_MAX, -1);
        if(ret <= 0) {
            fprintf(stderr, "hid read fail (%ls) \n",   hid_error(handle));
            goto endret;
        }

        if(memcmp(&respbuf[respcnt], &cid, 4) != 0) {
            fprintf(stderr, "invalid cid \n");
            ret = -1;
            goto endret;
        }

        respcnt += 4;
    } while(respbuf[respcnt] == HID_CTAP_KEEPALIVE);

    if( respbuf[respcnt++] != cmd ) {
        fprintf(stderr, "invalid response command \n");
        ret = -1;
        goto endret;

    }

    recvlen = respbuf[respcnt++] << 8 |  respbuf[respcnt++];

    if((recvlen > HID_BCNT_MAX) || (recvlen == 0)) {
        ret = -1;
        goto endret;
    }

    memcpy(response, &resbuf[respcnt++], ((recvlen > HID_PACKET1_DATA_SIZE) ? HID_PACKET1_DATA_SIZE : recvlen));

    pos = HID_PACKET1_DATA_SIZE;
    leftcnt = recvlen - HID_PACKET1_DATA_SIZE;

    while( leftcnt > 0 ) {

        ret = hid_read_timeout(handle, respbuf, HID_PACKET_MAX, -1);
        if(ret < 0) {
            fprintf(stderr, "left response read fail (%ls) \n", hid_error(handle));
            goto endret;
        }

        memcpy(&response[pos], &respbuf[5], ((leftcnt > HID_PACKET2_DATA_SIZE) ? HID_PACKET2_DATA_SIZE : leftcnt));

        pos += HID_PACKET2_DATA_SIZE;
        leftcnt -= HID_PACKET2_DATA_SIZE;
    }


    return recvlen;

endret:

    return ret;
}


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




