/*******************************************************************************
* File Name: wait_forever.c  
* Version 2.0
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
#include "wait_forever.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 wait_forever__PORT == 15 && ((wait_forever__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: wait_forever_Write
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
void wait_forever_Write(uint8 value) 
{
    uint8 staticBits = (wait_forever_DR & (uint8)(~wait_forever_MASK));
    wait_forever_DR = staticBits | ((uint8)(value << wait_forever_SHIFT) & wait_forever_MASK);
}


/*******************************************************************************
* Function Name: wait_forever_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  wait_forever_DM_STRONG     Strong Drive 
*  wait_forever_DM_OD_HI      Open Drain, Drives High 
*  wait_forever_DM_OD_LO      Open Drain, Drives Low 
*  wait_forever_DM_RES_UP     Resistive Pull Up 
*  wait_forever_DM_RES_DWN    Resistive Pull Down 
*  wait_forever_DM_RES_UPDWN  Resistive Pull Up/Down 
*  wait_forever_DM_DIG_HIZ    High Impedance Digital 
*  wait_forever_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void wait_forever_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(wait_forever_0, mode);
}


/*******************************************************************************
* Function Name: wait_forever_Read
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
*  Macro wait_forever_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 wait_forever_Read(void) 
{
    return (wait_forever_PS & wait_forever_MASK) >> wait_forever_SHIFT;
}


/*******************************************************************************
* Function Name: wait_forever_ReadDataReg
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
uint8 wait_forever_ReadDataReg(void) 
{
    return (wait_forever_DR & wait_forever_MASK) >> wait_forever_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(wait_forever_INTSTAT) 

    /*******************************************************************************
    * Function Name: wait_forever_ClearInterrupt
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
    uint8 wait_forever_ClearInterrupt(void) 
    {
        return (wait_forever_INTSTAT & wait_forever_MASK) >> wait_forever_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
