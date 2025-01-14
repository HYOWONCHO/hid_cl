#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


void dump(const char *title, const void* data, int size, bool offset)
{
    int i;
    uint8_t* buffer = NULL;

    if (( data == NULL ) || ( size == 0 ))
        return;

    buffer = (uint8_t*)data;

    (title != NULL) ? printf("Title : %s  ( Message Len : %d )\n", title, size) : printf("Title : None ( Message Len : %d )\n", size);

    if ( offset == true ) {
        fprintf(stdout, "    [  Offset  ] : 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F\n");
        fprintf(stdout, "    ----------------------------------------------------------------------------------------------");
        for ( i = 0; i < size; i++ ) {
            if ( i % 16 == 0 )
                fprintf(stdout, "\n    [0x%08X] : ", i);
            fprintf(stdout, "0x%02X ", buffer[i]);
        }
    } else {
        for ( i = 0; i < size; i++ ) {
            if (( i != 0 ) && ( i % 16 == 0 ))
                fprintf(stdout, "\n");
            fprintf(stdout, "%02X ", buffer[i]);
        }
    }
    fprintf(stdout, "\n");
}
