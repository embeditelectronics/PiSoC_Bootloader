/*******************************************************************************
* File Name: USB_Bootloader_cdc.h
* Version 2.80
*
* Description:
*  Header File for the USBFS component.
*  Contains CDC class prototypes and constant values.
*
* Related Document:
*  Universal Serial Bus Class Definitions for Communication Devices Version 1.1
*
********************************************************************************
* Copyright 2012-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USB_Bootloader_cdc_H)
#define CY_USBFS_USB_Bootloader_cdc_H

#include "cytypes.h"


/***************************************
* Prototypes of the USB_Bootloader_cdc API.
***************************************/

#if (USB_Bootloader_ENABLE_CDC_CLASS_API != 0u)
    void USB_Bootloader_CDC_Init(void) ;
    void USB_Bootloader_PutData(const uint8* pData, uint16 length) ;
    void USB_Bootloader_PutString(const char8 string[]) ;
    void USB_Bootloader_PutChar(char8 txDataByte) ;
    void USB_Bootloader_PutCRLF(void) ;
    uint16 USB_Bootloader_GetCount(void) ;
    uint8 USB_Bootloader_CDCIsReady(void) ;
    uint8 USB_Bootloader_DataIsReady(void) ;
    uint16 USB_Bootloader_GetData(uint8* pData, uint16 length) ;
    uint16 USB_Bootloader_GetAll(uint8* pData) ;
    uint8 USB_Bootloader_GetChar(void) ;
    uint8 USB_Bootloader_IsLineChanged(void) ;
    uint32 USB_Bootloader_GetDTERate(void) ;
    uint8 USB_Bootloader_GetCharFormat(void) ;
    uint8 USB_Bootloader_GetParityType(void) ;
    uint8 USB_Bootloader_GetDataBits(void) ;
    uint16 USB_Bootloader_GetLineControl(void) ;
#endif  /*  USB_Bootloader_ENABLE_CDC_CLASS_API */


/***************************************
*  Constants for USB_Bootloader_cdc API.
***************************************/

/* CDC Class-Specific Request Codes (CDC ver 1.2 Table 19) */
#define USB_Bootloader_CDC_SET_LINE_CODING        (0x20u)
#define USB_Bootloader_CDC_GET_LINE_CODING        (0x21u)
#define USB_Bootloader_CDC_SET_CONTROL_LINE_STATE (0x22u)

#define USB_Bootloader_LINE_CODING_CHANGED        (0x01u)
#define USB_Bootloader_LINE_CONTROL_CHANGED       (0x02u)

#define USB_Bootloader_1_STOPBIT                  (0x00u)
#define USB_Bootloader_1_5_STOPBITS               (0x01u)
#define USB_Bootloader_2_STOPBITS                 (0x02u)

#define USB_Bootloader_PARITY_NONE                (0x00u)
#define USB_Bootloader_PARITY_ODD                 (0x01u)
#define USB_Bootloader_PARITY_EVEN                (0x02u)
#define USB_Bootloader_PARITY_MARK                (0x03u)
#define USB_Bootloader_PARITY_SPACE               (0x04u)

#define USB_Bootloader_LINE_CODING_SIZE           (0x07u)
#define USB_Bootloader_LINE_CODING_RATE           (0x00u)
#define USB_Bootloader_LINE_CODING_STOP_BITS      (0x04u)
#define USB_Bootloader_LINE_CODING_PARITY         (0x05u)
#define USB_Bootloader_LINE_CODING_DATA_BITS      (0x06u)

#define USB_Bootloader_LINE_CONTROL_DTR           (0x01u)
#define USB_Bootloader_LINE_CONTROL_RTS           (0x02u)


/***************************************
* External data references
***************************************/

extern volatile uint8 USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_SIZE];
extern volatile uint8 USB_Bootloader_lineChanged;
extern volatile uint16 USB_Bootloader_lineControlBitmap;
extern volatile uint8 USB_Bootloader_cdc_data_in_ep;
extern volatile uint8 USB_Bootloader_cdc_data_out_ep;

#endif /*  CY_USBFS_USB_Bootloader_cdc_H */


/* [] END OF FILE */
