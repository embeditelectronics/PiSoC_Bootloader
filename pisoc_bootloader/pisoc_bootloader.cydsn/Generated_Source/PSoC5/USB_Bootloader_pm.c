/*******************************************************************************
* File Name: USB_Bootloader_pm.c
* Version 2.80
*
* Description:
*  This file provides Suspend/Resume APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "project.h"
#include "USB_Bootloader.h"
#include "USB_Bootloader_pvt.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START PM_CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


/***************************************
* Local data allocation
***************************************/

static USB_Bootloader_BACKUP_STRUCT  USB_Bootloader_backup;


#if(USB_Bootloader_DP_ISR_REMOVE == 0u)

    /*******************************************************************************
    * Function Name: USB_Bootloader_DP_Interrupt
    ********************************************************************************
    *
    * Summary:
    *  This Interrupt Service Routine handles DP pin changes for wake-up from
    *  the sleep mode.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(USB_Bootloader_DP_ISR)
    {
        #ifdef USB_Bootloader_DP_ISR_ENTRY_CALLBACK
            USB_Bootloader_DP_ISR_EntryCallback();
        #endif /* USB_Bootloader_DP_ISR_ENTRY_CALLBACK */

        /* `#START DP_USER_CODE` Place your code here */

        /* `#END` */

        /* Clears active interrupt */
        CY_GET_REG8(USB_Bootloader_DP_INTSTAT_PTR);

        #ifdef USB_Bootloader_DP_ISR_EXIT_CALLBACK
            USB_Bootloader_DP_ISR_ExitCallback();
        #endif /* USB_Bootloader_DP_ISR_EXIT_CALLBACK */
    }

#endif /* (USB_Bootloader_DP_ISR_REMOVE == 0u) */


/*******************************************************************************
* Function Name: USB_Bootloader_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
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
void USB_Bootloader_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: USB_Bootloader_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
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
void USB_Bootloader_RestoreConfig(void) 
{
    if(USB_Bootloader_configuration != 0u)
    {
        USB_Bootloader_ConfigReg();
    }
}


/*******************************************************************************
* Function Name: USB_Bootloader_Suspend
********************************************************************************
*
* Summary:
*  This function disables the USBFS block and prepares for power down mode.
*
* Parameters:
*  None.
*
* Return:
*   None.
*
* Global variables:
*  USB_Bootloader_backup.enable:  modified.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void USB_Bootloader_Suspend(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    if((CY_GET_REG8(USB_Bootloader_CR0_PTR) & USB_Bootloader_CR0_ENABLE) != 0u)
    {   /* USB block is enabled */
        USB_Bootloader_backup.enableState = 1u;

        #if(USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL)
            USB_Bootloader_Stop_DMA(USB_Bootloader_MAX_EP);     /* Stop all DMAs */
        #endif   /*  USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL */

        /* Ensure USB transmit enable is low (USB_USBIO_CR0.ten). - Manual Transmission - Disabled */
        USB_Bootloader_USBIO_CR0_REG &= (uint8)~USB_Bootloader_USBIO_CR0_TEN;
        CyDelayUs(0u);  /*~50ns delay */

        /* Disable the USBIO by asserting PM.USB_CR0.fsusbio_pd_n(Inverted) and pd_pullup_hv(Inverted) high. */
        USB_Bootloader_PM_USB_CR0_REG &=
                                (uint8)~(USB_Bootloader_PM_USB_CR0_PD_N | USB_Bootloader_PM_USB_CR0_PD_PULLUP_N);

        /* Disable the SIE */
        USB_Bootloader_CR0_REG &= (uint8)~USB_Bootloader_CR0_ENABLE;

        CyDelayUs(0u);  /* ~50ns delay */
        /* Store mode and Disable VRegulator*/
        USB_Bootloader_backup.mode = USB_Bootloader_CR1_REG & USB_Bootloader_CR1_REG_ENABLE;
        USB_Bootloader_CR1_REG &= (uint8)~USB_Bootloader_CR1_REG_ENABLE;

        CyDelayUs(1u);  /* 0.5 us min delay */
        /* Disable the USBIO reference by setting PM.USB_CR0.fsusbio_ref_en.*/
        USB_Bootloader_PM_USB_CR0_REG &= (uint8)~USB_Bootloader_PM_USB_CR0_REF_EN;

        /* Switch DP and DM terminals to GPIO mode and disconnect 1.5k pullup*/
        USB_Bootloader_USBIO_CR1_REG |= USB_Bootloader_USBIO_CR1_IOMODE;

        /* Disable USB in ACT PM */
        USB_Bootloader_PM_ACT_CFG_REG &= (uint8)~USB_Bootloader_PM_ACT_EN_FSUSB;
        /* Disable USB block for Standby Power Mode */
        USB_Bootloader_PM_STBY_CFG_REG &= (uint8)~USB_Bootloader_PM_STBY_EN_FSUSB;
        CyDelayUs(1u); /* min  0.5us delay required */

    }
    else
    {
        USB_Bootloader_backup.enableState = 0u;
    }

    CyExitCriticalSection(enableInterrupts);

    /* Set the DP Interrupt for wake-up from sleep mode. */
    #if(USB_Bootloader_DP_ISR_REMOVE == 0u)
        (void) CyIntSetVector(USB_Bootloader_DP_INTC_VECT_NUM, &USB_Bootloader_DP_ISR);
        CyIntSetPriority(USB_Bootloader_DP_INTC_VECT_NUM, USB_Bootloader_DP_INTC_PRIOR);
        CyIntClearPending(USB_Bootloader_DP_INTC_VECT_NUM);
        CyIntEnable(USB_Bootloader_DP_INTC_VECT_NUM);
    #endif /* (USB_Bootloader_DP_ISR_REMOVE == 0u) */
}


