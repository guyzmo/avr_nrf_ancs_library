/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */
//#define DEBUG1
#include "ancs_base.h"
#include "ancs_notification.h"
#include "ancs_notification_list.h"

#include "utilities.h"


// keep a buffer for each kind
static ancs_notification_t _notification_buffer[CACHE_SIZE];

static uint8_t _first_index = 0;
static uint8_t _last_index = 0;

void* _ancs_notification_list_alloc() {
    debug_println(F("[ANCS NL] _ancs_notification_list_alloc()"));
    return NULL;
}
void ancs_notification_list_init () {
    free_ram();
    return;
}

void ancs_notification_list_clear(uint8_t index) {
    _notification_buffer[index].uid                = 0;
    _notification_buffer[index].flags              = 0;
    _notification_buffer[index].category           = 0;
    _notification_buffer[index].action             = 0;
    _notification_buffer[index].msg_len            = 0;
#ifdef ANCS_USE_APP
    memset(_notification_buffer[index].app, 0, LINE_SIZE+1);
#endif
    memset(_notification_buffer[index].title, 0, LINE_SIZE+1);
#ifdef ANCS_USE_SUBTITLE
    memset(_notification_buffer[index].subtitle, 0, LINE_SIZE+1);
#endif
    memset(_notification_buffer[index].message, 0, LINE_SIZE+1);
}

void ancs_notification_list_push(ancs_notification_t* notif) {
    free_ram();
    
    ++_last_index;
    
    // if last index has reached CACHE_SIZE, make it cycle
    if (_last_index == CACHE_SIZE)
        _last_index = 0;
    // if last index has reached first index, step first index
    if (_last_index == _first_index)
    {
        ancs_notification_list_clear(_first_index);
        ++_first_index;
        if (_first_index == CACHE_SIZE)
            _first_index = 0;
    }
    ancs_notification_copy(&_notification_buffer[_last_index], notif);
    return;
}
ancs_notification_t* ancs_notification_list_get(uint32_t nid) {
    uint8_t idx;
    
    for (uint8_t i=0; i<CACHE_SIZE; ++i) {
        idx = (i + _first_index) % CACHE_SIZE;
        if (_notification_buffer[idx].uid == nid) {
            return &_notification_buffer[idx];
        }
        if (idx == _last_index) {
            break;
        }
    }
    return NULL;
}
ancs_notification_t* ancs_notification_list_pull() {
    
    if (_last_index == _first_index)
        return NULL;
    return &_notification_buffer[_last_index];
}
