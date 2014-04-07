/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */

#include "ancs_base.h"
#include "ancs_notification.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*apply_cb)(ancs_notification_t* notif);

extern void ancs_notifications_use_hook(ancs_notification_t* notif);
extern void ancs_notifications_remove_hook(ancs_notification_t* notif);

void ancs_notification_source_parser(const uint8_t* buffer);
void ancs_get_notification_data(uint32_t uid);
void ancs_run();
void ancs_init();

#ifdef __cplusplus
}
#endif
