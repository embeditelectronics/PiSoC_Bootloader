/*******************************************************************************
* File Name: USB_Bootloader_audio.h
* Version 2.80
*
* Description:
*  Header File for the USBFS component. Contains prototypes and constant values.
*
* Related Document:
*  Universal Serial Bus Device Class Definition for Audio Devices Release 1.0
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USB_Bootloader_audio_H)
#define CY_USBFS_USB_Bootloader_audio_H

#include "cytypes.h"


/***************************************
* Custom Declarations
***************************************/

/* `#START CUSTOM_CONSTANTS` Place your declaration here */

/* `#END` */


/***************************************
*  Constants for USB_Bootloader_audio API.
***************************************/

/* Audio Class-Specific Request Codes (AUDIO Table A-9) */
#define USB_Bootloader_REQUEST_CODE_UNDEFINED     (0x00u)
#define USB_Bootloader_SET_CUR                    (0x01u)
#define USB_Bootloader_GET_CUR                    (0x81u)
#define USB_Bootloader_SET_MIN                    (0x02u)
#define USB_Bootloader_GET_MIN                    (0x82u)
#define USB_Bootloader_SET_MAX                    (0x03u)
#define USB_Bootloader_GET_MAX                    (0x83u)
#define USB_Bootloader_SET_RES                    (0x04u)
#define USB_Bootloader_GET_RES                    (0x84u)
#define USB_Bootloader_SET_MEM                    (0x05u)
#define USB_Bootloader_GET_MEM                    (0x85u)
#define USB_Bootloader_GET_STAT                   (0xFFu)

/* point Control Selectors (AUDIO Table A-19) */
#define USB_Bootloader_EP_CONTROL_UNDEFINED       (0x00u)
#define USB_Bootloader_SAMPLING_FREQ_CONTROL      (0x01u)
#define USB_Bootloader_PITCH_CONTROL              (0x02u)

/* Feature Unit Control Selectors (AUDIO Table A-11) */
#define USB_Bootloader_FU_CONTROL_UNDEFINED       (0x00u)
#define USB_Bootloader_MUTE_CONTROL               (0x01u)
#define USB_Bootloader_VOLUME_CONTROL             (0x02u)
#define USB_Bootloader_BASS_CONTROL               (0x03u)
#define USB_Bootloader_MID_CONTROL                (0x04u)
#define USB_Bootloader_TREBLE_CONTROL             (0x05u)
#define USB_Bootloader_GRAPHIC_EQUALIZER_CONTROL  (0x06u)
#define USB_Bootloader_AUTOMATIC_GAIN_CONTROL     (0x07u)
#define USB_Bootloader_DELAY_CONTROL              (0x08u)
#define USB_Bootloader_BASS_BOOST_CONTROL         (0x09u)
#define USB_Bootloader_LOUDNESS_CONTROL           (0x0Au)

#define USB_Bootloader_SAMPLE_FREQ_LEN            (3u)
#define USB_Bootloader_VOLUME_LEN                 (2u)

#if !defined(USER_SUPPLIED_DEFAULT_VOLUME_VALUE)
    #define USB_Bootloader_VOL_MIN_MSB            (0x80u)
    #define USB_Bootloader_VOL_MIN_LSB            (0x01u)
    #define USB_Bootloader_VOL_MAX_MSB            (0x7Fu)
    #define USB_Bootloader_VOL_MAX_LSB            (0xFFu)
    #define USB_Bootloader_VOL_RES_MSB            (0x00u)
    #define USB_Bootloader_VOL_RES_LSB            (0x01u)
#endif /* USER_SUPPLIED_DEFAULT_VOLUME_VALUE */


/***************************************
* External data references
***************************************/

extern volatile uint8 USB_Bootloader_currentSampleFrequency[USB_Bootloader_MAX_EP]
                                                             [USB_Bootloader_SAMPLE_FREQ_LEN];
extern volatile uint8 USB_Bootloader_frequencyChanged;
extern volatile uint8 USB_Bootloader_currentMute;
extern volatile uint8 USB_Bootloader_currentVolume[USB_Bootloader_VOLUME_LEN];
extern volatile uint8 USB_Bootloader_minimumVolume[USB_Bootloader_VOLUME_LEN];
extern volatile uint8 USB_Bootloader_maximumVolume[USB_Bootloader_VOLUME_LEN];
extern volatile uint8 USB_Bootloader_resolutionVolume[USB_Bootloader_VOLUME_LEN];

#endif /*  CY_USBFS_USB_Bootloader_audio_H */


/* [] END OF FILE */
