/*******************************************************************************
* File Name: USB_Bootloader_std.c
* Version 2.80
*
* Description:
*  USB Standard request handler.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USB_Bootloader.h"
#include "USB_Bootloader_cdc.h"
#include "USB_Bootloader_pvt.h"
#if defined(USB_Bootloader_ENABLE_MIDI_STREAMING)
    #include "USB_Bootloader_midi.h"
#endif /*  USB_Bootloader_ENABLE_MIDI_STREAMING*/


/***************************************
*   Static data allocation
***************************************/

#if defined(USB_Bootloader_ENABLE_FWSN_STRING)
    static volatile uint8 *USB_Bootloader_fwSerialNumberStringDescriptor;
    static volatile uint8 USB_Bootloader_snStringConfirm = USB_Bootloader_FALSE;
#endif  /* USB_Bootloader_ENABLE_FWSN_STRING */

#if defined(USB_Bootloader_ENABLE_FWSN_STRING)

    /*******************************************************************************
    * Function Name: USB_Bootloader_SerialNumString
    ********************************************************************************
    *
    * Summary:
    *  Application firmware may supply the source of the USB device descriptors
    *  serial number string during runtime.
    *
    * Parameters:
    *  snString:  pointer to string.
    *
    * Return:
    *  None.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void  USB_Bootloader_SerialNumString(uint8 snString[]) 
    {
        USB_Bootloader_snStringConfirm = USB_Bootloader_FALSE;
        if(snString != NULL)
        {
            /* Check descriptor validation */
            if( (snString[0u] > 1u ) && (snString[1u] == USB_Bootloader_DESCR_STRING) )
            {
                USB_Bootloader_fwSerialNumberStringDescriptor = snString;
                USB_Bootloader_snStringConfirm = USB_Bootloader_TRUE;
            }
        }
    }

#endif  /* USB_Bootloader_ENABLE_FWSN_STRING */


