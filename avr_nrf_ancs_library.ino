/*
 * (c) 2013, Bernard PRATZ <bernard@pratz.net>. All Rights Reserved.
 *
 * Based on work that is under Copyright (c) 2009 Nordic Semiconductor.
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC SEMICONDUCTOR
 * STANDARD SOFTWARE LICENSE AGREEMENT. Licensees are granted free, non-transferable
 * use of the information. NO WARRANTY of ANY KIND is provided.
 *
 * This heading must NOT be removed from the file.
 *
 * $LastChangedRevision$
 */


/** @defgroup avr_nrf_ancs_library.ino
 @{
 @ingroup projects
 @brief This project is an example of use of the ANCS library
 
 @details
 
 */



//#define NO_ANCS
//#define DEBUG1
//#define DEBUG2


#include <SPI.h>
#include <lib_aci.h>
#include <ancs.h>
#include <pack_lib.h>
#include <aci_setup.h>
#include <EEPROM.h>

/**
 Put the nRF8001 setup in the RAM of the nRF8001.
 */
#include <utilities.h>
#include <ancs_services.h>
/**
 Include the services_lock.h to put the setup in the OTP memory of the nRF8001.
 This would mean that the setup cannot be changed once put in.
 However this removes the need to do the setup of the nRF8001 on every reset.
 */
#define DROP_NOTIF_SCHEDULE
#define DROP_NOTIF_EMAIL
#define DROP_NOTIF_NEWS
#define DROP_NOTIF_FITNESS
#define DROP_NOTIF_FINANCE
#define DROP_NOTIF_LOCATION
#define DROP_NOTIF_ENTERTAINMENT

#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
static services_pipe_type_mapping_t
services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
#define NUMBER_OF_PIPES 0
static services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif

/* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
static hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] PROGMEM = SETUP_MESSAGES_CONTENT;

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
static struct aci_state_t aci_state;

/*
 Temporary buffers for sending ACI commands
 */
static hal_aci_evt_t  aci_data;
static hal_aci_data_t aci_cmd;

/*
 We will store the bonding info for the nRF8001 in the EEPROM/Flash of the MCU to recover from a power loss situation
 */
static bool bonded_first_time = true;

/*
 Timing change state variable
 */
static bool timing_change_done = false;

void print_pipes(aci_evt_t* aci_evt) {
    Serial.print("Here are the available open pipes: ");
    for (uint8_t i=1; i<=NUMBER_OF_PIPES; ++i)
        if (lib_aci_is_pipe_available(&aci_state, i)) {
            Serial.print(i);
            Serial.print(", ");
        }
    Serial.println("");
    Serial.print(F("Here are the available closed pipes: "));
    for (uint8_t i=1; i<=NUMBER_OF_PIPES; ++i)
        if (lib_aci_is_pipe_closed(&aci_state, i)) {
            Serial.print(i);
            Serial.print(", ");
        }
    Serial.println("");
}

/* Define how assert should function in the BLE library */
void __ble_assert(const char *file, uint16_t line)
{
    Serial.print(F("ERROR "));
    Serial.print(file);
    Serial.print(F(": "));
    Serial.print(line);
    Serial.print(F("\n"));
    while(1);
}

/*************NOTE**********
 Scroll to the end of the file and read the loop() and setup() functions.
 The loop/setup functions is the equivalent of the main() function
 */

/*
 Read the Dymamic data from the EEPROM and send then as ACI Write Dynamic Data to the nRF8001
 This will restore the nRF8001 to the situation when the Dynamic Data was Read out
 */
