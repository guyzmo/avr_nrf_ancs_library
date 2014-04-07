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

// define all the dropped notifications

#define DROP_NOTIF_SCHEDULE
#define DROP_NOTIF_EMAIL
#define DROP_NOTIF_NEWS
#define DROP_NOTIF_FITNESS
#define DROP_NOTIF_FINANCE
#define DROP_NOTIF_LOCATION
#define DROP_NOTIF_ENTERTAINMENT

// define the pinout of the board ; here's what's to be used for RBL shield

#define HAL_IO_RADIO_CSN       SS
#define HAL_IO_RADIO_ACTIVE    UNUSED
#define HAL_IO_RADIO_REQN      5
#define HAL_IO_RADIO_RDY       2
#define HAL_IO_RADIO_RDY_INT   1
#define HAL_IO_RADIO_SCK       SCK
#define HAL_IO_RADIO_MOSI      MOSI
#define HAL_IO_RADIO_MISO      MISO
#define HAL_IO_RADIO_RESET     UNUSED
#define HAL_IO_BOARD_TYPE      REDBEARLAB_SHIELD_V1_1

#include <SPI.h>

#include <lib_aci.h>
#include <aci_setup.h>
#include <EEPROM.h>

#include <ancs.h>

#include <utilities.h>
#include <pack_lib.h>

#include <avr/wdt.h>
#include <avr/pgmspace.h>

/** Put the nRF8001 setup in the RAM of the nRF8001. */
#include <services.h>

#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
static services_pipe_type_mapping_t
services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
#define NUMBER_OF_PIPES 0
static services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif

/* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
static const hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] PROGMEM = SETUP_MESSAGES_CONTENT;

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
struct aci_state_t aci_state;

/* Temporary buffers for sending ACI commands */
static hal_aci_evt_t  aci_data;
static hal_aci_data_t aci_cmd;

/* We will store the bonding info for the nRF8001 in the EEPROM/Flash 
 * of the MCU to recover from a power loss situation */
static bool bonded_first_time = true;

/* Timing change state variable */
static bool timing_change_done = false;

#ifdef DEBUG3
void print_pipes(aci_evt_t* aci_evt) {
    Serial << (F("Here are the available open pipes: "));
    for (uint8_t i=1; i<=NUMBER_OF_PIPES; ++i)
        if (lib_aci_is_pipe_available(&aci_state, i)) {
            Serial << (i);
            Serial << (F(", "));
        }
    Serial << endl;
    Serial << (F("Here are the available closed pipes: "));
    for (uint8_t i=1; i<=NUMBER_OF_PIPES; ++i)
        if (lib_aci_is_pipe_closed(&aci_state, i)) {
            Serial << (i);
            Serial << (F(", "));
        }
    Serial << ln();
}
#endif

