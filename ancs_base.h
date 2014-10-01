/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */
#ifndef _ANCS_BASE_H_
#define _ANCS_BASE_H_

#include <Arduino.h>
#include <aci.h>

// user definable variable
#define ANCS_NOTIFICATION_ATTRIBUTE_DATA_SIZE 16
#define TITLE_LEN 16
#define LINE_SIZE 16
#define MESSAGE_SIZE 40


#define CACHE_SIZE 5

////////////////////////////////////////////////////////////////////////////////////
#define ANCS_EVT_NOTIFICATION_ADDED     0x0
#define ANCS_EVT_NOTIFICATION_MODIFIED  0x1
#define ANCS_EVT_NOTIFICATION_REMOVED   0x2
// reserved ids up to 0xFF

#define ANCS_EVT_FLAG_SILENT            0x1
#define ANCS_EVT_FLAG_IMPORTANT         0x2
// reserved flags 0x4, 0x8, 0x10, 0x20, 0x40 and 0x80

#define ANCS_CATEGORY_OTHER             0x0
#define ANCS_CATEGORY_INCOMING_CALL     0x1
#define ANCS_CATEGORY_MISSED_CALL       0x2
#define ANCS_CATEGORY_VOICEMAIL         0x3
#define ANCS_CATEGORY_SOCIAL            0x4
#define ANCS_CATEGORY_SCHEDULE          0x5
#define ANCS_CATEGORY_EMAIL             0x6
#define ANCS_CATEGORY_NEWS              0x7
#define ANCS_CATEGORY_HEALTH_FITNESS    0x8
#define ANCS_CATEGORY_BUSINESS_FINANCE  0x9
#define ANCS_CATEGORY_LOCATION          0xA
#define ANCS_CATEGORY_ENTERTAINMENT     0xB
// reserved ids up to 0xFF

#define ANCS_COMMAND_GET_NOTIF_ATTRIBUTES 0x0
#define ANCS_COMMAND_GET_APP_ATTRIBUTES   0x1
// reserved ids up to 0xFF

#define ANCS_NOTIFICATION_ATTRIBUTE_APP_IDENTIFIER 0x0
#define ANCS_NOTIFICATION_ATTRIBUTE_TITLE          0x1 // shall be followed by 2bytes max length
#define ANCS_NOTIFICATION_ATTRIBUTE_SUBTITLE       0x2 // shall be followed by 2bytes max length
#define ANCS_NOTIFICATION_ATTRIBUTE_MESSAGE        0x3 // shall be followed by 2bytes max length
#define ANCS_NOTIFICATION_ATTRIBUTE_MESSAGE_SIZE   0x4
#define ANCS_NOTIFICATION_ATTRIBUTE_DATE           0x5
// reserved ids up to 0xFF

#define ANCS_NOTIFICATION_APP_ID_DISPLAY_NAME       0x0
// reserved ids up to 0xFF

// Datasource definitions
#define ANCS_DATA_LEN 20//ACI_PIPE_RX_DATA_MAX_LEN-2
#define ANCS_HEADER_LEN 5
#define ANCS_ATTR_REQ_LEN 3
#define ANCS_FIRST_DATA_LEN 12

#endif
