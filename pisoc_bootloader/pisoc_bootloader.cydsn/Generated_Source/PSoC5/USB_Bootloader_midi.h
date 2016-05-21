/*******************************************************************************
* File Name: USB_Bootloader_midi.h
* Version 2.80
*
* Description:
*  Header File for the USBFS MIDI module.
*  Contains prototypes and constant values.
*
* Related Document:
*  Universal Serial Bus Device Class Definition for MIDI Devices Release 1.0
*  MIDI 1.0 Detailed Specification Document Version 4.2
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USB_Bootloader_midi_H)
#define CY_USBFS_USB_Bootloader_midi_H

#include "cytypes.h"
#include "USB_Bootloader.h"


/***************************************
*    Data Structure Definition
***************************************/

/* The following structure is used to hold status information for
   building and parsing incoming MIDI messages. */
typedef struct
{
    uint8    length;        /* expected length */
    uint8    count;         /* current byte count */
    uint8    size;          /* complete size */
    uint8    runstat;       /* running status */
    uint8    msgBuff[4];    /* message buffer */
} USB_Bootloader_MIDI_RX_STATUS;


/***************************************
*           MIDI Constants.
***************************************/

#define USB_Bootloader_ONE_EXT_INTRF              (0x01u)
#define USB_Bootloader_TWO_EXT_INTRF              (0x02u)

/* Flag definitions for use with MIDI device inquiry */
#define USB_Bootloader_INQ_SYSEX_FLAG             (0x01u)
#define USB_Bootloader_INQ_IDENTITY_REQ_FLAG      (0x02u)

/* USB-MIDI Code Index Number Classifications (MIDI Table 4-1) */
#define USB_Bootloader_CIN_MASK                   (0x0Fu)
#define USB_Bootloader_RESERVED0                  (0x00u)
#define USB_Bootloader_RESERVED1                  (0x01u)
#define USB_Bootloader_2BYTE_COMMON               (0x02u)
#define USB_Bootloader_3BYTE_COMMON               (0x03u)
#define USB_Bootloader_SYSEX                      (0x04u)
#define USB_Bootloader_1BYTE_COMMON               (0x05u)
#define USB_Bootloader_SYSEX_ENDS_WITH1           (0x05u)
#define USB_Bootloader_SYSEX_ENDS_WITH2           (0x06u)
#define USB_Bootloader_SYSEX_ENDS_WITH3           (0x07u)
#define USB_Bootloader_NOTE_OFF                   (0x08u)
#define USB_Bootloader_NOTE_ON                    (0x09u)
#define USB_Bootloader_POLY_KEY_PRESSURE          (0x0Au)
#define USB_Bootloader_CONTROL_CHANGE             (0x0Bu)
#define USB_Bootloader_PROGRAM_CHANGE             (0x0Cu)
#define USB_Bootloader_CHANNEL_PRESSURE           (0x0Du)
#define USB_Bootloader_PITCH_BEND_CHANGE          (0x0Eu)
#define USB_Bootloader_SINGLE_BYTE                (0x0Fu)

#define USB_Bootloader_CABLE_MASK                 (0xF0u)
#define USB_Bootloader_MIDI_CABLE_00              (0x00u)
#define USB_Bootloader_MIDI_CABLE_01              (0x10u)

#define USB_Bootloader_EVENT_BYTE0                (0x00u)
#define USB_Bootloader_EVENT_BYTE1                (0x01u)
#define USB_Bootloader_EVENT_BYTE2                (0x02u)
#define USB_Bootloader_EVENT_BYTE3                (0x03u)
#define USB_Bootloader_EVENT_LENGTH               (0x04u)

#define USB_Bootloader_MIDI_STATUS_BYTE_MASK      (0x80u)
#define USB_Bootloader_MIDI_STATUS_MASK           (0xF0u)
#define USB_Bootloader_MIDI_SINGLE_BYTE_MASK      (0x08u)
#define USB_Bootloader_MIDI_NOTE_OFF              (0x80u)
#define USB_Bootloader_MIDI_NOTE_ON               (0x90u)
#define USB_Bootloader_MIDI_POLY_KEY_PRESSURE     (0xA0u)
#define USB_Bootloader_MIDI_CONTROL_CHANGE        (0xB0u)
#define USB_Bootloader_MIDI_PROGRAM_CHANGE        (0xC0u)
#define USB_Bootloader_MIDI_CHANNEL_PRESSURE      (0xD0u)
#define USB_Bootloader_MIDI_PITCH_BEND_CHANGE     (0xE0u)
#define USB_Bootloader_MIDI_SYSEX                 (0xF0u)
#define USB_Bootloader_MIDI_EOSEX                 (0xF7u)
#define USB_Bootloader_MIDI_QFM                   (0xF1u)
#define USB_Bootloader_MIDI_SPP                   (0xF2u)
#define USB_Bootloader_MIDI_SONGSEL               (0xF3u)
#define USB_Bootloader_MIDI_TUNEREQ               (0xF6u)
#define USB_Bootloader_MIDI_ACTIVESENSE           (0xFEu)