void aci_loop() {
    // We enter the if statement only when there is a ACI event available to be processed
    if (lib_aci_event_get(&aci_state, &aci_data)) {
        debug_print(F("New evt received: "));
        aci_evt_t * aci_evt;

        aci_evt = &aci_data.evt;    
        switch(aci_evt->evt_opcode) {
            // As soon as you reset the nRF8001 you will get an ACI Device Started Event
            case ACI_EVT_DEVICE_STARTED:
                {
                    aci_state.data_credit_total = aci_evt->params.device_started.credit_available;
                    switch(aci_evt->params.device_started.device_mode) {
                        // When the device is in the setup mode
                        case ACI_DEVICE_SETUP:
                            debug_println(F("Evt Device Started: Setup"));
                            if (ACI_STATUS_TRANSACTION_COMPLETE != do_aci_setup(&aci_state))
                                debug_println(F("Error in ACI Setup"));
                            break;

                        case ACI_DEVICE_STANDBY:
                            {
                                debug_println(F("Evt Device Started: Standby"));
                                //Manage the bond in EEPROM of the AVR
                                {
                                    uint8_t eeprom_status = 0;
                                    eeprom_status = EEPROM.read(0);
                                    if (eeprom_status != 0x00 && eeprom_status != 0xFF) {
                                        debug_println(F("Previous Bond present. Restoring"));
                                        debug_println(F("Using existing bond stored in EEPROM."));
                                        //We must have lost power and restarted and must restore the bonding infromation using the ACI Write Dynamic Data
                                        if (ACI_STATUS_TRANSACTION_COMPLETE == bond_data_restore(&aci_state, &aci_cmd, &aci_data, eeprom_status, &bonded_first_time))
                                            debug_println(F("Bond restored successfully"));
                                        else
                                            debug_println(F("Bond restore failed. Delete the bond and try again."));
                                    }
                                }

                                // Start bonding as all proximity devices need to be bonded to be usable
                                if (ACI_BOND_STATUS_SUCCESS != aci_state.bonded) {
                                    lib_aci_bond(180/* in seconds */, 0x0050 /* advertising interval 50ms*/);
                                    debug_println(F("No Bond present in EEPROM."));
                                    debug_println(F("Advertising started : Waiting to be connected and bonded"));
                                } else {
                                    //connect to an already bonded device
                                    //Use lib_aci_direct_connect for faster re-connections with PC, not recommended to use with iOS/OS X
                                    lib_aci_connect(100/* in seconds */, 0x0020 /* advertising interval 20ms*/);
                                    debug_println(F("Already bonded : Advertising started : Waiting to be connected"));
                                }
                            }
                            break;
                    }
                }
                break; //ACI Device Started Event

            case ACI_EVT_CMD_RSP:
                debug_print(F("Evt Command Response: "));
                switch (aci_evt->params.cmd_rsp.cmd_opcode) {
                    case ACI_CMD_GET_DEVICE_ADDRESS: 
                        debug_print(F("Device Address"));
                        //Store the version and configuration information of the nRF8001 in the Hardware Revision String Characteristic
                        lib_aci_set_local_data(&aci_state, PIPE_DEVICE_INFORMATION_HARDWARE_REVISION_STRING_SET, 
                                (uint8_t *)&(aci_evt->params.cmd_rsp.params.get_device_version), sizeof(aci_evt_cmd_rsp_params_get_device_version_t));
                        break;
                    case ACI_CMD_WAKEUP:             debug_print(F("Wake Up"       )); break;
                    case ACI_CMD_SLEEP:              debug_print(F("Sleep"         )); break;
                    case ACI_CMD_GET_DEVICE_VERSION: debug_print(F("Device Version")); break;
                    case ACI_CMD_GET_BATTERY_LEVEL:  debug_print(F("Battery Level" )); break;
                    case ACI_CMD_GET_TEMPERATURE:    debug_print(F("Temperature"   )); break;
                    case ACI_CMD_ECHO:               debug_print(F("Echo"          )); break;
                    case ACI_CMD_BOND:               debug_print(F("Bond"          )); break;
                    case ACI_CMD_CONNECT:            debug_print(F("Connect"       )); break;
                    case ACI_CMD_DISCONNECT:         debug_print(F("Disconnect"    )); break;
                    case ACI_CMD_CHANGE_TIMING:      debug_print(F("Change Timing" )); break;
                    case ACI_CMD_OPEN_REMOTE_PIPE:   debug_print(F("Open Remote Pipe" )); break;
                    default:
                        Serial << F("Evt Unk Cmd ") << hex(aci_evt->params.cmd_rsp.cmd_opcode);
                }
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
                        Serial << F(": Error ") << hex(aci_evt->params.cmd_rsp.cmd_status) << endl;
                }
                break;

            case ACI_EVT_CONNECTED:
                debug_println(F("Evt Connected"));
                aci_state.data_credit_available = aci_state.data_credit_total;
                timing_change_done = false;        
                // Get the device version of the nRF8001 and store it in the Hardware Revision String
                lib_aci_device_version();
                break;

            case ACI_EVT_BOND_STATUS:
                debug_println(F("Evt Bond Status"));
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
                #if defined( DEBUG1 ) || defined( SHOW_KEY )
                Serial.print(F("Evt Display Passkey: [ "));
                for (uint8_t i=0; i<6; ++i) {
                    Serial.print((char)aci_evt->params.display_passkey.passkey[i]);
                    Serial.print(F(" "));
                }
                Serial.println(F("]"));
                #endif
                break;

            case ACI_EVT_TIMING:
                debug_println(F("Evt link connection interval changed"));
                //Disconnect as soon as we are bonded and required pipes are available
                //This is used to store the bonding info on disconnect and then re-connect to verify the bond
                if((ACI_BOND_STATUS_SUCCESS == aci_state.bonded) &&
                        (true == bonded_first_time) &&
                        (GAP_PPCP_MAX_CONN_INT >= aci_state.connection_interval) && 
                        (GAP_PPCP_MIN_CONN_INT <= aci_state.connection_interval) && //Timing change already done: Provide time for the the peer to finish
                        (lib_aci_is_pipe_available(&aci_state, PIPE_BATTERY_BATTERY_LEVEL_TX))) {
                    lib_aci_disconnect(&aci_state, ACI_REASON_TERMINATE);
                }
                break;

            case ACI_EVT_DISCONNECTED:
                debug_println(F("Evt Disconnected. Link Lost or Advertising timed out"));
                if (ACI_BOND_STATUS_SUCCESS == aci_state.bonded) {
                    //Link was disconnected
                    if (ACI_STATUS_EXTENDED == aci_evt->params.disconnected.aci_status) {
                        if (bonded_first_time) {
                            bonded_first_time = false;
                            //Store away the dynamic data of the nRF8001 in the Flash or EEPROM of the MCU 
                            // so we can restore the bond information of the nRF8001 in the event of power loss
                            if (bond_data_read_store(&aci_state, &aci_cmd, &aci_data)) {
                                debug_println(F("Dynamic Data read and stored successfully"));
                            }
                        }
                        if (0x24 == aci_evt->params.disconnected.btle_status) {
                            //The error code appears when phone or Arduino has deleted the pairing/bonding information.
                            //The Arduino stores the bonding information in EEPROM, which is deleted only by
                            // the user action of connecting pin 6 to 3.3v and then followed by a reset.
                            //While deleting bonding information delete on the Arduino and on the phone.
                            debug_println(F("phone has deleted the bonding/pairing information"));
                            //Clear the pairing
                            debug_println(F("Pairing/Bonding info cleared from EEPROM."));
                            //Address. Value
                            EEPROM.write(0, 0);
                            lib_aci_disconnect(&aci_state, ACI_REASON_TERMINATE);
                            delay(500);
                            lib_aci_radio_reset();
                        }

                        debug_print("Disconnected: ");
                        // btle_status == 13 when distant device removes bonding
                        debug_println((int)aci_evt->params.disconnected.btle_status, HEX);
                    } 
                    if(ACI_STATUS_ERROR_BOND_REQUIRED == aci_evt->params.disconnected.aci_status) {
                        debug_println(F("phone has deleted the bonding/pairing information"));
                        //Clear the pairing
                        debug_println(F("Pairing/Bonding info cleared from EEPROM."));
                        //Address. Value
                        EEPROM.write(0, 0);
                        lib_aci_disconnect(&aci_state, ACI_REASON_TERMINATE);
                        delay(500);
                        lib_aci_radio_reset();
                    } else {
                        lib_aci_connect(180/* in seconds */, 0x0100 /* advertising interval 100ms*/);
                        debug_println(F("Using existing bond stored in EEPROM."));
                        debug_println(F("Advertising started. Connecting."));
                    }
                    free_ram();
                } else {
                    //There is no existing bond. Try to bond.
                    lib_aci_bond(180/* in seconds */, 0x0050 /* advertising interval 50ms*/);
                    debug_println(F("Advertising started. Bonding."));
                }
                break;

            case ACI_EVT_PIPE_STATUS:
                debug_println(F("Evt Pipe Status"));
                //Link is encrypted when the PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO is available
                if ((false == timing_change_done) && 
                        lib_aci_is_pipe_available(&aci_state, PIPE_BATTERY_BATTERY_LEVEL_TX)) {
                    lib_aci_change_timing_GAP_PPCP(); // change the timing on the link as specified in the nRFgo studio -> nRF8001 conf. -> GAP. 
                    // Used to increase or decrease bandwidth
                    timing_change_done = true;
                }
                // The pipe will be available only in an encrpyted link to the phone
                if ((ACI_BOND_STATUS_SUCCESS == aci_state.bonded) && 
                        (lib_aci_is_pipe_available(&aci_state, PIPE_BATTERY_BATTERY_LEVEL_TX))) {
                    //Note: This may be called multiple times after the Arduino has connected to the right phone
                    debug2_println(F("phone detected."));

                    #ifdef DEBUG3
                    print_pipes(aci_evt);
                    #endif

                    // Detection of ANCS pipes
                    if (lib_aci_is_discovery_finished(&aci_state)) {
                        debug_println(F(" Service Discovery is over."));
                        // Test ANCS Pipes availability
                        if (!lib_aci_is_pipe_closed(&aci_state, PIPE_ANCS_CONTROL_POINT_TX_ACK))
                            debug3_println(F("  -> ANCS Control Point not available."));
                        else {
                            debug3_println(F("  -> ANCS Control Point available."));
                            if (!lib_aci_open_remote_pipe(&aci_state, PIPE_ANCS_CONTROL_POINT_TX_ACK))
                                debug3_println(F("  -> ANCS Control Point Pipe: Failure opening."));
                            else
                                debug3_println(F("  -> ANCS Control Point Pipe: Success opening."));
                        }
                        if (!lib_aci_is_pipe_closed(&aci_state, PIPE_ANCS_DATA_SOURCE_RX))
                            debug3_println(F("  -> ANCS Data Source not available."));
                        else {
                            debug3_println(F("  -> ANCS Data Source available."));
                            if (!lib_aci_open_remote_pipe(&aci_state, PIPE_ANCS_DATA_SOURCE_RX))
                                debug3_println(F("  -> ANCS Data Source Pipe: Failure opening."));
                            else {
                                debug3_println(F("  -> ANCS Data Source Pipe: Success opening."));
                            }
                        }
                        if (!lib_aci_is_pipe_closed(&aci_state, PIPE_ANCS_NOTIFICATION_SOURCE_RX)) {
                            debug3_println(F("  -> ANCS Notification Source not available."));
                        } else {
                            debug3_println(F("  -> ANCS Notification Source available."));
                            if (!lib_aci_open_remote_pipe(&aci_state, PIPE_ANCS_NOTIFICATION_SOURCE_RX)) {
                                debug3_println(F("  -> ANCS Notification Source Pipe: Failure opening."));
                            } else {
                                debug3_println(F("  -> ANCS Notification Source Pipe: Success opening."));
                            }
                        }
                    } else {
                        debug2_println(F(" Service Discovery is still going on."));
                    }

                }
                break;

            case ACI_EVT_INVALID:
                debug_println(F("Evt Invalid"));
                break;

            case ACI_EVT_ECHO:
                debug_println(F("Evt Echo"));
                break;

            case ACI_EVT_HW_ERROR:
                debug_println(F("Evt Hardware Error"));
                break;

            case ACI_EVT_DATA_ACK:
                debug_println(F("Evt Data Ack"));
                break;

            case ACI_EVT_DATA_RECEIVED:
                debug_println("Evt Data Received");
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
                        debug_println("DATA SOURCE");
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
                break;

            case ACI_EVT_DATA_CREDIT:
                debug_println(F("Evt Data credit"));
                aci_state.data_credit_available = aci_state.data_credit_available + aci_evt->params.data_credit.credit;
                break;

            case ACI_EVT_PIPE_ERROR:
                //See the appendix in the nRF8001 Product Specication for details on the error codes
                debug_print(F("ACI Evt Pipe Error: Pipe #:"));
                debug_print(aci_evt->params.pipe_error.pipe_number, DEC);
                switch (aci_evt->params.pipe_error.error_code) {
                    case ACI_STATUS_ERROR_BUSY:
                        debug_println(F("  Error Busy"));
                        break;
                    case ACI_STATUS_ERROR_PEER_ATT_ERROR:
                        debug_println(F("  Error reported by the peer"));
                        break;
                    default:
                        debug_print(F("  Pipe Error Code: 0x"));
                        debug_println(aci_evt->params.pipe_error.error_code, HEX);
                }

                //Increment the credit available as the data packet was not sent
                aci_state.data_credit_available++;
                break;


        }
    }
    else
    {
#ifndef NO_ANCS
        ancs_run();
#endif
        // No event in the ACI Event queue and if there is no event in the ACI command queue the arduino can go to sleep
        // Arduino can go to sleep now
        // Wakeup from sleep from the RDYN line

        // you can also run here your custom code, like getting actions from serial port...

        delay(500);
    }
}