aci_status_code_t bond_data_restore(aci_state_t *aci_stat, uint8_t eeprom_status, bool *bonded_first_time_state)
{
    aci_evt_t *aci_evt;
    uint8_t eeprom_offset_read = 1;
    uint8_t write_dyn_num_msgs = 0;
    uint8_t len =0;
    
    // Get the number of messages to write for the eeprom_status
    write_dyn_num_msgs = eeprom_status & 0x7F;
    
    //Read from the EEPROM
    while(1)
    {
        len = EEPROM.read(eeprom_offset_read);
        eeprom_offset_read++;
        aci_cmd.buffer[0] = len;
        
        for (uint8_t i=1; i<=len; i++)
        {
            aci_cmd.buffer[i] = EEPROM.read(eeprom_offset_read);
            eeprom_offset_read++;
        }
        //Send the ACI Write Dynamic Data
        if (!hal_aci_tl_send(&aci_cmd))
        {
            Serial.println(F("bond_data_restore: Cmd Q Full"));
            return ACI_STATUS_ERROR_INTERNAL;
        }
        
        //Spin in the while loop waiting for an event
        while (1)
        {
            if (lib_aci_event_get(aci_stat, &aci_data))
            {
                aci_evt = &aci_data.evt;
                
                if (ACI_EVT_CMD_RSP != aci_evt->evt_opcode)
                {
                    //Got something other than a command response evt -> Error
                    Serial.print(F("bond_data_restore: Expected cmd rsp evt. Got: 0x"));
                    Serial.println(aci_evt->evt_opcode, HEX);
                    return ACI_STATUS_ERROR_INTERNAL;
                }
                else
                {
                    write_dyn_num_msgs--;
                    
                    //ACI Evt Command Response
                    if (ACI_STATUS_TRANSACTION_COMPLETE == aci_evt->params.cmd_rsp.cmd_status)
                    {
                        //Set the state variables correctly
                        *bonded_first_time_state = false;
                        aci_stat->bonded = ACI_BOND_STATUS_SUCCESS;
                        
                        delay(10);
                        
                        return ACI_STATUS_TRANSACTION_COMPLETE;
                    }
                    if (0 >= write_dyn_num_msgs)
                    {
                        //should have returned earlier
                        return ACI_STATUS_ERROR_INTERNAL;
                    }
                    if (ACI_STATUS_TRANSACTION_CONTINUE == aci_evt->params.cmd_rsp.cmd_status)
                    {
                        //break and write the next ACI Write Dynamic Data
                        break;
                    }
                }
            }
        }
    }
}


/*
 This function is specific to the atmega328
 @params ACI Command Response Evt received from the Read Dynmaic Data
 */
void bond_data_store(aci_evt_t *evt)
{
    static int eeprom_write_offset = 1;
    
    //Write it to non-volatile storage
    EEPROM.write( eeprom_write_offset, evt->len -2 );
    eeprom_write_offset++;
    
    EEPROM.write( eeprom_write_offset, ACI_CMD_WRITE_DYNAMIC_DATA);
    eeprom_write_offset++;
    
    for (uint8_t i=0; i< (evt->len-3); i++)
    {
        EEPROM.write( eeprom_write_offset, evt->params.cmd_rsp.params.padding[i]);
        eeprom_write_offset++;
    }
}

bool bond_data_read_store(aci_state_t *aci_stat)
{
    /*
     The size of the dynamic data for a specific Bluetooth Low Energy configuration
     is present in the ublue_setup.gen.out.txt generated by the nRFgo studio as "dynamic data size".
     */
    bool status = false;
    aci_evt_t * aci_evt = NULL;
    uint8_t read_dyn_num_msgs = 0;
    
    //Start reading the dynamic data
    lib_aci_read_dynamic_data();
    read_dyn_num_msgs++;
    
    while (1)
    {
        if (true == lib_aci_event_get(aci_stat, &aci_data))
        {
            aci_evt = &aci_data.evt;
            
            if (ACI_EVT_CMD_RSP != aci_evt->evt_opcode )
            {
                //Got something other than a command response evt -> Error
                status = false;
                break;
            }
            
            if (ACI_STATUS_TRANSACTION_COMPLETE == aci_evt->params.cmd_rsp.cmd_status)
            {
                //Store the contents of the command response event in the EEPROM
                //(len, cmd, seq-no, data) : cmd ->Write Dynamic Data so it can be used directly
                bond_data_store(aci_evt);
                
                //Set the flag in the EEPROM that the contents of the EEPROM is valid
                EEPROM.write(0, 0x80|read_dyn_num_msgs );
                //Finished with reading the dynamic data
                status = true;
                
                break;
            }
            
            if (!(ACI_STATUS_TRANSACTION_CONTINUE == aci_evt->params.cmd_rsp.cmd_status))
            {
                //We failed the read dymanic data
                //Set the flag in the EEPROM that the contents of the EEPROM is invalid
                EEPROM.write(0, 0xFF);
                
                status = false;
                break;
            }
            else
            {
                //Store the contents of the command response event in the EEPROM
                // (len, cmd, seq-no, data) : cmd ->Write Dynamic Data so it can be used directly when re-storing the dynamic data
                bond_data_store(aci_evt);
                
                //Read the next dynamic data message
                lib_aci_read_dynamic_data();
                read_dyn_num_msgs++;
            }
        }
    }
    return status;
}