/*******************************************************************************
* Function Name: USB_Bootloader_HandleStandardRqst
********************************************************************************
*
* Summary:
*  This Routine dispatches standard requests
*
* Parameters:
*  None.
*
* Return:
*  TRUE if request handled.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 USB_Bootloader_HandleStandardRqst(void) 
{
    uint8 requestHandled = USB_Bootloader_FALSE;
    uint8 interfaceNumber;
    uint8 configurationN;
    #if defined(USB_Bootloader_ENABLE_STRINGS)
        volatile uint8 *pStr = 0u;
        #if defined(USB_Bootloader_ENABLE_DESCRIPTOR_STRINGS)
            uint8 nStr;
            uint8 descrLength;
        #endif /* USB_Bootloader_ENABLE_DESCRIPTOR_STRINGS */
    #endif /* USB_Bootloader_ENABLE_STRINGS */
    static volatile uint8 USB_Bootloader_tBuffer[USB_Bootloader_STATUS_LENGTH_MAX];
    const T_USB_Bootloader_LUT CYCODE *pTmp;
    USB_Bootloader_currentTD.count = 0u;

    if ((CY_GET_REG8(USB_Bootloader_bmRequestType) & USB_Bootloader_RQST_DIR_MASK) == USB_Bootloader_RQST_DIR_D2H)
    {
        /* Control Read */
        switch (CY_GET_REG8(USB_Bootloader_bRequest))
        {
            case USB_Bootloader_GET_DESCRIPTOR:
                if (CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_DESCR_DEVICE)
                {
                    pTmp = USB_Bootloader_GetDeviceTablePtr();
                    USB_Bootloader_currentTD.pData = (volatile uint8 *)pTmp->p_list;
                    USB_Bootloader_currentTD.count = USB_Bootloader_DEVICE_DESCR_LENGTH;
                    requestHandled  = USB_Bootloader_InitControlRead();
                }
                else if (CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_DESCR_CONFIG)
                {
                    pTmp = USB_Bootloader_GetConfigTablePtr(CY_GET_REG8(USB_Bootloader_wValueLo));
                    if( pTmp != NULL )  /* Verify that requested descriptor exists */
                    {
                        USB_Bootloader_currentTD.pData = (volatile uint8 *)pTmp->p_list;
                        USB_Bootloader_currentTD.count = ((uint16)(USB_Bootloader_currentTD.pData)[ \
                                          USB_Bootloader_CONFIG_DESCR_TOTAL_LENGTH_HI] << 8u) | \
                                         (USB_Bootloader_currentTD.pData)[USB_Bootloader_CONFIG_DESCR_TOTAL_LENGTH_LOW];
                        requestHandled  = USB_Bootloader_InitControlRead();
                    }
                }
                #if defined(USB_Bootloader_ENABLE_STRINGS)
                else if (CY_GET_REG8(USB_Bootloader_wValueHi) == USB_Bootloader_DESCR_STRING)
                {
                    /* Descriptor Strings*/
                    #if defined(USB_Bootloader_ENABLE_DESCRIPTOR_STRINGS)
                        nStr = 0u;
                        pStr = (volatile uint8 *)&USB_Bootloader_STRING_DESCRIPTORS[0u];
                        while ( (CY_GET_REG8(USB_Bootloader_wValueLo) > nStr) && (*pStr != 0u) )
                        {
                            /* Read descriptor length from 1st byte */
                            descrLength = *pStr;
                            /* Move to next string descriptor */
                            pStr = &pStr[descrLength];
                            nStr++;
                        }
                    #endif /*  USB_Bootloader_ENABLE_DESCRIPTOR_STRINGS */
                    /* Microsoft OS String*/
                    #if defined(USB_Bootloader_ENABLE_MSOS_STRING)
                        if( CY_GET_REG8(USB_Bootloader_wValueLo) == USB_Bootloader_STRING_MSOS )
                        {
                            pStr = (volatile uint8 *)&USB_Bootloader_MSOS_DESCRIPTOR[0u];
                        }
                    #endif /*  USB_Bootloader_ENABLE_MSOS_STRING*/
                    /* SN string */
                    #if defined(USB_Bootloader_ENABLE_SN_STRING)
                        if( (CY_GET_REG8(USB_Bootloader_wValueLo) != 0u) &&
                            (CY_GET_REG8(USB_Bootloader_wValueLo) ==
                            USB_Bootloader_DEVICE0_DESCR[USB_Bootloader_DEVICE_DESCR_SN_SHIFT]) )
                        {

                            #if defined(USB_Bootloader_ENABLE_IDSN_STRING)
                                /* Read DIE ID and generate string descriptor in RAM */
                                USB_Bootloader_ReadDieID(USB_Bootloader_idSerialNumberStringDescriptor);
                                pStr = USB_Bootloader_idSerialNumberStringDescriptor;
                            #elif defined(USB_Bootloader_ENABLE_FWSN_STRING)
                                if(USB_Bootloader_snStringConfirm != USB_Bootloader_FALSE)
                                {
                                    pStr = USB_Bootloader_fwSerialNumberStringDescriptor;
                                }
                                else
                                {
                                    pStr = (volatile uint8 *)&USB_Bootloader_SN_STRING_DESCRIPTOR[0u];
                                }
                            #else
                                pStr = (volatile uint8 *)&USB_Bootloader_SN_STRING_DESCRIPTOR[0u];
                            #endif  /* defined(USB_Bootloader_ENABLE_IDSN_STRING) */
                        }
                    #endif    /*  USB_Bootloader_ENABLE_SN_STRING */
                    if (*pStr != 0u)
                    {
                        USB_Bootloader_currentTD.count = *pStr;
                        USB_Bootloader_currentTD.pData = pStr;
                        requestHandled  = USB_Bootloader_InitControlRead();
                    }
                }
                #endif /*  USB_Bootloader_ENABLE_STRINGS */
                else
                {
                    requestHandled = USB_Bootloader_DispatchClassRqst();
                }
                break;
            case USB_Bootloader_GET_STATUS:
                switch ((CY_GET_REG8(USB_Bootloader_bmRequestType) & USB_Bootloader_RQST_RCPT_MASK))
                {
                    case USB_Bootloader_RQST_RCPT_EP:
                        USB_Bootloader_currentTD.count = USB_Bootloader_EP_STATUS_LENGTH;
                        USB_Bootloader_tBuffer[0u] = USB_Bootloader_EP[ \
                                        CY_GET_REG8(USB_Bootloader_wIndexLo) & USB_Bootloader_DIR_UNUSED].hwEpState;
                        USB_Bootloader_tBuffer[1u] = 0u;
                        USB_Bootloader_currentTD.pData = &USB_Bootloader_tBuffer[0u];
                        requestHandled  = USB_Bootloader_InitControlRead();
                        break;
                    case USB_Bootloader_RQST_RCPT_DEV:
                        USB_Bootloader_currentTD.count = USB_Bootloader_DEVICE_STATUS_LENGTH;
                        USB_Bootloader_tBuffer[0u] = USB_Bootloader_deviceStatus;
                        USB_Bootloader_tBuffer[1u] = 0u;
                        USB_Bootloader_currentTD.pData = &USB_Bootloader_tBuffer[0u];
                        requestHandled  = USB_Bootloader_InitControlRead();
                        break;
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
            case USB_Bootloader_GET_CONFIGURATION:
                USB_Bootloader_currentTD.count = 1u;
                USB_Bootloader_currentTD.pData = (volatile uint8 *)&USB_Bootloader_configuration;
                requestHandled  = USB_Bootloader_InitControlRead();
                break;
            case USB_Bootloader_GET_INTERFACE:
                USB_Bootloader_currentTD.count = 1u;
                USB_Bootloader_currentTD.pData = (volatile uint8 *)&USB_Bootloader_interfaceSetting[ \
                                                                            CY_GET_REG8(USB_Bootloader_wIndexLo)];
                requestHandled  = USB_Bootloader_InitControlRead();
                break;
            default: /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    else {
        /* Control Write */
        switch (CY_GET_REG8(USB_Bootloader_bRequest))
        {
            case USB_Bootloader_SET_ADDRESS:
                USB_Bootloader_deviceAddress = CY_GET_REG8(USB_Bootloader_wValueLo);
                requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                break;
            case USB_Bootloader_SET_CONFIGURATION:
                configurationN = CY_GET_REG8(USB_Bootloader_wValueLo);
                if(configurationN > 0u)
                {   /* Verify that configuration descriptor exists */
                    pTmp = USB_Bootloader_GetConfigTablePtr(configurationN - 1u);
                }
                /* Responds with a Request Error when configuration number is invalid */
                if (((configurationN > 0u) && (pTmp != NULL)) || (configurationN == 0u))
                {
                    /* Set new configuration if it has been changed */
                    if(configurationN != USB_Bootloader_configuration)
                    {
                        USB_Bootloader_configuration = configurationN;
                        USB_Bootloader_configurationChanged = USB_Bootloader_TRUE;
                        USB_Bootloader_Config(USB_Bootloader_TRUE);
                    }
                    requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                }
                break;
            case USB_Bootloader_SET_INTERFACE:
                if (USB_Bootloader_ValidateAlternateSetting() != 0u)
                {
                    interfaceNumber = CY_GET_REG8(USB_Bootloader_wIndexLo);
                    USB_Bootloader_interfaceNumber = interfaceNumber;
                    USB_Bootloader_configurationChanged = USB_Bootloader_TRUE;
                    #if ((USB_Bootloader_EP_MA == USB_Bootloader__MA_DYNAMIC) && \
                         (USB_Bootloader_EP_MM == USB_Bootloader__EP_MANUAL) )
                        USB_Bootloader_Config(USB_Bootloader_FALSE);
                    #else
                        USB_Bootloader_ConfigAltChanged();
                    #endif /*  (USB_Bootloader_EP_MA == USB_Bootloader__MA_DYNAMIC) */
                    /* Update handled Alt setting changes status */
                    USB_Bootloader_interfaceSetting_last[interfaceNumber] =
                         USB_Bootloader_interfaceSetting[interfaceNumber];
                    requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                }
                break;
            case USB_Bootloader_CLEAR_FEATURE:
                switch (CY_GET_REG8(USB_Bootloader_bmRequestType) & USB_Bootloader_RQST_RCPT_MASK)
                {
                    case USB_Bootloader_RQST_RCPT_EP:
                        if (CY_GET_REG8(USB_Bootloader_wValueLo) == USB_Bootloader_ENDPOINT_HALT)
                        {
                            requestHandled = USB_Bootloader_ClearEndpointHalt();
                        }
                        break;
                    case USB_Bootloader_RQST_RCPT_DEV:
                        /* Clear device REMOTE_WAKEUP */
                        if (CY_GET_REG8(USB_Bootloader_wValueLo) == USB_Bootloader_DEVICE_REMOTE_WAKEUP)
                        {
                            USB_Bootloader_deviceStatus &= (uint8)~USB_Bootloader_DEVICE_STATUS_REMOTE_WAKEUP;
                            requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                        }
                        break;
                    case USB_Bootloader_RQST_RCPT_IFC:
                        /* Validate interfaceNumber */
                        if (CY_GET_REG8(USB_Bootloader_wIndexLo) < USB_Bootloader_MAX_INTERFACES_NUMBER)
                        {
                            USB_Bootloader_interfaceStatus[CY_GET_REG8(USB_Bootloader_wIndexLo)] &=
                                                                (uint8)~(CY_GET_REG8(USB_Bootloader_wValueLo));
                            requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                        }
                        break;
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
            case USB_Bootloader_SET_FEATURE:
                switch (CY_GET_REG8(USB_Bootloader_bmRequestType) & USB_Bootloader_RQST_RCPT_MASK)
                {
                    case USB_Bootloader_RQST_RCPT_EP:
                        if (CY_GET_REG8(USB_Bootloader_wValueLo) == USB_Bootloader_ENDPOINT_HALT)
                        {
                            requestHandled = USB_Bootloader_SetEndpointHalt();
                        }
                        break;
                    case USB_Bootloader_RQST_RCPT_DEV:
                        /* Set device REMOTE_WAKEUP */
                        if (CY_GET_REG8(USB_Bootloader_wValueLo) == USB_Bootloader_DEVICE_REMOTE_WAKEUP)
                        {
                            USB_Bootloader_deviceStatus |= USB_Bootloader_DEVICE_STATUS_REMOTE_WAKEUP;
                            requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                        }
                        break;
                    case USB_Bootloader_RQST_RCPT_IFC:
                        /* Validate interfaceNumber */
                        if (CY_GET_REG8(USB_Bootloader_wIndexLo) < USB_Bootloader_MAX_INTERFACES_NUMBER)
                        {
                            USB_Bootloader_interfaceStatus[CY_GET_REG8(USB_Bootloader_wIndexLo)] &=
                                                                (uint8)~(CY_GET_REG8(USB_Bootloader_wValueLo));
                            requestHandled = USB_Bootloader_InitNoDataControlTransfer();
                        }
                        break;
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
            default:    /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    return(requestHandled);
}


#if defined(USB_Bootloader_ENABLE_IDSN_STRING)

    /***************************************************************************
    * Function Name: USB_Bootloader_ReadDieID
    ****************************************************************************
    *
    * Summary:
    *  This routine read Die ID and generate Serial Number string descriptor.
    *
    * Parameters:
    *  descr:  pointer on string descriptor.
    *
    * Return:
    *  None.
    *
    * Reentrant:
    *  No.
    *
    ***************************************************************************/
    void USB_Bootloader_ReadDieID(uint8 descr[]) 
    {
        uint8 i;
        uint8 j = 0u;
        uint8 value;
        const char8 CYCODE hex[16u] = "0123456789ABCDEF";

        /* Check descriptor validation */
        if( descr != NULL)
        {
            descr[0u] = USB_Bootloader_IDSN_DESCR_LENGTH;
            descr[1u] = USB_Bootloader_DESCR_STRING;

            /* fill descriptor */
            for(i = 2u; i < USB_Bootloader_IDSN_DESCR_LENGTH; i += 4u)
            {
                value = CY_GET_XTND_REG8((void CYFAR *)(USB_Bootloader_DIE_ID + j));
                j++;
                descr[i] = (uint8)hex[value >> 4u];
                descr[i + 2u] = (uint8)hex[value & 0x0Fu];
            }
        }
    }

#endif /*  USB_Bootloader_ENABLE_IDSN_STRING */


/*******************************************************************************
* Function Name: USB_Bootloader_ConfigReg
********************************************************************************
*
* Summary:
*  This routine configures hardware registers from the variables.
*  It is called from USB_Bootloader_Config() function and from RestoreConfig
*  after Wakeup.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void USB_Bootloader_ConfigReg(void) 
{
    uint8 ep;
    uint8 i;
    #if(USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO)
        uint8 epType = 0u;
    #endif /*  USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO */

    /* Set the endpoint buffer addresses */
    ep = USB_Bootloader_EP1;
    for (i = 0u; i < 0x80u; i+= 0x10u)
    {
        CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_EP1_CFG_IND + i), USB_Bootloader_ARB_EPX_CFG_DEFAULT);
        #if(USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL)
            /* Enable all Arbiter EP Interrupts : err, buf under, buf over, dma gnt(mode2 only), in buf full */
            CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_EP1_INT_EN_IND + i), USB_Bootloader_ARB_EPX_INT_MASK);
        #endif   /*  USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL */

        if(USB_Bootloader_EP[ep].epMode != USB_Bootloader_MODE_DISABLE)
        {
            if((USB_Bootloader_EP[ep].addr & USB_Bootloader_DIR_IN) != 0u )
            {
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + i), USB_Bootloader_MODE_NAK_IN);
            }
            else
            {
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + i), USB_Bootloader_MODE_NAK_OUT);
                /* Prepare EP type mask for automatic memory allocation */
                #if(USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO)
                    epType |= (uint8)(0x01u << (ep - USB_Bootloader_EP1));
                #endif /*  USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO */
            }
        }
        else
        {
            CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + i), USB_Bootloader_MODE_STALL_DATA_EP);
        }

        #if(USB_Bootloader_EP_MM != USB_Bootloader__EP_DMAAUTO)
            CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CNT0_IND + i),   USB_Bootloader_EP[ep].bufferSize >> 8u);
            CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CNT1_IND + i),   USB_Bootloader_EP[ep].bufferSize & 0xFFu);

            CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_RA_IND + i),     USB_Bootloader_EP[ep].buffOffset & 0xFFu);
            CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_RA_MSB_IND + i), USB_Bootloader_EP[ep].buffOffset >> 8u);
            CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_WA_IND + i),     USB_Bootloader_EP[ep].buffOffset & 0xFFu);
            CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_WA_MSB_IND + i), USB_Bootloader_EP[ep].buffOffset >> 8u);
        #endif /*  USB_Bootloader_EP_MM != USB_Bootloader__EP_DMAAUTO */

        ep++;
    }

    #if(USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO)
         /* BUF_SIZE depend on DMA_THRESS value: 55-32 bytes  44-16 bytes 33-8 bytes 22-4 bytes 11-2 bytes */
        USB_Bootloader_BUF_SIZE_REG = USB_Bootloader_DMA_BUF_SIZE;
        USB_Bootloader_DMA_THRES_REG = USB_Bootloader_DMA_BYTES_PER_BURST;   /* DMA burst threshold */
        USB_Bootloader_DMA_THRES_MSB_REG = 0u;
        USB_Bootloader_EP_ACTIVE_REG = USB_Bootloader_ARB_INT_MASK;
        USB_Bootloader_EP_TYPE_REG = epType;
        /* Cfg_cmp bit set to 1 once configuration is complete. */
        USB_Bootloader_ARB_CFG_REG = USB_Bootloader_ARB_CFG_AUTO_DMA | USB_Bootloader_ARB_CFG_AUTO_MEM |
                                       USB_Bootloader_ARB_CFG_CFG_CPM;
        /* Cfg_cmp bit set to 0 during configuration of PFSUSB Registers. */
        USB_Bootloader_ARB_CFG_REG = USB_Bootloader_ARB_CFG_AUTO_DMA | USB_Bootloader_ARB_CFG_AUTO_MEM;
    #endif /*  USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO */

    CY_SET_REG8(USB_Bootloader_SIE_EP_INT_EN_PTR, 0xFFu);
}


