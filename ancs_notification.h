/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */
#ifndef _ANCS_NOTIFICATION_H_
#define _ANCS_NOTIFICATION_H_

#include <stdlib.h>
#include <inttypes.h>

#include "ancs_base.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct date_t {
    uint8_t Y;
    uint8_t M;
    uint8_t D;
    uint8_t h;
    uint8_t m;
} date_t;

typedef struct ancs_notification_t {
    uint32_t uid;
    uint8_t  action;
    uint8_t  flags;
    uint8_t  category;
    uint16_t msg_len;
    date_t   time;
    char     title[LINE_SIZE+1];
    #ifdef ANCS_USE_SUBTITLE
    char     subtitle[LINE_SIZE+1];
#endif
    #ifdef ANCS_USE_APP
    char     app[LINE_SIZE+1];
#endif
    char     message[MESSAGE_SIZE+1];
} ancs_notification_t;

void ancs_notification_init(ancs_notification_t* n);
void ancs_notification_copy(ancs_notification_t* dst, ancs_notification_t* src);

ancs_notification_t* ancs_notification_cached();

#ifdef __cplusplus
}
#endif

#endif