void aci_loop()
{
    static bool setup_required = false;
    
    // We enter the if statement only when there is a ACI event available to be processed
    if (lib_aci_event_get(&aci_state, &aci_data))
    {
        aci_evt_t * aci_evt;
        
        aci_evt = &aci_data.evt;
        switch(aci_evt->evt_opcode)
        {
                /**
                 As soon as you reset the nRF8001 you will get an ACI Device Started Event
                 */
            case ACI_EVT_DEVICE_STARTED:
            {
                aci_state.data_credit_total = aci_evt->params.device_started.credit_available;
                switch(aci_evt->params.device_started.device_mode)
                {
                    case ACI_DEVICE_SETUP:
                        /**
                         When the device is in the setup mode
                         */
                        debug_println(F("Evt Device Started: Setup"));
                        setup_required = true;
                        break;
                        
                    case ACI_DEVICE_STANDBY:
                        Serial.println(F("Evt Device Started: Standby"));
                        if (aci_evt->params.device_started.hw_error)
                        {
                            delay(20); //Magic number used to make sure the HW error event is handled correctly.
                        }
                        else
                        {
                            //Manage the bond in EEPROM of the AVR
                            {
                                uint8_t eeprom_status = 0;
                                eeprom_status = EEPROM.read(0);
                                if (eeprom_status != 0xFF)
                                {
                                    Serial.println(F("Previous Bond present. Restoring"));
                                    Serial.println(F("Using existing bond stored in EEPROM."));
                                    Serial.println(F("   To delete the bond stored in EEPROM, connect Pin 6 to 3.3v and Reset."));
                                    Serial.println(F("   Make sure that the bond on the phone/PC is deleted as well."));
                                    //We must have lost power and restarted and must restore the bonding infromation using the ACI Write Dynamic Data
                                    if (ACI_STATUS_TRANSACTION_COMPLETE == bond_data_restore(&aci_state, eeprom_status, &bonded_first_time))
                                    {
                                        Serial.println(F("Bond restored successfully"));
                                    }
                                    else
                                    {
                                        Serial.println(F("Bond restore failed. Delete the bond and try again."));
                                    }
                                }
                            }
                            
                            // Start bonding as all proximity devices need to be bonded to be usable
                            if (ACI_BOND_STATUS_SUCCESS != aci_state.bonded)
                            {
                                lib_aci_bond(180/* in seconds */, 0x0050 /* advertising interval 50ms*/);
                                Serial.println(F("No Bond present in EEPROM."));
                                Serial.println(F("Advertising started : Waiting to be connected and bonded"));
                            }
                            else
                            {
                                //connect to an already bonded device
                                //Use lib_aci_direct_connect for faster re-connections with PC, not recommended to use with iOS/OS X
                                lib_aci_connect(100/* in seconds */, 0x0020 /* advertising interval 20ms*/);
                                Serial.println(F("Already bonded : Advertising started : Waiting to be connected"));
                            }
                        }
                        break;
                }
            }
                break; //ACI Device Started Event
                
            case ACI_EVT_CMD_RSP:
                debug_print(F("Evt Command Response: "));
                //If an ACI command response event comes with an error -> stop
                switch (aci_evt->params.cmd_rsp.cmd_status) {
                    case ACI_STATUS_SUCCESS:
                        debug_println(F(": Success!"));
                        break;
                    case ACI_STATUS_ERROR_PIPE_STATE_INVALID:
                        debug_println(F(": failed with error 'pipe state invalid'"));
                        break;
                    case ACI_STATUS_ERROR_REJECTED:
                        debug_println(F(": failed with error 'command rejected'"));
                        break;
                    default:
                        debug_print(F(": Error "));
                        debug_println(aci_evt->params.cmd_rsp.cmd_status, HEX);
                }
                
                switch (aci_evt->params.cmd_rsp.cmd_opcode) {
                    case ACI_CMD_GET_DEVICE_ADDRESS:
                        debug_print(F("Device Address"));
                        //Store the version and configuration information of the nRF8001 in the Hardware Revision String Characteristic
                        lib_aci_set_local_data(&aci_state, PIPE_DEVICE_INFORMATION_HARDWARE_REVISION_STRING_SET,
                                               (uint8_t *)&(aci_evt->params.cmd_rsp.params.get_device_version), sizeof(aci_evt_cmd_rsp_params_get_device_version_t));
                        break;
                    case ACI_CMD_WAKEUP:             debug_println(F("Wake Up"       )); break;
                    case ACI_CMD_SLEEP:              debug_println(F("Sleep"         )); break;
                    case ACI_CMD_GET_DEVICE_VERSION: debug_println(F("Device Version")); break;
                    case ACI_CMD_GET_BATTERY_LEVEL:  debug_println(F("Battery Level" )); break;
                    case ACI_CMD_GET_TEMPERATURE:    debug_println(F("Temperature"   )); break;
                    case ACI_CMD_ECHO:               debug_println(F("Echo"          )); break;
                    case ACI_CMD_BOND:               debug_println(F("Bond"          )); break;
                    case ACI_CMD_CONNECT:            debug_println(F("Connect"       )); break;
                    case ACI_CMD_DISCONNECT:         debug_println(F("Disconnect"    )); break;
                    case ACI_CMD_CHANGE_TIMING:      debug_println(F("Change Timing" )); break;
                    case ACI_CMD_OPEN_REMOTE_PIPE:   debug_println(F("Open Remote Pipe" )); break;
                    default:
                        Serial.print(F("Evt Unk Cmd: "));
                        Serial.println(  aci_evt->params.cmd_rsp.cmd_opcode); //hex(aci_evt->params.cmd_rsp.cmd_opcode);
                }
                break;
                
            case ACI_EVT_CONNECTED:
                debug_println(F("Evt Connected"));
                aci_state.data_credit_available = aci_state.data_credit_total;
                timing_change_done = false;
                /*
                 Get the device version of the nRF8001 and store it in the Hardware Revision String
                 */
                lib_aci_device_version();
                break;
                
            case ACI_EVT_BOND_STATUS:
                debug_print(F("Evt Bond Status: "));
                debug_println(aci_evt->params.bond_status.status_code);
                aci_state.bonded = aci_evt->params.bond_status.status_code;
                
                break;
                
            case ACI_EVT_KEY_REQUEST:
                debug_println(F("Evt Key Request"));
                
                switch (aci_evt->params.key_request.key_type) {
                    case ACI_KEY_TYPE_INVALID:
                        debug_println(F("Key type is invalid"));
                        break;
                    case ACI_KEY_TYPE_PASSKEY:
                        debug_println(F("Key type is passkey"));
                        break;
                }
                
                break;
                
            case ACI_EVT_DISPLAY_PASSKEY:
                
                Serial.print(F("Evt Display Passkey: [ "));
                for (uint8_t i=0; i<6; ++i) {
                    Serial.print((char)aci_evt->params.display_passkey.passkey[i]);
                    Serial.print(" ");
                }
                Serial.println("]");
                
                break;
                
            case ACI_EVT_PIPE_STATUS:
                
                debug_println(F("Evt Pipe Status"));
                //Link is encrypted when the PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO is available
                /*if ((false == timing_change_done) &&
                 lib_aci_is_pipe_available(&aci_state, PIPE_BATTERY_BATTERY_LEVEL_TX))
                 {
                 lib_aci_change_timing_GAP_PPCP(); // change the timing on the link as specified in the nRFgo studio -> nRF8001 conf. -> GAP.
                 // Used to increase or decrease bandwidth
                 timing_change_done = true;
                 }*/
                // The pipe will be available only in an encrpyted link to the phone
                /*if ((ACI_BOND_STATUS_SUCCESS == aci_state.bonded) &&
                 (lib_aci_is_pipe_available(&aci_state, PIPE_BATTERY_BATTERY_LEVEL_TX))) {*/
                
                if (lib_aci_is_discovery_finished(&aci_state) && (ACI_BOND_STATUS_SUCCESS != aci_state.bonded)) {
                    debug_println(F("Upgrading security!"));
                    lib_aci_bond_request();
                }
                /*
                 if (ACI_BOND_STATUS_SUCCESS == aci_state.bonded)  {
                 lib_aci_open_remote_pipe(&aci_state, PIPE_ANCS_NOTIFICATION_SOURCE_RX_1);
                 }*/
                if (ACI_BOND_STATUS_SUCCESS == aci_state.bonded)  {
                    
                    //Note: This may be called multiple times after the Arduino has connected to the right phone
                    debug_println(F("phone Detected."));
                    
                    
                    // Detection of ANCS pipes
                    if (lib_aci_is_discovery_finished(&aci_state)) {
                        debug_println(F(" Service Discovery is over."));
                        // Test ANCS Pipes availability
                        if (!lib_aci_is_pipe_closed(&aci_state, PIPE_ANCS_CONTROL_POINT_TX_ACK)) {
                            debug_println(F("  -> ANCS Control Point not available."));
                        } else {
                            debug_println(F("  -> ANCS Control Point available."));
                            if (!lib_aci_open_remote_pipe(&aci_state, PIPE_ANCS_CONTROL_POINT_TX_ACK)){
                                debug_println(F("  -> ANCS Control Point Pipe: Failure opening."));
                            } else {
                                debug_println(F("  -> ANCS Control Point Pipe: Success opening."));
                            }
                        }
                        if (!lib_aci_is_pipe_closed(&aci_state, PIPE_ANCS_DATA_SOURCE_RX)) {
                            debug_println(F("  -> ANCS Data Source not available."));
                        } else {
                            debug_println(F("  -> ANCS Data Source available."));
                            if (!lib_aci_open_remote_pipe(&aci_state, PIPE_ANCS_DATA_SOURCE_RX)){
                                debug_println(F("  -> ANCS Data Source Pipe: Failure opening."));
                            } else {
                                debug_println(F("  -> ANCS Data Source Pipe: Success opening."));
                            }
                        }
                        if (!lib_aci_is_pipe_closed(&aci_state, PIPE_ANCS_NOTIFICATION_SOURCE_RX)) {
                            debug_println(F("  -> ANCS Notification Source not available."));
                        } else {
                            debug_println(F("  -> ANCS Notification Source available."));
                            if (!lib_aci_open_remote_pipe(&aci_state, PIPE_ANCS_NOTIFICATION_SOURCE_RX)) {
                                debug_println(F("  -> ANCS Notification Source Pipe: Failure opening."));
                            } else {
                                debug_println(F("  -> ANCS Notification Source Pipe: Success opening."));
                            }
                        }
                    } else {
                        debug_println(F(" Service Discovery is still going on."));
                    }
                    
                    
                }
                
                break;
                
            case ACI_EVT_TIMING:
                debug_println(F("Evt link connection interval changed"));
                //Disconnect as soon as we are bonded and required pipes are available
                //This is used to store the bonding info on disconnect and then re-connect to verify the bond
                /*
                 if((ACI_BOND_STATUS_SUCCESS == aci_state.bonded) &&
                 (true == bonded_first_time) &&
                 (GAP_PPCP_MAX_CONN_INT >= aci_state.connection_interval) &&
                 (GAP_PPCP_MIN_CONN_INT <= aci_state.connection_interval) && //Timing change already done: Provide time for the the peer to finish
                 (lib_aci_is_pipe_available(&aci_state, PIPE_BATTERY_BATTERY_LEVEL_TX))) {
                 lib_aci_disconnect(&aci_state, ACI_REASON_TERMINATE);
                 }*/
                break;
                
            case ACI_EVT_DISCONNECTED:
                debug_println(F("Evt Disconnected. Link Lost or Advertising timed out"));
                if (ACI_BOND_STATUS_SUCCESS == aci_state.bonded)
                {
                    if (ACI_STATUS_EXTENDED == aci_evt->params.disconnected.aci_status) //Link was disconnected
                    {
                        if (bonded_first_time)
                        {
                            bonded_first_time = false;
                            //Store away the dynamic data of the nRF8001 in the Flash or EEPROM of the MCU
                            // so we can restore the bond information of the nRF8001 in the event of power loss
                            if (bond_data_read_store(&aci_state))
                            {
                                debug_println(F("Dynamic Data read and stored successfully"));
                            }
                        }
                        if (0x24 == aci_evt->params.disconnected.btle_status)
                        {
                            //The error code appears when phone or Arduino has deleted the pairing/bonding information.
                            //The Arduino stores the bonding information in EEPROM, which is deleted only by
                            // the user action of connecting pin 6 to 3.3v and then followed by a reset.
                            //While deleting bonding information delete on the Arduino and on the phone.
                            debug_println(F("phone/Arduino has deleted the bonding/pairing information"));
                            debug_println(F("Pairing/Bonding info cleared from EEPROM."));
                            //Address. Value
                            EEPROM.write(0, 0xFF);
                            lib_aci_disconnect(&aci_state, ACI_REASON_TERMINATE);
                            delay(500);
                            lib_aci_radio_reset();
                        }
                        debug_print(F("Disconnected: "));
                        // btle_status == 13 when distant device removes bonding
                        debug_println((int)aci_evt->params.disconnected.btle_status, HEX);
                    }
                    if(ACI_STATUS_ERROR_BOND_REQUIRED == aci_evt->params.disconnected.aci_status) {
                        debug_println(F("phone has deleted the bonding/pairing information"));
                        //Clear the pairing
                        debug_println(F("Pairing/Bonding info cleared from EEPROM."));
                        //Address. Value
                        EEPROM.write(0, 0xFF);
                        lib_aci_disconnect(&aci_state, ACI_REASON_TERMINATE);
                        delay(500);
                        lib_aci_radio_reset();
                    } else {
                        
                        lib_aci_connect(180/* in seconds */, 0x0100 /* advertising interval 100ms*/);
                        debug_println(F("Using existing bond stored in EEPROM."));
                        debug_println(F("Advertising started. Connecting."));
                    }
                    free_ram();
                }
                else
                {
                    //There is no existing bond. Try to bond.
                    lib_aci_bond(180/* in seconds */, 0x0050 /* advertising interval 50ms*/);
                    debug_println(F("Advertising started. Bonding."));
                }
                break;
                
            case ACI_EVT_DATA_RECEIVED:
                debug_println(F("Evt Data Received"));
                switch (aci_evt->params.data_received.rx_data.pipe_number) {
                    case PIPE_ANCS_NOTIFICATION_SOURCE_RX:
#ifdef NO_ANCS
                        handle_notification(aci_evt->params.data_received.rx_data.aci_data);
#else
                        ancs_notification_source_parser(aci_evt->params.data_received.rx_data.aci_data);
#endif
                        break;
                    case PIPE_ANCS_DATA_SOURCE_RX:
#ifdef NO_ANCS
                        debug_println(F("DATA SOURCE"));
#else
                        ancs_data_source_parser(aci_evt->params.data_received.rx_data.aci_data);
#endif
                        break;
                    default:
                        debug_println(F("Evt Data received on Pipe #"));
                        debug_println(aci_evt->params.data_received.rx_data.pipe_number, DEC);
                        debug_println(F(" -> "));
                        debug_println(aci_evt->params.data_received.rx_data.aci_data[0], DEC);
                }
                /*Serial.print(F("Pipe #"));
                 Serial.print(aci_evt->params.data_received.rx_data.pipe_number, DEC);
                 Serial.print(F("-> "));
                 Serial.println(aci_evt->params.data_received.rx_data.aci_data[0], DEC);
                 link_loss_pipes_updated_evt_rcvd(aci_evt->params.data_received.rx_data.pipe_number,
                 &aci_evt->params.data_received.rx_data.aci_data[0]);*/
                break;
                
            case ACI_EVT_DATA_CREDIT:
                aci_state.data_credit_available = aci_state.data_credit_available + aci_evt->params.data_credit.credit;
                break;
            ACI_EVT_DATA_ACK:
                break;
                
            case ACI_EVT_PIPE_ERROR:
                //See the appendix in the nRF8001 Product Specication for details on the error codes
                debug_print(F("ACI Evt Pipe Error: Pipe #:"));
                debug_print(aci_evt->params.pipe_error.pipe_number, DEC);
                debug_print(F("  Pipe Error Code: 0x"));
                debug_println(aci_evt->params.pipe_error.error_code, HEX);
                
                //Increment the credit available as the data packet was not sent.
                //The pipe error also represents the Attribute protocol Error Response sent from the peer and that should not be counted
                //for the credit.
                if (ACI_STATUS_ERROR_PEER_ATT_ERROR != aci_evt->params.pipe_error.error_code)
                {
                    aci_state.data_credit_available++;
                }
                break;
                
            case ACI_EVT_HW_ERROR:
                debug_print(F("HW error: "));
                debug_println(aci_evt->params.hw_error.line_num, DEC);
                
                for(uint8_t counter = 0; counter <= (aci_evt->len - 3); counter++)
                {
                    Serial.write(aci_evt->params.hw_error.file_name[counter]); //uint8_t file_name[20];
                }
                debug_println();
                
                //Manage the bond in EEPROM of the AVR
            {
                uint8_t eeprom_status = 0;
                eeprom_status = EEPROM.read(0);
                if (eeprom_status != 0xFF)
                {
                    debug_println(F("Previous Bond present. Restoring"));
                    debug_println(F("Using existing bond stored in EEPROM."));
                    debug_println(F("   To delete the bond stored in EEPROM, connect Pin 6 to 3.3v and Reset."));
                    debug_println(F("   Make sure that the bond on the phone/PC is deleted as well."));
                    //We must have lost power and restarted and must restore the bonding infromation using the ACI Write Dynamic Data
                    if (ACI_STATUS_TRANSACTION_COMPLETE == bond_data_restore(&aci_state, eeprom_status, &bonded_first_time))
                    {
                        debug_println(F("Bond restored successfully"));
                    }
                    else
                    {
                        debug_println(F("Bond restore failed. Delete the bond and try again."));
                    }
                }
            }
                
                // Start bonding as all proximity devices need to be bonded to be usable
                if (ACI_BOND_STATUS_SUCCESS != aci_state.bonded)
                {
                    lib_aci_bond(180/* in seconds */, 0x0050 /* advertising interval 50ms*/);
                    debug_println(F("No Bond present in EEPROM."));
                    debug_println(F("Advertising started : Waiting to be connected and bonded"));
                }
                else
                {
                    //connect to an already bonded device
                    //Use lib_aci_direct_connect for faster re-connections with PC, not recommended to use with iOS/OS X
                    lib_aci_connect(100/* in seconds */, 0x0020 /* advertising interval 20ms*/);
                    debug_println(F("Already bonded : Advertising started : Waiting to be connected"));
                }
                break;
            default:
                debug_print("Unknown evt code: ");
                debug_println(aci_evt->evt_opcode);
                break;
        }
    }
    else
    {
        //Serial.println(F("No ACI Events available"));
        // No event in the ACI Event queue and if there is no event in the ACI command queue the arduino can go to sleep
        // Arduino can go to sleep now
        // Wakeup from sleep from the RDYN line
#ifndef NO_ANCS
        ancs_run();
#endif
    }
    
    /* setup_required is set to true when the device starts up and enters setup mode.
     * It indicates that do_aci_setup() should be called. The flag should be cleared if
     * do_aci_setup() returns ACI_STATUS_TRANSACTION_COMPLETE.
     */
    if(setup_required)
    {
        if (SETUP_SUCCESS == do_aci_setup(&aci_state))
        {
            setup_required = false;
        }
    }
}

