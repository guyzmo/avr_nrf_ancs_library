/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */
#ifndef _ANCS_H_
#define _ANCS_H_

#include "ancs_notification.h"
#include "ancs_notification_list.h"
#include "ancs_notification_source.h"
#include "ancs_data_source.h"

//  #define NO_CACHE          // disables caching
//  #define ANCS_DATA_SIZE 10 // sets the length of the attributes to be grabbed

// in setup():
//  ancs_init();              // initializes the ancs library
// in aci_loop():
//  ancs_run();               // executes ancs routines
// in ACI_EVT_DATA_RECEIVED:
//  ancs_notification_source_parser() // to be called with the data incoming for the notification source pipe
//  ancs_data_source_parser() // to be called with the data incoming for the data source pipe

#endif