#ifndef NO_ANCS
void ancs_notifications_remove_hook(ancs_notification_t* notif) {
    Serial << F("REMOVED ");
    Serial << "Notif #" << notif->uid << endl;
}

void ancs_notifications_use_hook(ancs_notification_t* notif) {
    Serial << (F("["));
    if ((notif->flags & ANCS_EVT_FLAG_SILENT) == ANCS_EVT_FLAG_SILENT)
        Serial << (F("-"));
    else if ((notif->flags & ANCS_EVT_FLAG_IMPORTANT) == ANCS_EVT_FLAG_IMPORTANT)
        Serial << (F("!"));
    else
        Serial << (F(" "));
    Serial << (F("] "));
    Serial << "Notif #" << notif->uid << " ; from: '" << notif->app << "'" << endl;
    Serial << "   category: ";
    switch (notif->category) {
        case ANCS_CATEGORY_INCOMING_CALL:
            Serial << "incoming call" << endl;
            break;
        case ANCS_CATEGORY_MISSED_CALL:
            Serial << "missed call" << endl;
            break;
        case ANCS_CATEGORY_VOICEMAIL:
            Serial << "voicemail call" << endl;
            break;
        case ANCS_CATEGORY_SOCIAL:
            Serial << "social msg" << endl;
            break;
        case ANCS_CATEGORY_OTHER:
            Serial << "other" << endl;
        default:
            Serial << "ignored" << endl;
            return;
    }
    Serial << "   title:    '" << notif->title    << "'" << endl;
    Serial << "   subtitle: '" << notif->subtitle << "'" << endl;
    Serial << "   message:  '" << notif->message  << "'" << endl;
}
#else
void handle_notification(const unsigned char* data) {
    uint8_t event_id;
    uint8_t event_flags;
    uint8_t category_id;
    uint8_t category_count;
    uint32_t nid;

    debug_println("NOTIFICATION SOURCE");
    unpack(data, "BBBBI", &event_id,
            &event_flags,
            &category_id,
            &category_count,
            &nid);

    Serial << F("Notif Id: ") << nid;

    Serial << F(" ; Event type: ");
    switch (event_id) {
        case ANCS_EVT_NOTIFICATION_ADDED:
            Serial << F("ADDED");
            Serial << F(" ; Event Category: ");
            switch (category_id) {
                case ANCS_CATEGORY_INCOMING_CALL:
                    Serial << "incoming call" << endl;
                    break;
                case ANCS_CATEGORY_MISSED_CALL:
                    Serial << "missed call" << endl;
                    break;
                case ANCS_CATEGORY_VOICEMAIL:
                    Serial << "voicemail call" << endl;
                    break;
                case ANCS_CATEGORY_SOCIAL:
                    Serial << "social msg" << endl;
                    break;
                case ANCS_CATEGORY_OTHER:
                    Serial << "other" << endl;
                default:
                    Serial << "Ignored" << endl;
                    return;
            }
            break;
        case ANCS_EVT_NOTIFICATION_REMOVED:
            Serial << F("REMOVED") << endl;
            break;
    }
}

