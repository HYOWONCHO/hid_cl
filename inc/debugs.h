#ifndef __debugs_h
#define __debugs_h

#include <stdio.h>
#include <string.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __DBG__
#           define debug_printf(fmt,...)       fprintf(stdout, "[DBG %s:%d]: "fmt"\n", __FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#           define debug_printf(fmt,...)
#endif

#define err_printf(fmt,...) fprintf(stderr, "[ERR %s:%d]"fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)
#define info_printf(fmt,...) fprintf(stderr, "[INFO %s:%d]"fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__)


#define ERR_JUMP_VAL(retval)   do { ret = retval; goto endret; } while(0)
 

#ifdef __cplusplus
}
#endif



#endif