#ifndef NO_ANCS
void ancs_notifications_remove_hook(ancs_notification_t* notif) {
    Serial.print(F("REMOVED - "));
    Serial.print(F("Notif #"));
    Serial.println( notif->uid );
}

void ancs_notifications_use_hook(ancs_notification_t* notif) {
    Serial.print (F("["));
    if ((notif->flags & ANCS_EVT_FLAG_SILENT) == ANCS_EVT_FLAG_SILENT)
        Serial.print(F("-"));
    else if ((notif->flags & ANCS_EVT_FLAG_IMPORTANT) == ANCS_EVT_FLAG_IMPORTANT)
        Serial.print(F("!"));
    else
        Serial.print(" ");
    Serial.print (F("] "));
    Serial.print(F("Notif #")); Serial.print( notif->uid); Serial.print( F(" ; from: '")); Serial.print( notif->app); Serial.println( F("'"));
    Serial.print(F("   category: "));
    switch (notif->category) {
        case ANCS_CATEGORY_INCOMING_CALL:
            Serial.println(F("incoming call"));
            break;
        case ANCS_CATEGORY_MISSED_CALL:
            Serial.println(F("missed call"));
            break;
        case ANCS_CATEGORY_VOICEMAIL:
            Serial.println(F("voicemail call"));
            break;
        case ANCS_CATEGORY_SOCIAL:
            Serial.println(F("social msg"));
            break;
        case ANCS_CATEGORY_OTHER:
            Serial.println(F("other"));
        default:
            Serial.println(F("ignored"));
            Serial.println(notif->category, DEC);
            break;
            //return;
    }
    Serial.print(F("   title:    '")); Serial.print( notif->title    ); Serial.println("'");
    Serial.print(F("   subtitle: '")); Serial.print( notif->subtitle ); Serial.println("'");
    Serial.print(F("   message:  '")); Serial.print( notif->message  ); Serial.println("'");
}
#else
void handle_notification(const unsigned char* data) {
    uint8_t event_id;
    uint8_t event_flags;
    uint8_t category_id;
    uint8_t category_count;
    uint32_t nid;
    
    debug_println(F("NOTIFICATION SOURCE"));
    unpack(data, "BBBBI", &event_id,
           &event_flags,
           &category_id,
           &category_count,
           &nid);
    
    Serial.print(F("Notif Id: "));
    Serial.print(nid);
    
    Serial.print(F(" ; Event type: "));
    switch (event_id) {
        case ANCS_EVT_NOTIFICATION_ADDED:
            Serial.print(F("ADDED"));
            Serial.print(F(" ; Event Category: "));
            switch (category_id) {
                case ANCS_CATEGORY_INCOMING_CALL:
                    Serial.println(F("incoming call"));
                    break;
                case ANCS_CATEGORY_MISSED_CALL:
                    Serial.println(F("missed call"));
                    break;
                case ANCS_CATEGORY_VOICEMAIL:
                    Serial.println(F("voicemail call"));
                    break;
                case ANCS_CATEGORY_SOCIAL:
                    Serial.println(F("social msg"));
                    break;
                case ANCS_CATEGORY_OTHER:
                    Serial.println(F("other"));
                default:
                    Serial.println(F("Ignored"));
                    return;
            }
            break;
        case ANCS_EVT_NOTIFICATION_REMOVED:
            Serial.println(F("REMOVED"));
            break;
    }
}

