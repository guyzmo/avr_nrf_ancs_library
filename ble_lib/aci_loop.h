#ifndef _ACI_LOOP_H_
#define _ACI_LOOP_H_

#include <services.h>

extern void handle_idle_hook(aci_state_t*);
extern void handle_evt_pipe_status_hook(aci_state_t* aci_state, bool timing_change_done);
extern void handle_evt_data_received_hook(aci_evt_t* aci_evt, aci_state_t* aci_state);

void aci_setup(const uint8_t reqn,
               const uint8_t rdyn,
               const uint8_t mosi,
               const uint8_t miso,
               const uint8_t sck,
               const uint8_t rst,
               const uint8_t act,
               const uint8_t interrupt,
               const uint8_t board);
void aci_loop();

#endif