#endif

void aci_setup(const uint8_t reqn, 
               const uint8_t rdyn,
               const uint8_t mosi,
               const uint8_t miso,
               const uint8_t sck,
               const uint8_t rst,
               const uint8_t act,
               const uint8_t interrupt,
               const uint8_t board)
{
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
    aci_state.aci_setup_info.setup_msgs         = (hal_aci_data_t*)setup_msgs;
    aci_state.aci_setup_info.num_setup_msgs     = NB_SETUP_MESSAGES;

    //Tell the ACI library, the MCU to nRF8001 pin connections
    aci_state.aci_pins.board_name = board; //See board.h for details
    aci_state.aci_pins.reqn_pin   = reqn;
    aci_state.aci_pins.rdyn_pin   = rdyn;
    aci_state.aci_pins.mosi_pin   = mosi;
    aci_state.aci_pins.miso_pin   = miso;
    aci_state.aci_pins.sck_pin    = sck;

    aci_state.aci_pins.spi_clock_divider     = SPI_CLOCK_DIV8;

    aci_state.aci_pins.reset_pin             = rst;
    aci_state.aci_pins.active_pin            = act;
    aci_state.aci_pins.optional_chip_sel_pin = UNUSED;

    aci_state.aci_pins.interface_is_interrupt     = false;
    aci_state.aci_pins.interrupt_number                   = interrupt;

    //We reset the nRF8001 here by toggling the RESET line connected to the nRF8001
    //and initialize the data structures required to setup the nRF8001
    lib_aci_init(&aci_state);
    aci_state.bonded = ACI_BOND_STATUS_FAILED;
}


