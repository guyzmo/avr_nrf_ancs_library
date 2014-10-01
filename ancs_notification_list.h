/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */
#ifndef _ANCS_NOTIFICATION_LIST_H_
#define _ANCS_NOTIFICATION_LIST_H_

#include "ancs_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ancs_notification.h"

typedef void (*apply_cb)(ancs_notification_t* notif);



void ancs_notification_list_init();
void ancs_notification_list_push(ancs_notification_t* notif);
ancs_notification_t* ancs_notification_list_get(uint32_t nid);
ancs_notification_t* ancs_notification_list_pull();
bool ancs_notification_list_remove();
ancs_notification_t* ancs_notification_list_pop();
void ancs_notification_list_apply(apply_cb cb);

#ifdef __cplusplus
}
#endif

#endif

