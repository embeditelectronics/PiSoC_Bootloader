/*******************************************************************************
* File Name: wait_forever.h  
* Version 2.0
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_wait_forever_H) /* Pins wait_forever_H */
#define CY_PINS_wait_forever_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "wait_forever_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 wait_forever__PORT == 15 && ((wait_forever__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    wait_forever_Write(uint8 value) ;
void    wait_forever_SetDriveMode(uint8 mode) ;
uint8   wait_forever_ReadDataReg(void) ;
uint8   wait_forever_Read(void) ;
uint8   wait_forever_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define wait_forever_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define wait_forever_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define wait_forever_DM_RES_UP          PIN_DM_RES_UP
#define wait_forever_DM_RES_DWN         PIN_DM_RES_DWN
#define wait_forever_DM_OD_LO           PIN_DM_OD_LO
#define wait_forever_DM_OD_HI           PIN_DM_OD_HI
#define wait_forever_DM_STRONG          PIN_DM_STRONG
#define wait_forever_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define wait_forever_MASK               wait_forever__MASK
#define wait_forever_SHIFT              wait_forever__SHIFT
#define wait_forever_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define wait_forever_PS                     (* (reg8 *) wait_forever__PS)
/* Data Register */
#define wait_forever_DR                     (* (reg8 *) wait_forever__DR)
/* Port Number */
#define wait_forever_PRT_NUM                (* (reg8 *) wait_forever__PRT) 
/* Connect to Analog Globals */                                                  
#define wait_forever_AG                     (* (reg8 *) wait_forever__AG)                       
/* Analog MUX bux enable */
#define wait_forever_AMUX                   (* (reg8 *) wait_forever__AMUX) 
/* Bidirectional Enable */                                                        
#define wait_forever_BIE                    (* (reg8 *) wait_forever__BIE)
/* Bit-mask for Aliased Register Access */
#define wait_forever_BIT_MASK               (* (reg8 *) wait_forever__BIT_MASK)
/* Bypass Enable */
#define wait_forever_BYP                    (* (reg8 *) wait_forever__BYP)
/* Port wide control signals */                                                   
#define wait_forever_CTL                    (* (reg8 *) wait_forever__CTL)
/* Drive Modes */
#define wait_forever_DM0                    (* (reg8 *) wait_forever__DM0) 
#define wait_forever_DM1                    (* (reg8 *) wait_forever__DM1)
#define wait_forever_DM2                    (* (reg8 *) wait_forever__DM2) 
/* Input Buffer Disable Override */
#define wait_forever_INP_DIS                (* (reg8 *) wait_forever__INP_DIS)
/* LCD Common or Segment Drive */
#define wait_forever_LCD_COM_SEG            (* (reg8 *) wait_forever__LCD_COM_SEG)
/* Enable Segment LCD */
#define wait_forever_LCD_EN                 (* (reg8 *) wait_forever__LCD_EN)
/* Slew Rate Control */
#define wait_forever_SLW                    (* (reg8 *) wait_forever__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define wait_forever_PRTDSI__CAPS_SEL       (* (reg8 *) wait_forever__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define wait_forever_PRTDSI__DBL_SYNC_IN    (* (reg8 *) wait_forever__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define wait_forever_PRTDSI__OE_SEL0        (* (reg8 *) wait_forever__PRTDSI__OE_SEL0) 
#define wait_forever_PRTDSI__OE_SEL1        (* (reg8 *) wait_forever__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define wait_forever_PRTDSI__OUT_SEL0       (* (reg8 *) wait_forever__PRTDSI__OUT_SEL0) 
#define wait_forever_PRTDSI__OUT_SEL1       (* (reg8 *) wait_forever__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define wait_forever_PRTDSI__SYNC_OUT       (* (reg8 *) wait_forever__PRTDSI__SYNC_OUT) 


#if defined(wait_forever__INTSTAT)  /* Interrupt Registers */

    #define wait_forever_INTSTAT                (* (reg8 *) wait_forever__INTSTAT)
    #define wait_forever_SNAP                   (* (reg8 *) wait_forever__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_wait_forever_H */


/* [] END OF FILE */