/*******************************************************************************
* Function Name: USB_Bootloader_Config
********************************************************************************
*
* Summary:
*  This routine configures endpoints for the entire configuration by scanning
*  the configuration descriptor.
*
* Parameters:
*  clearAltSetting: It configures the bAlternateSetting 0 for each interface.
*
* Return:
*  None.
*
* USB_Bootloader_interfaceClass - Initialized class array for each interface.
*   It is used for handling Class specific requests depend on interface class.
*   Different classes in multiple Alternate settings does not supported.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void USB_Bootloader_Config(uint8 clearAltSetting) 
{
    uint8 ep;
    uint8 cur_ep;
    uint8 i;
    uint8 epType;
    const uint8 *pDescr;
    #if(USB_Bootloader_EP_MM != USB_Bootloader__EP_DMAAUTO)
        uint16 buffCount = 0u;
    #endif /*  USB_Bootloader_EP_MM != USB_Bootloader__EP_DMAAUTO */

    const T_USB_Bootloader_LUT CYCODE *pTmp;
    const T_USB_Bootloader_EP_SETTINGS_BLOCK CYCODE *pEP;

    /* Clear all of the endpoints */
    for (ep = 0u; ep < USB_Bootloader_MAX_EP; ep++)
    {
        USB_Bootloader_EP[ep].attrib = 0u;
        USB_Bootloader_EP[ep].hwEpState = 0u;
        USB_Bootloader_EP[ep].apiEpState = USB_Bootloader_NO_EVENT_PENDING;
        USB_Bootloader_EP[ep].epToggle = 0u;
        USB_Bootloader_EP[ep].epMode = USB_Bootloader_MODE_DISABLE;
        USB_Bootloader_EP[ep].bufferSize = 0u;
        USB_Bootloader_EP[ep].interface = 0u;

    }

    /* Clear Alternate settings for all interfaces */
    if(clearAltSetting != 0u)
    {
        for (i = 0u; i < USB_Bootloader_MAX_INTERFACES_NUMBER; i++)
        {
            USB_Bootloader_interfaceSetting[i] = 0x00u;
            USB_Bootloader_interfaceSetting_last[i] = 0x00u;
        }
    }

    /* Init Endpoints and Device Status if configured */
    if(USB_Bootloader_configuration > 0u)
    {
        pTmp = USB_Bootloader_GetConfigTablePtr(USB_Bootloader_configuration - 1u);
        /* Set Power status for current configuration */
        pDescr = (const uint8 *)pTmp->p_list;
        if((pDescr[USB_Bootloader_CONFIG_DESCR_ATTRIB] & USB_Bootloader_CONFIG_DESCR_ATTRIB_SELF_POWERED) != 0u)
        {
            USB_Bootloader_deviceStatus |=  USB_Bootloader_DEVICE_STATUS_SELF_POWERED;
        }
        else
        {
            USB_Bootloader_deviceStatus &=  (uint8)~USB_Bootloader_DEVICE_STATUS_SELF_POWERED;
        }
        /* Move to next element */
        pTmp = &pTmp[1u];
        ep = pTmp->c;  /* For this table, c is the number of endpoints configurations  */

        #if ((USB_Bootloader_EP_MA == USB_Bootloader__MA_DYNAMIC) && \
             (USB_Bootloader_EP_MM == USB_Bootloader__EP_MANUAL) )
            /* Configure for dynamic EP memory allocation */
            /* p_list points the endpoint setting table. */
            pEP = (T_USB_Bootloader_EP_SETTINGS_BLOCK *) pTmp->p_list;
            for (i = 0u; i < ep; i++)
            {
                /* Compare current Alternate setting with EP Alt */
                if(USB_Bootloader_interfaceSetting[pEP->interface] == pEP->altSetting)
                {
                    cur_ep = pEP->addr & USB_Bootloader_DIR_UNUSED;
                    epType = pEP->attributes & USB_Bootloader_EP_TYPE_MASK;
                    if (pEP->addr & USB_Bootloader_DIR_IN)
                    {
                        /* IN Endpoint */
                        USB_Bootloader_EP[cur_ep].apiEpState = USB_Bootloader_EVENT_PENDING;
                        USB_Bootloader_EP[cur_ep].epMode = (epType == USB_Bootloader_EP_TYPE_ISOC) ?
                                                        USB_Bootloader_MODE_ISO_IN : USB_Bootloader_MODE_ACK_IN;
                        #if defined(USB_Bootloader_ENABLE_CDC_CLASS)
                            if(((pEP->bMisc == USB_Bootloader_CLASS_CDC_DATA) ||
                                (pEP->bMisc == USB_Bootloader_CLASS_CDC)) &&
                                (epType != USB_Bootloader_EP_TYPE_INT))
                            {
                                USB_Bootloader_cdc_data_in_ep = cur_ep;
                            }
                        #endif  /*  USB_Bootloader_ENABLE_CDC_CLASS*/
                        #if ( defined(USB_Bootloader_ENABLE_MIDI_STREAMING) && \
                                             (USB_Bootloader_MIDI_IN_BUFF_SIZE > 0) )
                            if((pEP->bMisc == USB_Bootloader_CLASS_AUDIO) &&
                               (epType == USB_Bootloader_EP_TYPE_BULK))
                            {
                                USB_Bootloader_midi_in_ep = cur_ep;
                            }
                        #endif  /*  USB_Bootloader_ENABLE_MIDI_STREAMING*/
                    }
                    else
                    {
                        /* OUT Endpoint */
                        USB_Bootloader_EP[cur_ep].apiEpState = USB_Bootloader_NO_EVENT_PENDING;
                        USB_Bootloader_EP[cur_ep].epMode = (epType == USB_Bootloader_EP_TYPE_ISOC) ?
                                                    USB_Bootloader_MODE_ISO_OUT : USB_Bootloader_MODE_ACK_OUT;
                        #if defined(USB_Bootloader_ENABLE_CDC_CLASS)
                            if(((pEP->bMisc == USB_Bootloader_CLASS_CDC_DATA) ||
                                (pEP->bMisc == USB_Bootloader_CLASS_CDC)) &&
                                (epType != USB_Bootloader_EP_TYPE_INT))
                            {
                                USB_Bootloader_cdc_data_out_ep = cur_ep;
                            }
                        #endif  /*  USB_Bootloader_ENABLE_CDC_CLASS*/
                        #if ( defined(USB_Bootloader_ENABLE_MIDI_STREAMING) && \
                                     (USB_Bootloader_MIDI_OUT_BUFF_SIZE > 0) )
                            if((pEP->bMisc == USB_Bootloader_CLASS_AUDIO) &&
                               (epType == USB_Bootloader_EP_TYPE_BULK))
                            {
                                USB_Bootloader_midi_out_ep = cur_ep;
                            }
                        #endif  /*  USB_Bootloader_ENABLE_MIDI_STREAMING*/
                    }
                    USB_Bootloader_EP[cur_ep].bufferSize = pEP->bufferSize;
                    USB_Bootloader_EP[cur_ep].addr = pEP->addr;
                    USB_Bootloader_EP[cur_ep].attrib = pEP->attributes;
                }
                pEP = &pEP[1u];
            }
        #else /* Configure for static EP memory allocation  */
            for (i = USB_Bootloader_EP1; i < USB_Bootloader_MAX_EP; i++)
            {
                /* p_list points the endpoint setting table. */
                pEP = (const T_USB_Bootloader_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
                /* Find max length for each EP and select it (length could be different in different Alt settings) */
                /* but other settings should be correct with regards to Interface alt Setting */
                for (cur_ep = 0u; cur_ep < ep; cur_ep++)
                {
                    /* EP count is equal to EP # in table and we found larger EP length than have before*/
                    if(i == (pEP->addr & USB_Bootloader_DIR_UNUSED))
                    {
                        if(USB_Bootloader_EP[i].bufferSize < pEP->bufferSize)
                        {
                            USB_Bootloader_EP[i].bufferSize = pEP->bufferSize;
                        }
                        /* Compare current Alternate setting with EP Alt*/
                        if(USB_Bootloader_interfaceSetting[pEP->interface] == pEP->altSetting)
                        {
                            epType = pEP->attributes & USB_Bootloader_EP_TYPE_MASK;
                            if ((pEP->addr & USB_Bootloader_DIR_IN) != 0u)
                            {
                                /* IN Endpoint */
                                USB_Bootloader_EP[i].apiEpState = USB_Bootloader_EVENT_PENDING;
                                USB_Bootloader_EP[i].epMode = (epType == USB_Bootloader_EP_TYPE_ISOC) ?
                                                        USB_Bootloader_MODE_ISO_IN : USB_Bootloader_MODE_ACK_IN;
                                /* Find and initialize CDC IN endpoint number */
                                #if defined(USB_Bootloader_ENABLE_CDC_CLASS)
                                    if(((pEP->bMisc == USB_Bootloader_CLASS_CDC_DATA) ||
                                        (pEP->bMisc == USB_Bootloader_CLASS_CDC)) &&
                                        (epType != USB_Bootloader_EP_TYPE_INT))
                                    {
                                        USB_Bootloader_cdc_data_in_ep = i;
                                    }
                                #endif  /*  USB_Bootloader_ENABLE_CDC_CLASS*/
                                #if ( defined(USB_Bootloader_ENABLE_MIDI_STREAMING) && \
                                             (USB_Bootloader_MIDI_IN_BUFF_SIZE > 0) )
                                    if((pEP->bMisc == USB_Bootloader_CLASS_AUDIO) &&
                                       (epType == USB_Bootloader_EP_TYPE_BULK))
                                    {
                                        USB_Bootloader_midi_in_ep = i;
                                    }
                                #endif  /*  USB_Bootloader_ENABLE_MIDI_STREAMING*/
                            }
                            else
                            {
                                /* OUT Endpoint */
                                USB_Bootloader_EP[i].apiEpState = USB_Bootloader_NO_EVENT_PENDING;
                                USB_Bootloader_EP[i].epMode = (epType == USB_Bootloader_EP_TYPE_ISOC) ?
                                                    USB_Bootloader_MODE_ISO_OUT : USB_Bootloader_MODE_ACK_OUT;
                                /* Find and initialize CDC IN endpoint number */
                                #if defined(USB_Bootloader_ENABLE_CDC_CLASS)
                                    if(((pEP->bMisc == USB_Bootloader_CLASS_CDC_DATA) ||
                                        (pEP->bMisc == USB_Bootloader_CLASS_CDC)) &&
                                        (epType != USB_Bootloader_EP_TYPE_INT))
                                    {
                                        USB_Bootloader_cdc_data_out_ep = i;
                                    }
                                #endif  /*  USB_Bootloader_ENABLE_CDC_CLASS*/
                                #if ( defined(USB_Bootloader_ENABLE_MIDI_STREAMING) && \
                                             (USB_Bootloader_MIDI_OUT_BUFF_SIZE > 0) )
                                    if((pEP->bMisc == USB_Bootloader_CLASS_AUDIO) &&
                                       (epType == USB_Bootloader_EP_TYPE_BULK))
                                    {
                                        USB_Bootloader_midi_out_ep = i;
                                    }
                                #endif  /*  USB_Bootloader_ENABLE_MIDI_STREAMING*/
                            }
                            USB_Bootloader_EP[i].addr = pEP->addr;
                            USB_Bootloader_EP[i].attrib = pEP->attributes;

                            #if(USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO)
                                break;      /* use first EP setting in Auto memory managment */
                            #endif /*  USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO */
                        }
                    }
                    pEP = &pEP[1u];
                }
            }
        #endif /*  (USB_Bootloader_EP_MA == USB_Bootloader__MA_DYNAMIC) */

        /* Init class array for each interface and interface number for each EP.
        *  It is used for handling Class specific requests directed to either an
        *  interface or the endpoint.
        */
        /* p_list points the endpoint setting table. */
        pEP = (const T_USB_Bootloader_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
        for (i = 0u; i < ep; i++)
        {
            /* Configure interface number for each EP*/
            USB_Bootloader_EP[pEP->addr & USB_Bootloader_DIR_UNUSED].interface = pEP->interface;
            pEP = &pEP[1u];
        }
        /* Init pointer on interface class table*/
        USB_Bootloader_interfaceClass = USB_Bootloader_GetInterfaceClassTablePtr();
        /* Set the endpoint buffer addresses */

        #if(USB_Bootloader_EP_MM != USB_Bootloader__EP_DMAAUTO)
            for (ep = USB_Bootloader_EP1; ep < USB_Bootloader_MAX_EP; ep++)
            {
                USB_Bootloader_EP[ep].buffOffset = buffCount;
                 buffCount += USB_Bootloader_EP[ep].bufferSize;
            }
        #endif /*  USB_Bootloader_EP_MM != USB_Bootloader__EP_DMAAUTO */

        /* Configure hardware registers */
        USB_Bootloader_ConfigReg();
    } /* USB_Bootloader_configuration > 0 */
}


