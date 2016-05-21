/*******************************************************************************
* File Name: BOOT_MODE.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "BOOT_MODE.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 BOOT_MODE__PORT == 15 && ((BOOT_MODE__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: BOOT_MODE_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None
*  
*******************************************************************************/
void BOOT_MODE_Write(uint8 value) 
{
    uint8 staticBits = (BOOT_MODE_DR & (uint8)(~BOOT_MODE_MASK));
    BOOT_MODE_DR = staticBits | ((uint8)(value << BOOT_MODE_SHIFT) & BOOT_MODE_MASK);
}


/*******************************************************************************
* Function Name: BOOT_MODE_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  BOOT_MODE_DM_STRONG     Strong Drive 
*  BOOT_MODE_DM_OD_HI      Open Drain, Drives High 
*  BOOT_MODE_DM_OD_LO      Open Drain, Drives Low 
*  BOOT_MODE_DM_RES_UP     Resistive Pull Up 
*  BOOT_MODE_DM_RES_DWN    Resistive Pull Down 
*  BOOT_MODE_DM_RES_UPDWN  Resistive Pull Up/Down 
*  BOOT_MODE_DM_DIG_HIZ    High Impedance Digital 
*  BOOT_MODE_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void BOOT_MODE_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(BOOT_MODE_0, mode);
}


/*******************************************************************************
* Function Name: BOOT_MODE_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro BOOT_MODE_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 BOOT_MODE_Read(void) 
{
    return (BOOT_MODE_PS & BOOT_MODE_MASK) >> BOOT_MODE_SHIFT;
}


/*******************************************************************************
* Function Name: BOOT_MODE_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 BOOT_MODE_ReadDataReg(void) 
{
    return (BOOT_MODE_DR & BOOT_MODE_MASK) >> BOOT_MODE_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(BOOT_MODE_INTSTAT) 

    /*******************************************************************************
    * Function Name: BOOT_MODE_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 BOOT_MODE_ClearInterrupt(void) 
    {
        return (BOOT_MODE_INTSTAT & BOOT_MODE_MASK) >> BOOT_MODE_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
