
#include <inttypes.h>
#define DEBUG1
#include <ancs_services.h>
#include "lib_aci.h"

#define PACK_LITTLE_ENDIAN
#include "data_lib/pack_lib.h"
#include "data_lib/utilities.h"
#include "data_lib/linked_list.h"

#include "ancs_base.h"
#include "ancs_notification_source.h"
#include "ancs_notification_list.h"

boolean command_send_enable = true;

static linked_list_t* buffer_commands;
static bool ancs_updated = true;

bool ancs_send_buffered_command() {
    if (!command_send_enable) {
        return true;
    }
    uint8_t* buffer;
    size_t len = fifo_pop(buffer_commands, &buffer);
    if (len == 0) {
        return false;
    }
    uint8_t cmd, aid;
    uint32_t uid;
    unpack(buffer, "BIB", &cmd, &uid, &aid);
    debug_print(F("[ANCS CP] Command sent 0x"));
    debug_print(cmd, HEX);
    debug_print(F(" for notification #"));
    debug_print(uid, DEC);
    debug_print(F(" attribute 0x"));
    debug_print(aid, HEX);
    debug_println();
    command_send_enable = false;
    lib_aci_send_data(PIPE_ANCS_CONTROL_POINT_TX_ACK, buffer, len);
    free(buffer);
    return true;
}

void ancs_run() {
    if (ancs_send_buffered_command()) {
        
        ancs_updated = false;
    } else if (!ancs_updated) {
        ancs_updated = true;
        ancs_notification_list_apply(&ancs_notifications_use_hook);
        free_ram();
    }
}

void ancs_init() {
    buffer_commands = fifo_create();
    ancs_notification_list_init();
}

void ancs_get_notification_data(uint32_t uid) {
     if (command_send_enable) {
    Serial.print(F("[ANCS NS] ancs_get_notification_data("));
    Serial.print(uid, DEC);
    Serial.println(F(")"));
    debug2_print(F("[ANCS NS] Buffering commands to get details for notification #"));
    debug2_println(uid, DEC);
    

    
    uint8_t* buffer;
    buffer = (uint8_t*)malloc(6);
    // 
    pack(buffer, "BIB", ANCS_COMMAND_GET_NOTIF_ATTRIBUTES, uid,
                        ANCS_NOTIFICATION_ATTRIBUTE_APP_IDENTIFIER);
    
    fifo_push(buffer_commands, buffer, 6);
    free(buffer);
    //
    buffer = (uint8_t*)malloc(8);
    pack(buffer, "BIBH", ANCS_COMMAND_GET_NOTIF_ATTRIBUTES, uid,
                         ANCS_NOTIFICATION_ATTRIBUTE_TITLE,
                         ANCS_NOTIFICATION_ATTRIBUTE_DATA_SIZE);
    fifo_push(buffer_commands, buffer, 8);
    free(buffer);
    //
    buffer = (uint8_t*)malloc(6);
    pack(buffer, "BIB", ANCS_COMMAND_GET_NOTIF_ATTRIBUTES, uid,
                        ANCS_NOTIFICATION_ATTRIBUTE_DATE);
    fifo_push(buffer_commands, buffer, 6);
    free(buffer);
    buffer = (uint8_t*)malloc(6);
    pack(buffer, "BIB", ANCS_COMMAND_GET_NOTIF_ATTRIBUTES, uid,
                        ANCS_NOTIFICATION_ATTRIBUTE_MESSAGE_SIZE);
    fifo_push(buffer_commands, buffer, 6);
    free(buffer);
    //
    buffer = (uint8_t*)malloc(8);
    pack(buffer, "BIBB", ANCS_COMMAND_GET_NOTIF_ATTRIBUTES, uid,
                         ANCS_NOTIFICATION_ATTRIBUTE_SUBTITLE,
                         ANCS_NOTIFICATION_ATTRIBUTE_DATA_SIZE);
    fifo_push(buffer_commands, buffer, 8);
    free(buffer);
    //
    buffer = (uint8_t*)malloc(8);
    pack(buffer, "BIBH", ANCS_COMMAND_GET_NOTIF_ATTRIBUTES, uid,
                         ANCS_NOTIFICATION_ATTRIBUTE_MESSAGE,
                         ANCS_NOTIFICATION_ATTRIBUTE_DATA_SIZE);
    fifo_push(buffer_commands, buffer, 8);
    free(buffer);
    free_ram();
    debug_println(F("[ANCS NS]	ancs_get_notification_data(): end"));
    debug_print(F("[ANCS NS]	Command Send Enable: "));
    debug_println(command_send_enable);
     } else {
         Serial.print(F("[ANCS NS] ancs_get_notification_data("));
         Serial.print(uid, DEC);
         Serial.println(F(")"));
     }
    
}