/*******************************************************************************
* Function Name: USB_Bootloader_ConfigAltChanged
********************************************************************************
*
* Summary:
*  This routine update configuration for the required endpoints only.
*  It is called after SET_INTERFACE request when Static memory allocation used.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void USB_Bootloader_ConfigAltChanged(void) 
{
    uint8 ep;
    uint8 cur_ep;
    uint8 i;
    uint8 epType;
    uint8 ri;

    const T_USB_Bootloader_LUT CYCODE *pTmp;
    const T_USB_Bootloader_EP_SETTINGS_BLOCK CYCODE *pEP;


    /* Init Endpoints and Device Status if configured */
    if(USB_Bootloader_configuration > 0u)
    {
        pTmp = USB_Bootloader_GetConfigTablePtr(USB_Bootloader_configuration - 1u);
        pTmp = &pTmp[1u];
        ep = pTmp->c;  /* For this table, c is the number of endpoints configurations  */

        /* Do not touch EP which doesn't need reconfiguration */
        /* When Alt setting changed, the only required endpoints need to be reconfigured */
        /* p_list points the endpoint setting table. */
        pEP = (const T_USB_Bootloader_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
        for (i = 0u; i < ep; i++)
        {
            /*If Alt setting changed and new is same with EP Alt */
            if((USB_Bootloader_interfaceSetting[pEP->interface] !=
                USB_Bootloader_interfaceSetting_last[pEP->interface] ) &&
               (USB_Bootloader_interfaceSetting[pEP->interface] == pEP->altSetting) &&
               (pEP->interface == CY_GET_REG8(USB_Bootloader_wIndexLo)))
            {
                cur_ep = pEP->addr & USB_Bootloader_DIR_UNUSED;
                ri = ((cur_ep - USB_Bootloader_EP1) << USB_Bootloader_EPX_CNTX_ADDR_SHIFT);
                epType = pEP->attributes & USB_Bootloader_EP_TYPE_MASK;
                if ((pEP->addr & USB_Bootloader_DIR_IN) != 0u)
                {
                    /* IN Endpoint */
                    USB_Bootloader_EP[cur_ep].apiEpState = USB_Bootloader_EVENT_PENDING;
                    USB_Bootloader_EP[cur_ep].epMode = (epType == USB_Bootloader_EP_TYPE_ISOC) ?
                                                USB_Bootloader_MODE_ISO_IN : USB_Bootloader_MODE_ACK_IN;
                }
                else
                {
                    /* OUT Endpoint */
                    USB_Bootloader_EP[cur_ep].apiEpState = USB_Bootloader_NO_EVENT_PENDING;
                    USB_Bootloader_EP[cur_ep].epMode = (epType == USB_Bootloader_EP_TYPE_ISOC) ?
                                                USB_Bootloader_MODE_ISO_OUT : USB_Bootloader_MODE_ACK_OUT;
                }
                 /* Change the SIE mode for the selected EP to NAK ALL */
                 CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_NAK_IN_OUT);
                USB_Bootloader_EP[cur_ep].bufferSize = pEP->bufferSize;
                USB_Bootloader_EP[cur_ep].addr = pEP->addr;
                USB_Bootloader_EP[cur_ep].attrib = pEP->attributes;

                /* Clear the data toggle */
                USB_Bootloader_EP[cur_ep].epToggle = 0u;

                /* Dynamic reconfiguration for mode 3 transfer */
            #if(USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO)
                /* In_data_rdy for selected EP should be set to 0 */
                * (reg8 *)(USB_Bootloader_ARB_EP1_CFG_IND + ri) &= (uint8)~USB_Bootloader_ARB_EPX_CFG_IN_DATA_RDY;

                /* write the EP number for which reconfiguration is required */
                USB_Bootloader_DYN_RECONFIG_REG = (cur_ep - USB_Bootloader_EP1) <<
                                                    USB_Bootloader_DYN_RECONFIG_EP_SHIFT;
                /* Set the dyn_config_en bit in dynamic reconfiguration register */
                USB_Bootloader_DYN_RECONFIG_REG |= USB_Bootloader_DYN_RECONFIG_ENABLE;
                /* wait for the dyn_config_rdy bit to set by the block,
                *  this bit will be set to 1 when block is ready for reconfiguration.
                */
                while((USB_Bootloader_DYN_RECONFIG_REG & USB_Bootloader_DYN_RECONFIG_RDY_STS) == 0u)
                {
                    ;
                }
                /* Once dyn_config_rdy bit is set, FW can change the EP configuration. */
                /* Change EP Type with new direction */
                if((pEP->addr & USB_Bootloader_DIR_IN) == 0u)
                {
                    USB_Bootloader_EP_TYPE_REG |= (uint8)(0x01u << (cur_ep - USB_Bootloader_EP1));
                }
                else
                {
                    USB_Bootloader_EP_TYPE_REG &= (uint8)~(uint8)(0x01u << (cur_ep - USB_Bootloader_EP1));
                }
                /* dynamic reconfiguration enable bit cleared, pointers and control/status
                *  signals for the selected EP is cleared/re-initialized on negative edge
                *  of dynamic reconfiguration enable bit).
                */
                USB_Bootloader_DYN_RECONFIG_REG &= (uint8)~USB_Bootloader_DYN_RECONFIG_ENABLE;
                /* The main loop has to re-enable DMA and OUT endpoint*/
            #else
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CNT0_IND + ri),
                                                                USB_Bootloader_EP[cur_ep].bufferSize >> 8u);
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CNT1_IND + ri),
                                                                USB_Bootloader_EP[cur_ep].bufferSize & 0xFFu);
                CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_RA_IND + ri),
                                                                USB_Bootloader_EP[cur_ep].buffOffset & 0xFFu);
                CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_RA_MSB_IND + ri),
                                                                USB_Bootloader_EP[cur_ep].buffOffset >> 8u);
                CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_WA_IND + ri),
                                                                USB_Bootloader_EP[cur_ep].buffOffset & 0xFFu);
                CY_SET_REG8((reg8 *)(USB_Bootloader_ARB_RW1_WA_MSB_IND + ri),
                                                                USB_Bootloader_EP[cur_ep].buffOffset >> 8u);
            #endif /*  USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO */
            }
            /* Get next EP element */
            pEP = &pEP[1u];
        }
    }   /* USB_Bootloader_configuration > 0 */
}


