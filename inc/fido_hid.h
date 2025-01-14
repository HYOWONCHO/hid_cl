#ifndef __fido_hid_h
#define __fido_hid_h


#define     MAX_STR     255


typedef struct _hid_ctlv_t {
    void *handle;
    uint32_t cid;   /* !< FIDO channel ID */
    uint16_t tag;  /* !< FIDO command */
    uint32_t length;
    uint8_t  message[1];
}__attribute__((packed)) hid_ctlv_t;


typedef struct _fido_device_info_t {
    void *handle;
    uint32_t cid;
    uint8_t protocol_v;
    uint8_t major_v;
    uint8_t minor_v;
    uint8_t build_v;
    uint8_t capa_flags;
}__attribute__((packed)) fido_device_info_t;


typedef struct _fido_hid_info_t {
    void *handle;                       //< HID Handle
    fido_device_info_t *dev_info;
    wchar_t str_mf[MAX_STR];            //< Manfcture string
    wchar_t str_pt[MAX_STR];            //< Product string 
    wchar_t str_sn[MAX_STR];
    wchar_t str_is[MAX_STR];            //< Indexed String
}__attribute__((packed)) fido_hid_info_t;






#endif

