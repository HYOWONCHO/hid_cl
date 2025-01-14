#ifndef __fido_hid_h
#define __fido_hid_h


#define     MAX_STR     255


typedef struct _fido_device_info_t {
    void *handle;
    uint32_t cid;
    uint8_t protocol_v;
    uint8_t major_v;
    uint8_t minor_v;
    uint8_t build_v;
    uint8_t capa_flags;
}fido_device_info_t;


typedef struct _fido_hid_info_t {
    void *handle;                       //< HID Handle
    fido_device_info_t *dev_info;
    wchar_t str_mf[MAX_STR];            //< Manfcture string
    wchar_t str_pt[MAX_STR];            //< Product string 
    wchar_t str_sn[MAX_STR];
    wchar_t str_is[MAX_STR];            //< Indexed String
}fido_hid_info_t;






#endif