/*******************************************************************************
* Function Name: USB_Bootloader_GetConfigTablePtr
********************************************************************************
*
* Summary:
*  This routine returns a pointer a configuration table entry
*
* Parameters:
*  confIndex:  Configuration Index
*
* Return:
*  Device Descriptor pointer or NULL when descriptor isn't exists.
*
*******************************************************************************/
const T_USB_Bootloader_LUT CYCODE *USB_Bootloader_GetConfigTablePtr(uint8 confIndex)
                                                        
{
    /* Device Table */
    const T_USB_Bootloader_LUT CYCODE *pTmp;

    pTmp = (const T_USB_Bootloader_LUT CYCODE *) USB_Bootloader_TABLE[USB_Bootloader_device].p_list;

    /* The first entry points to the Device Descriptor,
    *  the rest configuration entries.
    *  Set pointer to the first Configuration Descriptor
    */
    pTmp = &pTmp[1u];
    /* For this table, c is the number of configuration descriptors  */
    if(confIndex >= pTmp->c)   /* Verify that required configuration descriptor exists */
    {
        pTmp = (const T_USB_Bootloader_LUT CYCODE *) NULL;
    }
    else
    {
        pTmp = (const T_USB_Bootloader_LUT CYCODE *) pTmp[confIndex].p_list;
    }

    return( pTmp );
}