/* MIDI Universal System Exclusive defines */
#define USB_Bootloader_MIDI_SYSEX_NON_REAL_TIME   (0x7Eu)
#define USB_Bootloader_MIDI_SYSEX_REALTIME        (0x7Fu)
/* ID of target device */
#define USB_Bootloader_MIDI_SYSEX_ID_ALL          (0x7Fu)
/* Sub-ID#1*/
#define USB_Bootloader_MIDI_SYSEX_GEN_INFORMATION (0x06u)
#define USB_Bootloader_MIDI_SYSEX_GEN_MESSAGE     (0x09u)
/* Sub-ID#2*/
#define USB_Bootloader_MIDI_SYSEX_IDENTITY_REQ    (0x01u)
#define USB_Bootloader_MIDI_SYSEX_IDENTITY_REPLY  (0x02u)
#define USB_Bootloader_MIDI_SYSEX_SYSTEM_ON       (0x01u)
#define USB_Bootloader_MIDI_SYSEX_SYSTEM_OFF      (0x02u)

#define USB_Bootloader_CUSTOM_UART_TX_PRIOR_NUM   (0x04u)
#define USB_Bootloader_CUSTOM_UART_RX_PRIOR_NUM   (0x02u)

#define USB_Bootloader_ISR_SERVICE_MIDI_OUT         \
        ( (USB_Bootloader_ENABLE_MIDI_API != 0u) && \
          (USB_Bootloader_MIDI_OUT_BUFF_SIZE > 0) && (USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO))
#define USB_Bootloader_ISR_SERVICE_MIDI_IN     \
        ( (USB_Bootloader_ENABLE_MIDI_API != 0u) && (USB_Bootloader_MIDI_IN_BUFF_SIZE > 0) )


/***************************************
* External function references
***************************************/

void USB_Bootloader_callbackLocalMidiEvent(uint8 cable, uint8 *midiMsg)
                                                     ;


/***************************************
*    External references
***************************************/

#if (USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_ONE_EXT_INTRF)
    #include "MIDI1_UART.h"
#endif /*  USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_ONE_EXT_INTRF */
#if (USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_TWO_EXT_INTRF)
    #include "MIDI2_UART.h"
#endif /*  USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_TWO_EXT_INTRF */
#if(USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL)
    #include <CyDmac.h>
#endif /*  USB_Bootloader_EP_MM */


/***************************************
*    Private function prototypes
***************************************/

void USB_Bootloader_PrepareInBuffer(uint8 ic, const uint8 srcBuff[], uint8 eventLen, uint8 cable)
                                                                 ;
#if (USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_ONE_EXT_INTRF)
    void USB_Bootloader_MIDI_Init(void) ;
    uint8 USB_Bootloader_ProcessMidiIn(uint8 mData, USB_Bootloader_MIDI_RX_STATUS *rxStat)
                                                                ;
    uint8 USB_Bootloader_MIDI1_GetEvent(void) ;
    void USB_Bootloader_MIDI1_ProcessUsbOut(const uint8 epBuf[])
                                                     ;

    #if (USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_TWO_EXT_INTRF)
        uint8 USB_Bootloader_MIDI2_GetEvent(void) ;
        void USB_Bootloader_MIDI2_ProcessUsbOut(const uint8 epBuf[])
                                                    ;
    #endif /*  USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_TWO_EXT_INTRF */
#endif /*  USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_ONE_EXT_INTRF */


/***************************************
* External data references
***************************************/

#if defined(USB_Bootloader_ENABLE_MIDI_STREAMING)

#if (USB_Bootloader_MIDI_IN_BUFF_SIZE > 0)
    #if (USB_Bootloader_MIDI_IN_BUFF_SIZE >= 256)
        extern volatile uint16 USB_Bootloader_midiInPointer;                       /* Input endpoint buffer pointer */
    #else
        extern volatile uint8 USB_Bootloader_midiInPointer;                        /* Input endpoint buffer pointer */
    #endif /*  USB_Bootloader_MIDI_IN_BUFF_SIZE >=256 */
    extern volatile uint8 USB_Bootloader_midi_in_ep;                               /* Input endpoint number */
    extern uint8 USB_Bootloader_midiInBuffer[USB_Bootloader_MIDI_IN_BUFF_SIZE];  /* Input endpoint buffer */
#endif /* USB_Bootloader_MIDI_IN_BUFF_SIZE > 0 */

#if (USB_Bootloader_MIDI_OUT_BUFF_SIZE > 0)
    extern volatile uint8 USB_Bootloader_midi_out_ep;                               /* Output endpoint number */
    extern uint8 USB_Bootloader_midiOutBuffer[USB_Bootloader_MIDI_OUT_BUFF_SIZE]; /* Output endpoint buffer */
#endif /* USB_Bootloader_MIDI_OUT_BUFF_SIZE > 0 */

#if (USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_ONE_EXT_INTRF)
    extern volatile uint8 USB_Bootloader_MIDI1_InqFlags;                              /* Device inquiry flag */
    #if (USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_TWO_EXT_INTRF)
        extern volatile uint8 USB_Bootloader_MIDI2_InqFlags;                          /* Device inquiry flag */
    #endif /*  USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_TWO_EXT_INTRF */
#endif /*  USB_Bootloader_MIDI_EXT_MODE >= USB_Bootloader_ONE_EXT_INTRF */

#endif /* USB_Bootloader_ENABLE_MIDI_STREAMING */


#endif /*  CY_USBFS_USB_Bootloader_midi_H */


/* [] END OF FILE */
