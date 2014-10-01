/* 
* Copyright (c) 2013, Nordic Semiconductor ASA
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
* 
* - Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
* 
* - Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
* 
* - The name of Nordic Semiconductor ASA may not be used to endorse or promote
*   products derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
* This file is autogenerated by nRFgo Studio 1.16.1.3119 
*/

#ifndef SETUP_MESSAGES_H__
#define SETUP_MESSAGES_H__

#include "hal_platform.h" 
#include "aci.h"


#define SETUP_ID 1
#define SETUP_FORMAT 3 /** nRF8001 D */
#define ACI_DYNAMIC_DATA_SIZE 143

/* Service: GATT - Characteristic: Service Changed - Pipe: TX_ACK */
#define PIPE_GATT_SERVICE_CHANGED_TX_ACK          1
#define PIPE_GATT_SERVICE_CHANGED_TX_ACK_MAX_SIZE 4

/* Service: ANCS - Characteristic: Notification Source - Pipe: RX */
#define PIPE_ANCS_NOTIFICATION_SOURCE_RX          2
#define PIPE_ANCS_NOTIFICATION_SOURCE_RX_MAX_SIZE 1

/* Service: ANCS - Characteristic: Data Source - Pipe: RX */
#define PIPE_ANCS_DATA_SOURCE_RX          3
#define PIPE_ANCS_DATA_SOURCE_RX_MAX_SIZE 60

/* Service: ANCS - Characteristic: Control Point - Pipe: TX_ACK */
#define PIPE_ANCS_CONTROL_POINT_TX_ACK          4
#define PIPE_ANCS_CONTROL_POINT_TX_ACK_MAX_SIZE 1


#define NUMBER_OF_PIPES 4

#define SERVICES_PIPE_TYPE_MAPPING_CONTENT {\
  {ACI_STORE_LOCAL, ACI_TX_ACK},   \
  {ACI_STORE_REMOTE, ACI_RX},   \
  {ACI_STORE_REMOTE, ACI_RX},   \
  {ACI_STORE_REMOTE, ACI_TX_ACK},   \
}

#define GAP_PPCP_MAX_CONN_INT 0x320 /**< Maximum connection interval as a multiple of 1.25 msec , 0xFFFF means no specific value requested */
#define GAP_PPCP_MIN_CONN_INT  0x190 /**< Minimum connection interval as a multiple of 1.25 msec , 0xFFFF means no specific value requested */
#define GAP_PPCP_SLAVE_LATENCY 0
#define GAP_PPCP_CONN_TIMEOUT 0x258 /** Connection Supervision timeout multiplier as a multiple of 10msec, 0xFFFF means no specific value requested */

#define NB_SETUP_MESSAGES 19
#define SETUP_MESSAGES_CONTENT {\
    {0x00,\
        {\
            0x07,0x06,0x00,0x00,0x03,0x02,0x41,0xfe,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x00,0x01,0x00,0x00,0x00,0x02,0x02,0x01,0x04,0x05,0x01,0x01,0x00,0x00,0x06,0x00,0x04,\
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x1c,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf4,0x31,0x02,\
            0x00,0x00,0x08,0x10,0x00,0x00,0x08,0x10,0x03,0x90,0x04,0x64,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x38,0x02,0xff,0x02,0x58,0x0a,0x05,0x00,0x00,0x08,0x10,0x00,0x00,0x08,0x10,0x00,0x00,\
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x05,0x06,0x10,0x54,0x01,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x00,0x04,0x04,0x02,0x02,0x00,0x01,0x28,0x00,0x01,0x00,0x18,0x04,0x04,0x05,0x05,0x00,\
            0x02,0x28,0x03,0x01,0x02,0x03,0x00,0x00,0x2a,0x04,0x04,0x14,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x1c,0x05,0x00,0x03,0x2a,0x00,0x01,0x4e,0x6f,0x74,0x69,0x66,0x63,0x73,0x65,0x6d,0x69,\
            0x2e,0x63,0x6f,0x6d,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x38,0x05,0x05,0x00,0x04,0x28,0x03,0x01,0x02,0x05,0x00,0x01,0x2a,0x06,0x04,0x03,0x02,\
            0x00,0x05,0x2a,0x01,0x01,0x40,0x02,0x04,0x04,0x05,0x05,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x54,0x06,0x28,0x03,0x01,0x02,0x07,0x00,0x04,0x2a,0x06,0x04,0x09,0x08,0x00,0x07,0x2a,\
            0x04,0x01,0x90,0x01,0x20,0x03,0x00,0x00,0x58,0x02,0x04,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x70,0x02,0x02,0x00,0x08,0x28,0x00,0x01,0x01,0x18,0x04,0x04,0x05,0x05,0x00,0x09,0x28,\
            0x03,0x01,0x22,0x0a,0x00,0x05,0x2a,0x26,0x0c,0x05,0x04,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x17,0x06,0x20,0x8c,0x0a,0x2a,0x05,0x01,0x00,0x00,0x00,0x00,0x46,0x34,0x03,0x02,0x00,0x0b,0x29,0x02,\
            0x01,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x0d,0x06,0x30,0x00,0xf4,0x31,0x02,0x01,0x03,0x18,0x01,0x01,0x04,0x01,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x40,0x00,0x2a,0x05,0x01,0x00,0x04,0x04,0x00,0x0a,0x00,0x0b,0x12,0x0d,0x03,0x00,0x08,0x04,\
            0x00,0x00,0x00,0x00,0xc6,0xe9,0x04,0x00,0x08,0x04,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x19,0x06,0x40,0x1c,0x00,0x00,0xd8,0xf3,0x05,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x2a,0x05,0x01,0x00,\
            0x10,0x04,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x50,0x00,0xd0,0x00,0x2d,0x12,0x1e,0x4b,0x0f,0xa4,0x99,0x4e,0xce,0xb5,0x00,0x00,0x05,0x79,\
            0xbd,0x1d,0xa2,0x99,0xe6,0x25,0x58,0x8c,0xd9,0x42,0x01,0x63,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x50,0x1c,0x00,0x00,0xbf,0x9f,0xfb,0x7b,0x7c,0xce,0x6a,0xb3,0x44,0xbe,0xb5,0x4b,0xd6,0x24,\
            0x00,0x00,0xea,0x22,0xd9,0xd9,0xaa,0xfd,0xbd,0x9b,0x21,0x98,\
        },\
    },\
    {0x00,\
        {\
            0x0b,0x06,0x50,0x38,0xa8,0x49,0xe1,0x45,0x00,0x00,0xd1,0x69,\
        },\
    },\
    {0x00,\
        {\
            0x12,0x06,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x06,0x06,0xf0,0x00,0x03,0x71,0x71,\
        },\
    },\
}

#endif