/*******************************************************************************
* Function Name: USB_Bootloader_GetDeviceTablePtr
********************************************************************************
*
* Summary:
*  This routine returns a pointer to the Device table
*
* Parameters:
*  None.
*
* Return:
*  Device Table pointer
*
*******************************************************************************/
const T_USB_Bootloader_LUT CYCODE *USB_Bootloader_GetDeviceTablePtr(void)
                                                            
{
    /* Device Table */
    return( (const T_USB_Bootloader_LUT CYCODE *) USB_Bootloader_TABLE[USB_Bootloader_device].p_list );
}


/*******************************************************************************
* Function Name: USB_GetInterfaceClassTablePtr
********************************************************************************
*
* Summary:
*  This routine returns Interface Class table pointer, which contains
*  the relation between interface number and interface class.
*
* Parameters:
*  None.
*
* Return:
*  Interface Class table pointer.
*
*******************************************************************************/
const uint8 CYCODE *USB_Bootloader_GetInterfaceClassTablePtr(void)
                                                        
{
    const T_USB_Bootloader_LUT CYCODE *pTmp;
    const uint8 CYCODE *pInterfaceClass;
    uint8 currentInterfacesNum;

    pTmp = USB_Bootloader_GetConfigTablePtr(USB_Bootloader_configuration - 1u);
    if( pTmp != NULL )
    {
        currentInterfacesNum  = ((const uint8 *) pTmp->p_list)[USB_Bootloader_CONFIG_DESCR_NUM_INTERFACES];
        /* Third entry in the LUT starts the Interface Table pointers */
        /* The INTERFACE_CLASS table is located after all interfaces */
        pTmp = &pTmp[currentInterfacesNum + 2u];
        pInterfaceClass = (const uint8 CYCODE *) pTmp->p_list;
    }
    else
    {
        pInterfaceClass = (const uint8 CYCODE *) NULL;
    }

    return( pInterfaceClass );
}