#endif

void setup(void)
{
    Serial.begin(115200);
    //Wait until the serial port is available (useful only for the Leonardo)
    //As the Leonardo board is not reseted every time you open the Serial Monitor
#if defined (__AVR_ATmega32U4__)
    while(!Serial)
    {}
#endif
    
    Serial.println(F("Arduino setup"));
    
    /**
     Point ACI data structures to the the setup data that the nRFgo studio generated for the nRF8001
     */
    if (NULL != services_pipe_type_mapping)
    {
        aci_state.aci_setup_info.services_pipe_type_mapping = &services_pipe_type_mapping[0];
    }
    else
    {
        aci_state.aci_setup_info.services_pipe_type_mapping = NULL;
    }
    aci_state.aci_setup_info.number_of_pipes    = NUMBER_OF_PIPES;
    aci_state.aci_setup_info.setup_msgs         = setup_msgs;
    aci_state.aci_setup_info.num_setup_msgs     = NB_SETUP_MESSAGES;
    
    //Tell the ACI library, the MCU to nRF8001 pin connections
    aci_state.aci_pins.board_name = BOARD_DEFAULT; //See board.h for details
    aci_state.aci_pins.reqn_pin   = 12;            //The REQN and RDYN jumpers are settable, make sure this is the same
    aci_state.aci_pins.rdyn_pin   = 11;
    aci_state.aci_pins.mosi_pin   = MOSI;
    aci_state.aci_pins.miso_pin   = MISO;
    aci_state.aci_pins.sck_pin    = SCK;
    
    aci_state.aci_pins.spi_clock_divider      = SPI_CLOCK_DIV8;//SPI_CLOCK_DIV8  = 2MHz SPI speed
    //SPI_CLOCK_DIV16 = 1MHz SPI speed
    
    aci_state.aci_pins.reset_pin              = UNUSED; //4 for Nordic board, UNUSED for REDBEARLABS
    aci_state.aci_pins.active_pin             = UNUSED;
    aci_state.aci_pins.optional_chip_sel_pin  = UNUSED;
    
    aci_state.aci_pins.interface_is_interrupt = false;
    aci_state.aci_pins.interrupt_number       = UNUSED;
    
    //We reset the nRF8001 here by toggling the RESET line connected to the nRF8001
    //and initialize the data structures required to setup the nRF8001
    //The second parameter is for turning debug printing on for the ACI Commands and Events so they be printed on the Serial
    lib_aci_init(&aci_state, false);
    aci_state.bonded = ACI_BOND_STATUS_FAILED;
    
    
    /*
     pinMode(6, INPUT); //Pin #6 on Arduino -> PAIRING CLEAR pin: Connect to 3.3v to clear the pairing
     if (0x01 == digitalRead(6))
     {
     //Clear the pairing
     Serial.println(F("Pairing/Bonding info cleared from EEPROM."));
     Serial.println(F("Remove the wire on Pin 6 and reset the board for normal operation."));
     //Address. Value
     EEPROM.write(0, 0xFF);
     
     }*/
#ifndef NO_ANCS
    ancs_init();
#endif
    free_ram();
}


void loop()
{
    aci_loop();
}


