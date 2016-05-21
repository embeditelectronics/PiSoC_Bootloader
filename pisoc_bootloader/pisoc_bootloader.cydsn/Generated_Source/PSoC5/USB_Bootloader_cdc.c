/*******************************************************************************
* File Name: USB_Bootloader_cdc.c
* Version 2.80
*
* Description:
*  USB CDC class request handler.
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

#include "USB_Bootloader.h"

#if defined(USB_Bootloader_ENABLE_CDC_CLASS)

#include "USB_Bootloader_cdc.h"
#include "USB_Bootloader_pvt.h"
#include "cyapicallbacks.h"


/***************************************
*    CDC Variables
***************************************/

volatile uint8 USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_SIZE] =
{
    0x00u, 0xC2u, 0x01u, 0x00u,     /* Data terminal rate 115200 */
    0x00u,                          /* 1 Stop bit */
    0x00u,                          /* None parity */
    0x08u                           /* 8 data bits */
};
volatile uint8 USB_Bootloader_lineChanged;
volatile uint16 USB_Bootloader_lineControlBitmap;
volatile uint8 USB_Bootloader_cdc_data_in_ep;
volatile uint8 USB_Bootloader_cdc_data_out_ep;


/***************************************
*     Static Function Prototypes
***************************************/
#if (USB_Bootloader_ENABLE_CDC_CLASS_API != 0u)
    static uint16 USB_Bootloader_StrLen(const char8 string[]) ;
#endif /* (USB_Bootloader_ENABLE_CDC_CLASS_API != 0u) */


/***************************************
* Custom Declarations
***************************************/