/*
Description:

<Add description of the proximity application.

The ACI Evt Data Credit provides the radio level ack of a transmitted packet.
 */
/* uint8_t SPCR_DF; */
/* uint8_t SPCR_BLE; */

void setup(void)
{ 
    // reset any timer before booting!
    MCUSR=0;
    wdt_disable();

    Serial.begin((uint16_t)115200);

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
    aci_state.aci_setup_info.setup_msgs         = (hal_aci_data_t*)setup_msgs;
    aci_state.aci_setup_info.num_setup_msgs     = NB_SETUP_MESSAGES;

    aci_setup(HAL_IO_RADIO_REQN,
              HAL_IO_RADIO_RDY,
              HAL_IO_RADIO_MOSI,
              HAL_IO_RADIO_MISO,
              HAL_IO_RADIO_SCK,
              HAL_IO_RADIO_RESET,
              HAL_IO_RADIO_ACTIVE,
              HAL_IO_RADIO_RDY_INT,
              HAL_IO_BOARD_TYPE);

    //We reset the nRF8001 here by toggling the RESET line connected to the nRF8001
    //and initialize the data structures required to setup the nRF8001
    Serial << (F("nRF8001 setup: "));

    pinMode(SS, OUTPUT);

    lib_aci_init(&aci_state);
    /* SPCR_BLE = SPCR; */
    #ifdef DEBUG4
    lib_aci_debug_print(true);
    #else
    lib_aci_debug_print(false);
    #endif
    aci_state.bonded = ACI_BOND_STATUS_FAILED;

    Serial << (F("done")) << endl;

#ifndef NO_ANCS
    Serial << (F("ANCS setup: "));
    ancs_init();
    Serial << ("done") << endl;
#endif
}

void loop()
{
    aci_loop();
}

