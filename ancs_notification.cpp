/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */

#include "ancs_notification.h"
#include <string.h>
#include "utilities.h"

void ancs_notification_init(ancs_notification_t* n) {
    debug_println(F("ancs_notification_init()"));
    n->uid                = 0xFF;
}

void ancs_notification_copy(ancs_notification_t* dst,
                            ancs_notification_t* src) {
    debug_println(F("ancs_notification_copy()"));
    dst->uid                = src->uid;
    dst->flags              = src->flags;
    dst->category           = src->category;
    dst->action             = src->action;
    dst->msg_len            = src->msg_len;
    #ifdef ANCS_USE_APP
    strncpy(dst->app,    src->app,    LINE_SIZE+1);
    #endif
    strncpy(dst->title,    src->title,    LINE_SIZE+1);
    #ifdef ANCS_USE_SUBTITLE
    strncpy(dst->subtitle,    src->subtitle,    LINE_SIZE+1);
    #endif
    strncpy(dst->message,    src->message,    LINE_SIZE+1);
}


ancs_notification_t* ancs_notification_cached() {
    static ancs_notification_t notification_cache;
    debug_print(F("ancs_notification_cached("));
    debug_print(notification_cache.uid);
    debug_println(F(")"));
    return &notification_cache;
}