/*******************************************************************************
* Function Name: USB_Bootloader_TerminateEP
********************************************************************************
*
* Summary:
*  This function terminates the specified USBFS endpoint.
*  This function should be used before endpoint reconfiguration.
*
* Parameters:
*  Endpoint number.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void USB_Bootloader_TerminateEP(uint8 ep) 
{
    uint8 ri;

    ep &= USB_Bootloader_DIR_UNUSED;
    ri = ((ep - USB_Bootloader_EP1) << USB_Bootloader_EPX_CNTX_ADDR_SHIFT);

    if ((ep > USB_Bootloader_EP0) && (ep < USB_Bootloader_MAX_EP))
    {
        /* Set the endpoint Halt */
        USB_Bootloader_EP[ep].hwEpState |= (USB_Bootloader_ENDPOINT_STATUS_HALT);

        /* Clear the data toggle */
        USB_Bootloader_EP[ep].epToggle = 0u;
        USB_Bootloader_EP[ep].apiEpState = USB_Bootloader_NO_EVENT_ALLOWED;

        if ((USB_Bootloader_EP[ep].addr & USB_Bootloader_DIR_IN) != 0u)
        {
            /* IN Endpoint */
            CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_NAK_IN);
        }
        else
        {
            /* OUT Endpoint */
            CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_NAK_OUT);
        }
    }
}