void ancs_notification_source_parser(const uint8_t* buffer) {
    Serial.println(F("[ANCS NS] ancs_notification_source_parser()"));
    uint8_t event_id;
    uint8_t event_flags;
    uint8_t category_id;
    uint8_t category_count;
    uint32_t nid;

    ancs_notification_t* notif = ancs_notification_cached();
    
    unpack(buffer, "BBBBI", &event_id,
                            &event_flags,
                            &category_id,
                            &category_count,
                            &nid);

    Serial.print(F("N #"));
    Serial.print(nid, DEC);
    Serial.print(F(": "));

    notif->uid = nid;
    notif->flags = event_flags;
    notif->category = category_id;
    notif->action = event_id;

    if (event_flags != 0) {
        debug_println(F("FLAGS: "));
        if ((event_flags & ANCS_EVT_FLAG_SILENT) == ANCS_EVT_FLAG_SILENT)
            debug_println(F("SILENT "));
        if ((event_flags & ANCS_EVT_FLAG_IMPORTANT) == ANCS_EVT_FLAG_IMPORTANT)
            debug_println(F("IMPORTANT "));
        debug_println(F("; "));
    } else
        debug_println(F("NO FLAGS; "));

    Serial.println(F("Category: "));
    switch (category_id) {
        case ANCS_CATEGORY_OTHER:
            Serial.println(F("Other"));
#ifndef DROP_NOTIF_OTHER
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_INCOMING_CALL:
            Serial.println(F("Incoming call"));
#ifndef DROP_NOTIF_INCOMING_CALL
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_MISSED_CALL:
            Serial.println(F("Missed call"));
#ifndef DROP_NOTIF_MISSED_CALL
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_VOICEMAIL:
            Serial.println(F("Voicemail"));
#ifndef DROP_NOTIF_VOICEMAIL_CALL
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_SOCIAL:
            Serial.println(F("Social"));
#ifndef DROP_NOTIF_SOCIAL
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_SCHEDULE:
            debug_print(F("Schedule"));
#ifndef DROP_NOTIF_SCHEDULE
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_EMAIL:
            debug_print(F("Email"));
#ifndef DROP_NOTIF_EMAIL
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_NEWS:
            debug_print(F("News"));
#ifndef DROP_NOTIF_NEWS
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_HEALTH_FITNESS:
            debug_print(F("Health & Fitness"));
#ifndef DROP_NOTIF_HEALTH_FITNESS
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_BUSINESS_FINANCE:
            debug_print(F("Business & Finance"));
#ifndef DROP_NOTIF_BUSINESS_FINANCE
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_LOCATION:
            debug_print(F("Location"));
#ifndef DROP_NOTIF_CATEGORY_LOCATION
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        case ANCS_CATEGORY_ENTERTAINMENT:
            debug_print(F("Entertainment"));
#ifndef DROP_NOTIF_ENTERTAINMENT
            break;
#else
            debug_println(F(": ignored"));
            return;
#endif
        default: return;
    }
    debug_print(F("["));
    debug_print(category_count);
    debug_print(F("] -> "));
    switch (event_id) {
        case ANCS_EVT_NOTIFICATION_ADDED:
            debug_println(F("ADDED"));
            if (ancs_notification_list_get(nid) == NULL) {
                ancs_get_notification_data(nid);
            } else {
                debug_print(F("Notification already in cache: "));
                debug_println(nid);
            }
            break;
        case ANCS_EVT_NOTIFICATION_MODIFIED:
            debug_println(F("MODIFIED"));
            ancs_get_notification_data(nid);
            break;
        case ANCS_EVT_NOTIFICATION_REMOVED:
            debug_println(F("REMOVED"));
            ancs_notifications_remove_hook(notif);
            break;
    }
}

