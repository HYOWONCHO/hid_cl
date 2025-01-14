#ifndef __fido_hid_h
#define __fido_hid_h


#define     MAX_STR     255


typedef struct _fido_hid_info_t {
    void *handle;                       //< HID Handle
    wchar_t str_mf[MAX_STR];            //< Manfcture string
    wchar_t str_pt[MAX_STR];            //< Product string 
    wchar_t str_sn[MAX_STR];
    wchar_t str_is[MAX_STR];            //< Indexed String
}fido_hid_info_t;






#endif

