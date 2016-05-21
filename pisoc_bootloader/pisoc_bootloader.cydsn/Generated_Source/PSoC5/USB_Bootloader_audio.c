/*******************************************************************************
* File Name: USB_Bootloader_audio.c
* Version 2.80
*
* Description:
*  USB AUDIO Class request handler.
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

#include "USB_Bootloader.h"
#include "cyapicallbacks.h"

#if defined(USB_Bootloader_ENABLE_AUDIO_CLASS)

#include "USB_Bootloader_audio.h"
#include "USB_Bootloader_pvt.h"
#if defined(USB_Bootloader_ENABLE_MIDI_STREAMING)
    #include "USB_Bootloader_midi.h"
#endif /*  USB_Bootloader_ENABLE_MIDI_STREAMING*/


/***************************************
* Custom Declarations
***************************************/

/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


#if !defined(USER_SUPPLIED_AUDIO_HANDLER)


/***************************************
*    AUDIO Variables
***************************************/

#if defined(USB_Bootloader_ENABLE_AUDIO_STREAMING)
    volatile uint8 USB_Bootloader_currentSampleFrequency[USB_Bootloader_MAX_EP][USB_Bootloader_SAMPLE_FREQ_LEN];
    volatile uint8 USB_Bootloader_frequencyChanged;
    volatile uint8 USB_Bootloader_currentMute;
    volatile uint8 USB_Bootloader_currentVolume[USB_Bootloader_VOLUME_LEN];
    volatile uint8 USB_Bootloader_minimumVolume[USB_Bootloader_VOLUME_LEN] = {USB_Bootloader_VOL_MIN_LSB,
                                                                                  USB_Bootloader_VOL_MIN_MSB};
    volatile uint8 USB_Bootloader_maximumVolume[USB_Bootloader_VOLUME_LEN] = {USB_Bootloader_VOL_MAX_LSB,
                                                                                  USB_Bootloader_VOL_MAX_MSB};
    volatile uint8 USB_Bootloader_resolutionVolume[USB_Bootloader_VOLUME_LEN] = {USB_Bootloader_VOL_RES_LSB,
                                                                                     USB_Bootloader_VOL_RES_MSB};
#endif /*  USB_Bootloader_ENABLE_AUDIO_STREAMING */