/*******************************************************************************
* Function Name: USB_Bootloader_SetEndpointHalt
********************************************************************************
*
* Summary:
*  This routine handles set endpoint halt.
*
* Parameters:
*  None.
*
* Return:
*  requestHandled.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 USB_Bootloader_SetEndpointHalt(void) 
{
    uint8 ep;
    uint8 ri;
    uint8 requestHandled = USB_Bootloader_FALSE;

    /* Set endpoint halt */
    ep = CY_GET_REG8(USB_Bootloader_wIndexLo) & USB_Bootloader_DIR_UNUSED;
    ri = ((ep - USB_Bootloader_EP1) << USB_Bootloader_EPX_CNTX_ADDR_SHIFT);

    if ((ep > USB_Bootloader_EP0) && (ep < USB_Bootloader_MAX_EP))
    {
        /* Set the endpoint Halt */
        USB_Bootloader_EP[ep].hwEpState |= (USB_Bootloader_ENDPOINT_STATUS_HALT);

        /* Clear the data toggle */
        USB_Bootloader_EP[ep].epToggle = 0u;
        USB_Bootloader_EP[ep].apiEpState |= USB_Bootloader_NO_EVENT_ALLOWED;

        if ((USB_Bootloader_EP[ep].addr & USB_Bootloader_DIR_IN) != 0u)
        {
            /* IN Endpoint */
            CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_STALL_DATA_EP |
                                                               USB_Bootloader_MODE_ACK_IN);
        }
        else
        {
            /* OUT Endpoint */
            CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_STALL_DATA_EP |
                                                               USB_Bootloader_MODE_ACK_OUT);
        }
        requestHandled = USB_Bootloader_InitNoDataControlTransfer();
    }

    return(requestHandled);
}


/*******************************************************************************
* Function Name: USB_Bootloader_ClearEndpointHalt
********************************************************************************
*
* Summary:
*  This routine handles clear endpoint halt.
*
* Parameters:
*  None.
*
* Return:
*  requestHandled.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 USB_Bootloader_ClearEndpointHalt(void) 
{
    uint8 ep;
    uint8 ri;
    uint8 requestHandled = USB_Bootloader_FALSE;

    /* Clear endpoint halt */
    ep = CY_GET_REG8(USB_Bootloader_wIndexLo) & USB_Bootloader_DIR_UNUSED;
    ri = ((ep - USB_Bootloader_EP1) << USB_Bootloader_EPX_CNTX_ADDR_SHIFT);

    if ((ep > USB_Bootloader_EP0) && (ep < USB_Bootloader_MAX_EP))
    {
        /* Clear the endpoint Halt */
        USB_Bootloader_EP[ep].hwEpState &= (uint8)~(USB_Bootloader_ENDPOINT_STATUS_HALT);

        /* Clear the data toggle */
        USB_Bootloader_EP[ep].epToggle = 0u;
        /* Clear toggle bit for already armed packet */
        CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CNT0_IND + ri), CY_GET_REG8(
                    (reg8 *)(USB_Bootloader_SIE_EP1_CNT0_IND + ri)) & (uint8)~USB_Bootloader_EPX_CNT_DATA_TOGGLE);
        /* Return API State as it was defined before */
        USB_Bootloader_EP[ep].apiEpState &= (uint8)~USB_Bootloader_NO_EVENT_ALLOWED;

        if ((USB_Bootloader_EP[ep].addr & USB_Bootloader_DIR_IN) != 0u)
        {
            /* IN Endpoint */
            if(USB_Bootloader_EP[ep].apiEpState == USB_Bootloader_IN_BUFFER_EMPTY)
            {       /* Wait for next packet from application */
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_NAK_IN);
            }
            else    /* Continue armed transfer */
            {
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_ACK_IN);
            }
        }
        else
        {
            /* OUT Endpoint */
            if(USB_Bootloader_EP[ep].apiEpState == USB_Bootloader_OUT_BUFFER_FULL)
            {       /* Allow application to read full buffer */
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_NAK_OUT);
            }
            else    /* Mark endpoint as empty, so it will be reloaded */
            {
                CY_SET_REG8((reg8 *)(USB_Bootloader_SIE_EP1_CR0_IND + ri), USB_Bootloader_MODE_ACK_OUT);
            }
        }
        requestHandled = USB_Bootloader_InitNoDataControlTransfer();
    }

    return(requestHandled);
}


/*******************************************************************************
* Function Name: USB_Bootloader_ValidateAlternateSetting
********************************************************************************
*
* Summary:
*  Validates (and records) a SET INTERFACE request.
*
* Parameters:
*  None.
*
* Return:
*  requestHandled.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 USB_Bootloader_ValidateAlternateSetting(void) 
{
    uint8 requestHandled = USB_Bootloader_TRUE;
    uint8 interfaceNum;
    const T_USB_Bootloader_LUT CYCODE *pTmp;
    uint8 currentInterfacesNum;

    interfaceNum = CY_GET_REG8(USB_Bootloader_wIndexLo);
    /* Validate interface setting, stall if invalid. */
    pTmp = USB_Bootloader_GetConfigTablePtr(USB_Bootloader_configuration - 1u);
    currentInterfacesNum  = ((const uint8 *) pTmp->p_list)[USB_Bootloader_CONFIG_DESCR_NUM_INTERFACES];

    if((interfaceNum >= currentInterfacesNum) || (interfaceNum >= USB_Bootloader_MAX_INTERFACES_NUMBER))
    {   /* Wrong interface number */
        requestHandled = USB_Bootloader_FALSE;
    }
    else
    {
        /* Save current Alt setting to find out the difference in Config() function */
        USB_Bootloader_interfaceSetting_last[interfaceNum] = USB_Bootloader_interfaceSetting[interfaceNum];
        USB_Bootloader_interfaceSetting[interfaceNum] = CY_GET_REG8(USB_Bootloader_wValueLo);
    }

    return (requestHandled);
}


/* [] END OF FILE */