/* `#START CDC_CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: USB_Bootloader_DispatchCDCClassRqst
********************************************************************************
*
* Summary:
*  This routine dispatches CDC class requests.
*
* Parameters:
*  None.
*
* Return:
*  requestHandled
*
* Global variables:
*   USB_Bootloader_lineCoding: Contains the current line coding structure.
*     It is set by the Host using SET_LINE_CODING request and returned to the
*     user code by the USBFS_GetDTERate(), USBFS_GetCharFormat(),
*     USBFS_GetParityType(), USBFS_GetDataBits() APIs.
*   USB_Bootloader_lineControlBitmap: Contains the current control signal
*     bitmap. It is set by the Host using SET_CONTROL_LINE request and returned
*     to the user code by the USBFS_GetLineControl() API.
*   USB_Bootloader_lineChanged: This variable is used as a flag for the
*     USBFS_IsLineChanged() API, to be aware that Host has been sent request
*     for changing Line Coding or Control Bitmap.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 USB_Bootloader_DispatchCDCClassRqst(void) 
{
    uint8 requestHandled = USB_Bootloader_FALSE;

    if ((CY_GET_REG8(USB_Bootloader_bmRequestType) & USB_Bootloader_RQST_DIR_MASK) == USB_Bootloader_RQST_DIR_D2H)
    {   /* Control Read */
        switch (CY_GET_REG8(USB_Bootloader_bRequest))
        {
            case USB_Bootloader_CDC_GET_LINE_CODING:
                USB_Bootloader_currentTD.count = USB_Bootloader_LINE_CODING_SIZE;
                USB_Bootloader_currentTD.pData = USB_Bootloader_lineCoding;
                requestHandled  = USB_Bootloader_InitControlRead();
                break;

            /* `#START CDC_READ_REQUESTS` Place other request handler here */

            /* `#END` */

            #ifdef USB_Bootloader_DISPATCH_CDC_CLASS_CDC_READ_REQUESTS_CALLBACK
                USB_Bootloader_DispatchCDCClass_CDC_READ_REQUESTS_Callback();
            #endif /* USB_Bootloader_DISPATCH_CDC_CLASS_CDC_READ_REQUESTS_CALLBACK */

            default:    /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    else if ((CY_GET_REG8(USB_Bootloader_bmRequestType) & USB_Bootloader_RQST_DIR_MASK) == \
                                                                            USB_Bootloader_RQST_DIR_H2D)
    {   /* Control Write */
        switch (CY_GET_REG8(USB_Bootloader_bRequest))
        {
            case USB_Bootloader_CDC_SET_LINE_CODING:
                USB_Bootloader_currentTD.count = USB_Bootloader_LINE_CODING_SIZE;
                USB_Bootloader_currentTD.pData = USB_Bootloader_lineCoding;
                USB_Bootloader_lineChanged |= USB_Bootloader_LINE_CODING_CHANGED;
                requestHandled = USB_Bootloader_InitControlWrite();
                break;

            case USB_Bootloader_CDC_SET_CONTROL_LINE_STATE:
                USB_Bootloader_lineControlBitmap = CY_GET_REG8(USB_Bootloader_wValueLo);
                USB_Bootloader_lineChanged |= USB_Bootloader_LINE_CONTROL_CHANGED;
                requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                break;

            /* `#START CDC_WRITE_REQUESTS` Place other request handler here */

            /* `#END` */

            #ifdef USB_Bootloader_DISPATCH_CDC_CLASS_CDC_WRITE_REQUESTS_CALLBACK
                USB_Bootloader_DispatchCDCClass_CDC_WRITE_REQUESTS_Callback();
            #endif /* USB_Bootloader_DISPATCH_CDC_CLASS_CDC_WRITE_REQUESTS_CALLBACK */

            default:    /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    else
    {   /* requestHandled is initialized as FALSE by default */
    }

    return(requestHandled);
}


/***************************************
* Optional CDC APIs
***************************************/
#if (USB_Bootloader_ENABLE_CDC_CLASS_API != 0u)

    /*******************************************************************************
    * Function Name: USB_Bootloader_CDC_Init
    ********************************************************************************
    *
    * Summary:
    *  This function initialize the CDC interface to be ready for the receive data
    *  from the PC.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global variables:
    *   USB_Bootloader_lineChanged: Initialized to zero.
    *   USB_Bootloader_cdc_data_out_ep: Used as an OUT endpoint number.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void USB_Bootloader_CDC_Init(void) 
    {
        USB_Bootloader_lineChanged = 0u;
        USB_Bootloader_EnableOutEP(USB_Bootloader_cdc_data_out_ep);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_PutData
    ********************************************************************************
    *
    * Summary:
    *  This function sends a specified number of bytes from the location specified
    *  by a pointer to the PC. The USB_Bootloader_CDCIsReady() function should be
    *  called before sending new data, to be sure that the previous data has
    *  finished sending.
    *  If the last sent packet is less than maximum packet size the USB transfer
    *  of this short packet will identify the end of the segment. If the last sent
    *  packet is exactly maximum packet size, it shall be followed by a zero-length
    *  packet (which is a short packet) to assure the end of segment is properly
    *  identified. To send zero-length packet, use USB_Bootloader_PutData() API
    *  with length parameter set to zero.
    *
    * Parameters:
    *  pData: pointer to the buffer containing data to be sent.
    *  length: Specifies the number of bytes to send from the pData
    *  buffer. Maximum length will be limited by the maximum packet
    *  size for the endpoint. Data will be lost if length is greater than Max
    *  Packet Size.
    *
    * Return:
    *  None.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_in_ep: CDC IN endpoint number used for sending
    *     data.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void USB_Bootloader_PutData(const uint8* pData, uint16 length) 
    {
        /* Limits length to maximum packet size for the EP */
        if(length > USB_Bootloader_EP[USB_Bootloader_cdc_data_in_ep].bufferSize)
        {
            /* Caution: Data will be lost if length is greater than Max Packet Length */
            length = USB_Bootloader_EP[USB_Bootloader_cdc_data_in_ep].bufferSize;
             /* Halt CPU in debug mode */
            CYASSERT(0u != 0u);
        }
        USB_Bootloader_LoadInEP(USB_Bootloader_cdc_data_in_ep, pData, length);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_StrLen
    ********************************************************************************
    *
    * Summary:
    *  Calculates length of a null terminated string.
    *
    * Parameters:
    *  string: pointer to the string.
    *
    * Return:
    *  Length of the string
    *
    *******************************************************************************/
    static uint16 USB_Bootloader_StrLen(const char8 string[]) 
    {
        uint16 len = 0u;

        while (string[len] != (char8)0)
        {
            len++;
        }

        return (len);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_PutString
    ********************************************************************************
    *
    * Summary:
    *  This function sends a null terminated string to the PC. This function will
    *  block if there is not enough memory to place the whole string. It will block
    *  until the entire string has been written to the transmit buffer.
    *  The USBUART_CDCIsReady() function should be called before sending data with
    *  a new call to USB_Bootloader_PutString(), to be sure that the previous data
    *  has finished sending.
    *
    * Parameters:
    *  string: pointer to the string to be sent to the PC.
    *
    * Return:
    *  None.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_in_ep: CDC IN endpoint number used for sending
    *     data.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void USB_Bootloader_PutString(const char8 string[]) 
    {
        uint16 strLength;
        uint16 sendLength;
        uint16 bufIndex = 0u;

        /* Get length of the null terminated string */
        strLength = USB_Bootloader_StrLen(string);
        do
        {
            /* Limits length to maximum packet size for the EP */
            sendLength = (strLength > USB_Bootloader_EP[USB_Bootloader_cdc_data_in_ep].bufferSize) ?
                          USB_Bootloader_EP[USB_Bootloader_cdc_data_in_ep].bufferSize : strLength;
             /* Enable IN transfer */
            USB_Bootloader_LoadInEP(USB_Bootloader_cdc_data_in_ep, (const uint8 *)&string[bufIndex], sendLength);
            strLength -= sendLength;

            /* If more data are present to send or full packet was sent */
            if((strLength > 0u) || (sendLength == USB_Bootloader_EP[USB_Bootloader_cdc_data_in_ep].bufferSize))
            {
                bufIndex += sendLength;
                /* Wait for the Host to read it. */
                while(USB_Bootloader_EP[USB_Bootloader_cdc_data_in_ep].apiEpState ==
                                          USB_Bootloader_IN_BUFFER_FULL)
                {
                    ;
                }
                /* If the last sent packet is exactly maximum packet size,
                *  it shall be followed by a zero-length packet to assure the
                *  end of segment is properly identified by the terminal.
                */
                if(strLength == 0u)
                {
                    USB_Bootloader_LoadInEP(USB_Bootloader_cdc_data_in_ep, NULL, 0u);
                }
            }
        }while(strLength > 0u);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Writes a single character to the PC.
    *
    * Parameters:
    *  txDataByte: Character to be sent to the PC.
    *
    * Return:
    *  None.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_in_ep: CDC IN endpoint number used for sending
    *     data.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void USB_Bootloader_PutChar(char8 txDataByte) 
    {
        uint8 dataByte;
        dataByte = (uint8)txDataByte;

        USB_Bootloader_LoadInEP(USB_Bootloader_cdc_data_in_ep, &dataByte, 1u);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Sends a carriage return (0x0D) and line feed (0x0A) to the PC
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_in_ep: CDC IN endpoint number used for sending
    *     data.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void USB_Bootloader_PutCRLF(void) 
    {
        const uint8 CYCODE txData[] = {0x0Du, 0x0Au};

        USB_Bootloader_LoadInEP(USB_Bootloader_cdc_data_in_ep, (const uint8 *)txData, 2u);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetCount
    ********************************************************************************
    *
    * Summary:
    *  This function returns the number of bytes that were received from the PC.
    *  The returned length value should be passed to USB_Bootloader_GetData() as
    *  a parameter to read all received data. If all of the received data is not
    *  read at one time by the USB_Bootloader_GetData() API, the unread data will
    *  be lost.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Returns the number of received bytes. The maximum amount of received data at
    *  a time is limited by the maximum packet size for the endpoint.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_out_ep: CDC OUT endpoint number used.
    *
    *******************************************************************************/
    uint16 USB_Bootloader_GetCount(void) 
    {
        uint16 bytesCount;

        if (USB_Bootloader_EP[USB_Bootloader_cdc_data_out_ep].apiEpState == USB_Bootloader_OUT_BUFFER_FULL)
        {
            bytesCount = USB_Bootloader_GetEPCount(USB_Bootloader_cdc_data_out_ep);
        }
        else
        {
            bytesCount = 0u;
        }

        return(bytesCount);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_DataIsReady
    ********************************************************************************
    *
    * Summary:
    *  Returns a nonzero value if the component received data or received
    *  zero-length packet. The USB_Bootloader_GetAll() or
    *  USB_Bootloader_GetData() API should be called to read data from the buffer
    *  and re-init OUT endpoint even when zero-length packet received.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  If the OUT packet received this function returns a nonzero value.
    *  Otherwise zero is returned.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_out_ep: CDC OUT endpoint number used.
    *
    *******************************************************************************/
    uint8 USB_Bootloader_DataIsReady(void) 
    {
        return(USB_Bootloader_EP[USB_Bootloader_cdc_data_out_ep].apiEpState);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_CDCIsReady
    ********************************************************************************
    *
    * Summary:
    *  This function returns a nonzero value if the component is ready to send more
    *  data to the PC; otherwise, it returns zero. The function should be called
    *  before sending new data when using any of the following APIs:
    *  USB_Bootloader_PutData(),USB_Bootloader_PutString(),
    *  USB_Bootloader_PutChar or USB_Bootloader_PutCRLF(),
    *  to be sure that the previous data has finished sending.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  If the buffer can accept new data, this function returns a nonzero value.
    *  Otherwise, it returns zero.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_in_ep: CDC IN endpoint number used.
    *
    *******************************************************************************/
    uint8 USB_Bootloader_CDCIsReady(void) 
    {
        return(USB_Bootloader_EP[USB_Bootloader_cdc_data_in_ep].apiEpState);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetData
    ********************************************************************************
    *
    * Summary:
    *  This function gets a specified number of bytes from the input buffer and
    *  places them in a data array specified by the passed pointer.
    *  The USB_Bootloader_DataIsReady() API should be called first, to be sure
    *  that data is received from the host. If all received data will not be read at
    *  once, the unread data will be lost. The USB_Bootloader_GetData() API should
    *  be called to get the number of bytes that were received.
    *
    * Parameters:
    *  pData: Pointer to the data array where data will be placed.
    *  Length: Number of bytes to read into the data array from the RX buffer.
    *          Maximum length is limited by the the number of received bytes.
    *
    * Return:
    *  Number of bytes received.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_out_ep: CDC OUT endpoint number used.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 USB_Bootloader_GetData(uint8* pData, uint16 length) 
    {
        return(USB_Bootloader_ReadOutEP(USB_Bootloader_cdc_data_out_ep, pData, length));
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetAll
    ********************************************************************************
    *
    * Summary:
    *  Gets all bytes of received data from the input buffer and places it into a
    *  specified data array. USB_Bootloader_DataIsReady() API should be called
    *  before, to be sure that data is received from the Host.
    *
    * Parameters:
    *  pData: Pointer to the data array where data will be placed.
    *
    * Return:
    *  Number of bytes received.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_out_ep: CDC OUT endpoint number used.
    *   USB_Bootloader_EP[].bufferSize: EP max packet size is used as a length
    *     to read all data from the EP buffer.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 USB_Bootloader_GetAll(uint8* pData) 
    {
        return (USB_Bootloader_ReadOutEP(USB_Bootloader_cdc_data_out_ep, pData,
                                           USB_Bootloader_EP[USB_Bootloader_cdc_data_out_ep].bufferSize));
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetChar
    ********************************************************************************
    *
    * Summary:
    *  This function reads one byte of received data from the buffer. If more than
    *  one byte has been received from the host, the rest of the data will be lost.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received one character.
    *
    * Global variables:
    *   USB_Bootloader_cdc_data_out_ep: CDC OUT endpoint number used.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 USB_Bootloader_GetChar(void) 
    {
         uint8 rxData;

        (void) USB_Bootloader_ReadOutEP(USB_Bootloader_cdc_data_out_ep, &rxData, 1u);

        return(rxData);
    }

    /*******************************************************************************
    * Function Name: USB_Bootloader_IsLineChanged
    ********************************************************************************
    *
    * Summary:
    *  This function returns clear on read status of the line. It returns not zero
    *  value when the host sends updated coding or control information to the
    *  device. The USB_Bootloader_GetDTERate(), USB_Bootloader_GetCharFormat()
    *  or USB_Bootloader_GetParityType() or USB_Bootloader_GetDataBits() API
    *  should be called to read data coding information.
    *  The USB_Bootloader_GetLineControl() API should be called to read line
    *  control information.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  If SET_LINE_CODING or CDC_SET_CONTROL_LINE_STATE requests are received, it
    *  returns a nonzero value. Otherwise, it returns zero.
    *
    * Global variables:
    *  USB_Bootloader_transferState: it is checked to be sure then OUT data
    *    phase has been complete, and data written to the lineCoding or Control
    *    Bitmap buffer.
    *  USB_Bootloader_lineChanged: used as a flag to be aware that Host has been
    *    sent request for changing Line Coding or Control Bitmap.
    *
    *******************************************************************************/
    uint8 USB_Bootloader_IsLineChanged(void) 
    {
        uint8 state = 0u;

        /* transferState is checked to be sure then OUT data phase has been complete */
        if(USB_Bootloader_transferState == USB_Bootloader_TRANS_STATE_IDLE)
        {
            if(USB_Bootloader_lineChanged != 0u)
            {
                state = USB_Bootloader_lineChanged;
                USB_Bootloader_lineChanged = 0u;
            }
        }

        return(state);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetDTERate
    ********************************************************************************
    *
    * Summary:
    *  Returns the data terminal rate set for this port in bits per second.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Returns a uint32 value of the data rate in bits per second.
    *
    * Global variables:
    *  USB_Bootloader_lineCoding: First four bytes converted to uint32
    *    depend on compiler, and returned as a data rate.
    *
    *******************************************************************************/
    uint32 USB_Bootloader_GetDTERate(void) 
    {
        uint32 rate;

        rate = USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_RATE + 3u];
        rate = (rate << 8u) | USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_RATE + 2u];
        rate = (rate << 8u) | USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_RATE + 1u];
        rate = (rate << 8u) | USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_RATE];

        return(rate);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetCharFormat
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of stop bits.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Returns the number of stop bits.
    *
    * Global variables:
    *  USB_Bootloader_lineCoding: used to get a parameter.
    *
    *******************************************************************************/
    uint8 USB_Bootloader_GetCharFormat(void) 
    {
        return(USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_STOP_BITS]);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetParityType
    ********************************************************************************
    *
    * Summary:
    *  Returns the parity type for the CDC port.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Returns the parity type.
    *
    * Global variables:
    *  USB_Bootloader_lineCoding: used to get a parameter.
    *
    *******************************************************************************/
    uint8 USB_Bootloader_GetParityType(void) 
    {
        return(USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_PARITY]);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetDataBits
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of data bits for the CDC port.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Returns the number of data bits.
    *  The number of data bits can be 5, 6, 7, 8 or 16.
    *
    * Global variables:
    *  USB_Bootloader_lineCoding: used to get a parameter.
    *
    *******************************************************************************/
    uint8 USB_Bootloader_GetDataBits(void) 
    {
        return(USB_Bootloader_lineCoding[USB_Bootloader_LINE_CODING_DATA_BITS]);
    }


    /*******************************************************************************
    * Function Name: USB_Bootloader_GetLineControl
    ********************************************************************************
    *
    * Summary:
    *  Returns Line control bitmap.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Returns Line control bitmap.
    *
    * Global variables:
    *  USB_Bootloader_lineControlBitmap: used to get a parameter.
    *
    *******************************************************************************/
    uint16 USB_Bootloader_GetLineControl(void) 
    {
        return(USB_Bootloader_lineControlBitmap);
    }

#endif  /*  USB_Bootloader_ENABLE_CDC_CLASS_API*/


/*******************************************************************************
* Additional user functions supporting CDC Requests
********************************************************************************/

/* `#START CDC_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif  /*  USB_Bootloader_ENABLE_CDC_CLASS*/


/* [] END OF FILE */