/*******************************************************************************
* Function Name: USB_Bootloader_DispatchAUDIOClassRqst
********************************************************************************
*
* Summary:
*  This routine dispatches class requests
*
* Parameters:
*  None.
*
* Return:
*  requestHandled
*
* Global variables:
*   USB_Bootloader_currentSampleFrequency: Contains the current audio Sample
*       Frequency. It is set by the Host using SET_CUR request to the endpoint.
*   USB_Bootloader_frequencyChanged: This variable is used as a flag for the
*       user code, to be aware that Host has been sent request for changing
*       Sample Frequency. Sample frequency will be sent on the next OUT
*       transaction. It is contains endpoint address when set. The following
*       code is recommended for detecting new Sample Frequency in main code:
*       if((USB_Bootloader_frequencyChanged != 0) &&
*       (USB_Bootloader_transferState == USB_Bootloader_TRANS_STATE_IDLE))
*       {
*          USB_Bootloader_frequencyChanged = 0;
*       }
*       USB_Bootloader_transferState variable is checked to be sure that
*             transfer completes.
*   USB_Bootloader_currentMute: Contains mute configuration set by Host.
*   USB_Bootloader_currentVolume: Contains volume level set by Host.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 USB_Bootloader_DispatchAUDIOClassRqst(void) 
{
    uint8 requestHandled = USB_Bootloader_FALSE;
    uint8 bmRequestType  = CY_GET_REG8(USB_Bootloader_bmRequestType);

    #if defined(USB_Bootloader_ENABLE_AUDIO_STREAMING)
        uint8 epNumber;
        epNumber = CY_GET_REG8(USB_Bootloader_wIndexLo) & USB_Bootloader_DIR_UNUSED;
    #endif /*  USB_Bootloader_ENABLE_AUDIO_STREAMING */


    if ((bmRequestType & USB_Bootloader_RQST_DIR_MASK) == USB_Bootloader_RQST_DIR_D2H)
    {
        /* Control Read */
        if((bmRequestType & USB_Bootloader_RQST_RCPT_MASK) == USB_Bootloader_RQST_RCPT_EP)
        {
            /* Endpoint */
            switch (CY_GET_REG8(USB_Bootloader_bRequest))
            {
                case USB_Bootloader_GET_CUR:
                #if defined(USB_Bootloader_ENABLE_AUDIO_STREAMING)
                    if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_SAMPLING_FREQ_CONTROL)
                    {
                         /* point Control Selector is Sampling Frequency */
                        USB_Bootloader_currentTD.wCount = USB_Bootloader_SAMPLE_FREQ_LEN;
                        USB_Bootloader_currentTD.pData  = USB_Bootloader_currentSampleFrequency[epNumber];
                        requestHandled   = USB_Bootloader_InitControlRead();
                    }
                #endif /*  USB_Bootloader_ENABLE_AUDIO_STREAMING */

                /* `#START AUDIO_READ_REQUESTS` Place other request handler here */

                /* `#END` */
                
                #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_READ_REQUESTS_CALLBACK
                    USB_Bootloader_DispatchAUDIOClass_AUDIO_READ_REQUESTS_Callback();
                #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_READ_REQUESTS_CALLBACK */

                    break;
                default:
                    break;
            }
        }
        else if((bmRequestType & USB_Bootloader_RQST_RCPT_MASK) == USB_Bootloader_RQST_RCPT_IFC)
        {
            /* Interface or Entity ID */
            switch (CY_GET_REG8(USB_Bootloader_bRequest))
            {
                case USB_Bootloader_GET_CUR:
                #if defined(USB_Bootloader_ENABLE_AUDIO_STREAMING)
                    if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_MUTE_CONTROL)
                    {
                        /* `#START MUTE_CONTROL_GET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                        #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_MUTE_CONTROL_GET_REQUEST_CALLBACK
                            USB_Bootloader_DispatchAUDIOClass_MUTE_CONTROL_GET_REQUEST_Callback();
                        #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_MUTE_CONTROL_GET_REQUEST_CALLBACK */

                        /* Entity ID Control Selector is MUTE */
                        USB_Bootloader_currentTD.wCount = 1u;
                        USB_Bootloader_currentTD.pData  = &USB_Bootloader_currentMute;
                        requestHandled   = USB_Bootloader_InitControlRead();
                    }
                    else if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_VOLUME_CONTROL)
                    {
                        /* `#START VOLUME_CONTROL_GET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                        #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_GET_REQUEST_CALLBACK
                            USB_Bootloader_DispatchAUDIOClass_VOLUME_CONTROL_GET_REQUEST_Callback();
                        #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_GET_REQUEST_CALLBACK */

                        /* Entity ID Control Selector is VOLUME, */
                        USB_Bootloader_currentTD.wCount = USB_Bootloader_VOLUME_LEN;
                        USB_Bootloader_currentTD.pData  = USB_Bootloader_currentVolume;
                        requestHandled   = USB_Bootloader_InitControlRead();
                    }
                    else
                    {
                        /* `#START OTHER_GET_CUR_REQUESTS` Place other request handler here */

                        /* `#END` */

                        #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_OTHER_GET_CUR_REQUESTS_CALLBACK
                            USB_Bootloader_DispatchAUDIOClass_OTHER_GET_CUR_REQUESTS_Callback();
                        #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_OTHER_GET_CUR_REQUESTS_CALLBACK */
                    }
                    break;
                case USB_Bootloader_GET_MIN:    /* GET_MIN */
                    if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_VOLUME_CONTROL)
                    {
                        /* Entity ID Control Selector is VOLUME, */
                        USB_Bootloader_currentTD.wCount = USB_Bootloader_VOLUME_LEN;
                        USB_Bootloader_currentTD.pData  = &USB_Bootloader_minimumVolume[0];
                        requestHandled   = USB_Bootloader_InitControlRead();
                    }
                    break;
                case USB_Bootloader_GET_MAX:    /* GET_MAX */
                    if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_VOLUME_CONTROL)
                    {
                        /* Entity ID Control Selector is VOLUME, */
                        USB_Bootloader_currentTD.wCount = USB_Bootloader_VOLUME_LEN;
                        USB_Bootloader_currentTD.pData  = &USB_Bootloader_maximumVolume[0];
                        requestHandled   = USB_Bootloader_InitControlRead();
                    }
                    break;
                case USB_Bootloader_GET_RES:    /* GET_RES */
                    if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_VOLUME_CONTROL)
                    {
                         /* Entity ID Control Selector is VOLUME, */
                        USB_Bootloader_currentTD.wCount = USB_Bootloader_VOLUME_LEN;
                        USB_Bootloader_currentTD.pData  = &USB_Bootloader_resolutionVolume[0];
                        requestHandled   = USB_Bootloader_InitControlRead();
                    }
                    break;
                /* The contents of the status message is reserved for future use.
                *  For the time being, a null packet should be returned in the data stage of the
                *  control transfer, and the received null packet should be ACKed.
                */
                case USB_Bootloader_GET_STAT:
                        USB_Bootloader_currentTD.wCount = 0u;
                        requestHandled   = USB_Bootloader_InitControlWrite();

                #endif /*  USB_Bootloader_ENABLE_AUDIO_STREAMING */

                /* `#START AUDIO_WRITE_REQUESTS` Place other request handler here */

                /* `#END` */

                #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_WRITE_REQUESTS_CALLBACK
                    USB_Bootloader_DispatchAUDIOClass_AUDIO_WRITE_REQUESTS_Callback();
                #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_WRITE_REQUESTS_CALLBACK */

                    break;
                default:
                    break;
            }
        }
        else
        {   /* USB_Bootloader_RQST_RCPT_OTHER */
        }
    }
    else
    {
        /* Control Write */
        if((bmRequestType & USB_Bootloader_RQST_RCPT_MASK) == USB_Bootloader_RQST_RCPT_EP)
        {
            /* point */
            switch (CY_GET_REG8(USB_Bootloader_bRequest))
            {
                case USB_Bootloader_SET_CUR:
                #if defined(USB_Bootloader_ENABLE_AUDIO_STREAMING)
                    if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_SAMPLING_FREQ_CONTROL)
                    {
                         /* point Control Selector is Sampling Frequency */
                        USB_Bootloader_currentTD.wCount = USB_Bootloader_SAMPLE_FREQ_LEN;
                        USB_Bootloader_currentTD.pData  = USB_Bootloader_currentSampleFrequency[epNumber];
                        requestHandled   = USB_Bootloader_InitControlWrite();
                        USB_Bootloader_frequencyChanged = epNumber;
                    }
                #endif /*  USB_Bootloader_ENABLE_AUDIO_STREAMING */

                /* `#START AUDIO_SAMPLING_FREQ_REQUESTS` Place other request handler here */

                /* `#END` */

                #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_SAMPLING_FREQ_REQUESTS_CALLBACK
                    USB_Bootloader_DispatchAUDIOClass_AUDIO_SAMPLING_FREQ_REQUESTS_Callback();
                #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_SAMPLING_FREQ_REQUESTS_CALLBACK */

                    break;
                default:
                    break;
            }
        }
        else if((bmRequestType & USB_Bootloader_RQST_RCPT_MASK) == USB_Bootloader_RQST_RCPT_IFC)
        {
            /* Interface or Entity ID */
            switch (CY_GET_REG8(USB_Bootloader_bRequest))
            {
                case USB_Bootloader_SET_CUR:
                #if defined(USB_Bootloader_ENABLE_AUDIO_STREAMING)
                    if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_MUTE_CONTROL)
                    {
                        /* `#START MUTE_SET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                        #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_MUTE_SET_REQUEST_CALLBACK
                            USB_Bootloader_DispatchAUDIOClass_MUTE_SET_REQUEST_Callback();
                        #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_MUTE_SET_REQUEST_CALLBACK */

                        /* Entity ID Control Selector is MUTE */
                        USB_Bootloader_currentTD.wCount = 1u;
                        USB_Bootloader_currentTD.pData  = &USB_Bootloader_currentMute;
                        requestHandled   = USB_Bootloader_InitControlWrite();
                    }
                    else if(CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_VOLUME_CONTROL)
                    {
                        /* `#START VOLUME_CONTROL_SET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                        #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_SET_REQUEST_CALLBACK
                            USB_Bootloader_DispatchAUDIOClass_VOLUME_CONTROL_SET_REQUEST_Callback();
                        #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_SET_REQUEST_CALLBACK */

                        /* Entity ID Control Selector is VOLUME */
                        USB_Bootloader_currentTD.wCount = USB_Bootloader_VOLUME_LEN;
                        USB_Bootloader_currentTD.pData  = USB_Bootloader_currentVolume;
                        requestHandled   = USB_Bootloader_InitControlWrite();
                    }
                    else
                    {
                        /* `#START OTHER_SET_CUR_REQUESTS` Place other request handler here */

                        /* `#END` */

                        #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_OTHER_SET_CUR_REQUESTS_CALLBACK
                            USB_Bootloader_DispatchAUDIOClass_OTHER_SET_CUR_REQUESTS_Callback();
                        #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_OTHER_SET_CUR_REQUESTS_CALLBACK */
                    }
                #endif /*  USB_Bootloader_ENABLE_AUDIO_STREAMING */

                /* `#START AUDIO_CONTROL_SEL_REQUESTS` Place other request handler here */

                /* `#END` */

                #ifdef USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_CONTROL_SEL_REQUESTS_CALLBACK
                    USB_Bootloader_DispatchAUDIOClass_AUDIO_CONTROL_SEL_REQUESTS_Callback();
                #endif /* USB_Bootloader_DISPATCH_AUDIO_CLASS_AUDIO_CONTROL_SEL_REQUESTS_CALLBACK */

                    break;
                default:
                    break;
            }
        }
        else
        {
            /* USB_Bootloader_RQST_RCPT_OTHER */
        }
    }

    return(requestHandled);
}

#endif /* USER_SUPPLIED_AUDIO_HANDLER */


/*******************************************************************************
* Additional user functions supporting AUDIO Requests
********************************************************************************/

/* `#START AUDIO_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif  /*  USB_Bootloader_ENABLE_AUDIO_CLASS */


/* [] END OF FILE */