/*******************************************************************************
* Function Name: USB_Bootloader_Resume
********************************************************************************
*
* Summary:
*  This function enables the USBFS block after power down mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  USB_Bootloader_backup - checked.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void USB_Bootloader_Resume(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    if(USB_Bootloader_backup.enableState != 0u)
    {
        #if(USB_Bootloader_DP_ISR_REMOVE == 0u)
            CyIntDisable(USB_Bootloader_DP_INTC_VECT_NUM);
        #endif /*  USB_Bootloader_DP_ISR_REMOVE */

        /* Enable USB block */
        USB_Bootloader_PM_ACT_CFG_REG |= USB_Bootloader_PM_ACT_EN_FSUSB;
        /* Enable USB block for Standby Power Mode */
        USB_Bootloader_PM_STBY_CFG_REG |= USB_Bootloader_PM_STBY_EN_FSUSB;
        /* Enable core clock */
        USB_Bootloader_USB_CLK_EN_REG |= USB_Bootloader_USB_CLK_ENABLE;

        /* Enable the USBIO reference by setting PM.USB_CR0.fsusbio_ref_en.*/
        USB_Bootloader_PM_USB_CR0_REG |= USB_Bootloader_PM_USB_CR0_REF_EN;
        /* The reference will be available ~40us after power restored */
        CyDelayUs(40u);
        /* Return VRegulator*/
        USB_Bootloader_CR1_REG |= USB_Bootloader_backup.mode;
        CyDelayUs(0u);  /*~50ns delay */
        /* Enable USBIO */
        USB_Bootloader_PM_USB_CR0_REG |= USB_Bootloader_PM_USB_CR0_PD_N;
        CyDelayUs(2u);
        /* Set the USBIO pull-up enable */
        USB_Bootloader_PM_USB_CR0_REG |= USB_Bootloader_PM_USB_CR0_PD_PULLUP_N;

        /* Re-init Arbiter configuration for DMA transfers */
        #if(USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL)
            /* Usb arb interrupt enable */
            USB_Bootloader_ARB_INT_EN_REG = USB_Bootloader_ARB_INT_MASK;
            #if(USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAMANUAL)
                USB_Bootloader_ARB_CFG_REG = USB_Bootloader_ARB_CFG_MANUAL_DMA;
            #endif   /*  USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAMANUAL */
            #if(USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO)
                /*Set cfg cmplt this rises DMA request when the full configuration is done */
                USB_Bootloader_ARB_CFG_REG = USB_Bootloader_ARB_CFG_AUTO_DMA | USB_Bootloader_ARB_CFG_AUTO_MEM;
            #endif   /*  USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO */
        #endif   /*  USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL */

        /* STALL_IN_OUT */
        CY_SET_REG8(USB_Bootloader_EP0_CR_PTR, USB_Bootloader_MODE_STALL_IN_OUT);
        /* Enable the SIE with a last address */
        USB_Bootloader_CR0_REG |= USB_Bootloader_CR0_ENABLE;
        CyDelayCycles(1u);
        /* Finally, Enable d+ pullup and select iomode to USB mode*/
        CY_SET_REG8(USB_Bootloader_USBIO_CR1_PTR, USB_Bootloader_USBIO_CR1_USBPUEN);

        /* Restore USB register settings */
        USB_Bootloader_RestoreConfig();
    }

    CyExitCriticalSection(enableInterrupts);
}


/* [] END OF FILE */
