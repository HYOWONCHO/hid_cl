#include "whcbor_types.h"

void dump(const char *,const void*, int , bool);

static int whcbor_scp_map_fetch(int operand, void *decode, void *scpmap)
{
    int ret = WHCBOR_RET_FAIL;
    whcbor_scpmap_t *m = (whcbor_scpmap_t *)scpmap;

    if( m == NULL || decode == NULL) {
        fprintf(stderr, "Invalid Argument ( Located to NULL (%p, %p))!!! \n",
                decode, scpmap);
        goto err;
    }

     //           printf("%s:%d \n", __func__,  __LINE__);
    //operand = ((whcbor_map_t *)decode)->subcommand->v.uint;
     //           printf("%s:%d \n", __func__,  __LINE__);

    switch(operand) {
        case WHCBOR_SCP_SUBCMD_CONNECT:
            // TO DO : Obtain the Public key map buffer 
            printf(" SCP Connect operand !!! \n");
            m->pubkey = cn_cbor_mapget_string((const void *)decode, WHCBOR_SCP_PUBKEY_ID);
            if(m->pubkey == NULL) {
                fprintf(stderr, "Invalid Packet for CONNECT !!! \n");
                goto err;
            }

            dump("PUB Key for Connect", (const void *)m->pubkey->v.bytes, m->pubkey->length,1);
        case WHCBOR_SCP_SUBCMD_RECONNECT:
            m->nonce = cn_cbor_mapget_string((const void *)decode, WHCBOR_SCP_NONCE_ID);
            if(m->nonce== NULL) {
                fprintf(stderr, "Invalid Packet for NONCE !!! \n");
                goto err;
            }

            dump("Nonce for Connect", (const void *)m->nonce->v.bytes, m->nonce->length,1);
            break;
        default:
            fprintf(stderr, "Invalid Packet SUBCOMMAND for SCP operand !!! \n");
            goto err;
            //break;
    }
    
    ret = WHCBOR_RET_DONE;
err:

    return ret;
}

static int whcbor_cmd_map_fetch(void *decode, void *map)
{
    int ret = -1;

    whcbor_map_t *m = calloc(1, sizeof(whcbor_map_t));

    //printf("%s:%d \n", __func__, __LINE__);

    m->command = cn_cbor_mapget_int((const cn_cbor *)decode, WHCBOR_CMD_SCP);
    if( m->command == NULL ) {
        m->command = cn_cbor_mapget_int((const cn_cbor *)decode, WHCBOR_CMD_BIO);
    }

    //printf("%s:%d \n", __func__, __LINE__);

    switch(m->command->v.uint) {
        case WHCBOR_CMD_SCP:
            printf("WHCBOR CMD SCP \n");
            for (int x = 1; x <= WHCBOR_SCP_SUBCMD_RECONNECT; x++) {
                m->subcommand = cn_cbor_mapget_int((const cn_cbor *)decode,x);

                //printf("subcommand")
                (m->subcommand != NULL) ? printf("value: %ld \n", m->subcommand->v.uint) : printf("NULL\n");

                if( m->subcommand != NULL ) {
                   break;
                }
            } 

            if ( m->subcommand == NULL ) {
                goto err;
            }

            if (( ret = whcbor_scp_map_fetch(m->subcommand->v.uint,
                            decode, &m->scpmap)) != WHCBOR_RET_DONE ) {
                printf(" SCP MAP fetch fail ~~~ \n");
                goto err;
            }
#if 0
            m->scpmap->nonce = cn_cbor_mapget_string((const cn_cbor *)decode, 
                                                                    WHCBOR_SCP_PUBKEY_ID);

            if ( m->subcommand )

#endif

            break;
        case WHCBOR_CMD_BIO:
            printf("WHCBOR CMD BIO \n");
            for( int x = 1; x < WHCBOR_BIO_SUBCMD_IDENTIFY; x++ ) {
                m->subcommand = cn_cbor_mapget_int((const cn_cbor *)decode,x);

                if( m->subcommand != NULL )  {
                    break;
                }
            }
            break;
        default:
            break;
    }
    
    ret = 0L;

err:
    return ret;

}


int whcbor_cmd_dispatch(uint8_t *payload, uint32_t len)
{
    int ret = WHCBOR_RET_FAIL;

    uint8_t *_payload = payload;
    cn_cbor *decodemap = NULL;
    whcbor_map_t whmap;

    memset((void *)&whmap, 0x00, sizeof whmap);

    //dump("Decode buf", _payload, len, true);

    decodemap = cn_cbor_decode((const uint8_t *)_payload, len CBOR_CONTEXT_PARAM, NULL);

    if(decodemap == NULL) {
        printf("MAP decoding fail \n");
        goto err;
    }

    whcbor_cmd_map_fetch((void *)decodemap, (void *)&whmap);


    if(whmap.command == NULL || whmap.subcommand == NULL) {
        goto err;
    }


err:
    return ret;
}
