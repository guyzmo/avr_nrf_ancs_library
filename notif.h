//
//  nrf_ancs.h
//  
//
//  Created by Luke Berndt on 8/23/14.
//
//

#ifndef notif_h
#define notif_h

#include <Arduino.h>

#include <boards.h>
#include <lib_aci.h>
#include <aci_setup.h>
//#include "../BLE/lib_aci.h"

//#include "../BLE/aci_setup.h"

#include <avr/eeprom.h>




//#include <hal_platform.h>


#include "ancs.h"

//#include <hal_aci_tl.h>
//#include <hal_platform.h>


void eepromWrite(int address, uint8_t value);
uint8_t eepromRead(int address);
class Notif
{
public:
    Notif(uint8_t reqnPin, uint8_t rdynPin);


    void ReadNotifications();

    void setHandleNotification(void (*fptr)(ancs_notification_t* notif));
void setup();
    

private:
    void (*mHandleNotificationCallback)(ancs_notification_t* notif);
    aci_status_code_t bond_data_restore( uint8_t eeprom_status, bool *bonded_first_time_state);
    void bond_data_store(aci_evt_t *evt);
    bool bond_data_read_store();
    void DeviceStarted( aci_evt_t *aci_evt);
    void CommandResponse( aci_evt_t *aci_evt);
    void PipeStatus(aci_evt_t *aci_evt);
    void Disconnected( aci_evt_t *aci_evt);
    void HwError( aci_evt_t *aci_evt);
    
    int _pin;
    /*
     We will store the bonding info for the nRF8001 in the EEPROM/Flash of the MCU to recover from a power loss situation
     */
    bool bonded_first_time;
    
    /*
     Timing change state variable
     */
    bool timing_change_done;
    bool setup_required;
    
    // aci_struct that will contain
    // total initial credits
    // current credit
    // current state of the aci (setup/standby/active/sleep)
    // open remote pipe pending
    // close remote pipe pending
    // Current pipe available bitmap
    // Current pipe closed bitmap
    // Current connection interval, slave latency and link supervision timeout
    // Current State of the the GATT client (Service Discovery)
    // Status of the bond (R) Peer address
     //aci_state_t aci_state;
    
    /*
     Temporary buffers for sending ACI commands
     */
    
    hal_aci_data_t aci_cmd;
};


#endif
