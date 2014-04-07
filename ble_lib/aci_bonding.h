#ifndef _BONDING_H_
#define _BONDING_H_

#include <EEPROM.h>

#include "lib_aci.h"

aci_status_code_t bond_data_restore(aci_state_t *aci_stat, hal_aci_data_t* aci_cmd, hal_aci_evt_t* aci_data, uint8_t eeprom_status, bool *bonded_first_time_state);
void bond_data_store(aci_evt_t *evt);
bool bond_data_read_store(aci_state_t *aci_stat, hal_aci_data_t* aci_cmd, hal_aci_evt_t* aci_data);

#endif
