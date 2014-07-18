/*******************************************************************************
* File Name: BL_wait.h  
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

#if !defined(CY_PINS_BL_wait_H) /* Pins BL_wait_H */
#define CY_PINS_BL_wait_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "BL_wait_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 BL_wait__PORT == 15 && ((BL_wait__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    BL_wait_Write(uint8 value) ;
void    BL_wait_SetDriveMode(uint8 mode) ;
uint8   BL_wait_ReadDataReg(void) ;
uint8   BL_wait_Read(void) ;
uint8   BL_wait_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define BL_wait_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define BL_wait_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define BL_wait_DM_RES_UP          PIN_DM_RES_UP
#define BL_wait_DM_RES_DWN         PIN_DM_RES_DWN
#define BL_wait_DM_OD_LO           PIN_DM_OD_LO
#define BL_wait_DM_OD_HI           PIN_DM_OD_HI
#define BL_wait_DM_STRONG          PIN_DM_STRONG
#define BL_wait_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define BL_wait_MASK               BL_wait__MASK
#define BL_wait_SHIFT              BL_wait__SHIFT
#define BL_wait_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define BL_wait_PS                     (* (reg8 *) BL_wait__PS)
/* Data Register */
#define BL_wait_DR                     (* (reg8 *) BL_wait__DR)
/* Port Number */
#define BL_wait_PRT_NUM                (* (reg8 *) BL_wait__PRT) 
/* Connect to Analog Globals */                                                  
#define BL_wait_AG                     (* (reg8 *) BL_wait__AG)                       
/* Analog MUX bux enable */
#define BL_wait_AMUX                   (* (reg8 *) BL_wait__AMUX) 
/* Bidirectional Enable */                                                        
#define BL_wait_BIE                    (* (reg8 *) BL_wait__BIE)
/* Bit-mask for Aliased Register Access */
#define BL_wait_BIT_MASK               (* (reg8 *) BL_wait__BIT_MASK)
/* Bypass Enable */
#define BL_wait_BYP                    (* (reg8 *) BL_wait__BYP)
/* Port wide control signals */                                                   
#define BL_wait_CTL                    (* (reg8 *) BL_wait__CTL)
/* Drive Modes */
#define BL_wait_DM0                    (* (reg8 *) BL_wait__DM0) 
#define BL_wait_DM1                    (* (reg8 *) BL_wait__DM1)
#define BL_wait_DM2                    (* (reg8 *) BL_wait__DM2) 
/* Input Buffer Disable Override */
#define BL_wait_INP_DIS                (* (reg8 *) BL_wait__INP_DIS)
/* LCD Common or Segment Drive */
#define BL_wait_LCD_COM_SEG            (* (reg8 *) BL_wait__LCD_COM_SEG)
/* Enable Segment LCD */
#define BL_wait_LCD_EN                 (* (reg8 *) BL_wait__LCD_EN)
/* Slew Rate Control */
#define BL_wait_SLW                    (* (reg8 *) BL_wait__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define BL_wait_PRTDSI__CAPS_SEL       (* (reg8 *) BL_wait__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define BL_wait_PRTDSI__DBL_SYNC_IN    (* (reg8 *) BL_wait__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define BL_wait_PRTDSI__OE_SEL0        (* (reg8 *) BL_wait__PRTDSI__OE_SEL0) 
#define BL_wait_PRTDSI__OE_SEL1        (* (reg8 *) BL_wait__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define BL_wait_PRTDSI__OUT_SEL0       (* (reg8 *) BL_wait__PRTDSI__OUT_SEL0) 
#define BL_wait_PRTDSI__OUT_SEL1       (* (reg8 *) BL_wait__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define BL_wait_PRTDSI__SYNC_OUT       (* (reg8 *) BL_wait__PRTDSI__SYNC_OUT) 


#if defined(BL_wait__INTSTAT)  /* Interrupt Registers */

    #define BL_wait_INTSTAT                (* (reg8 *) BL_wait__INTSTAT)
    #define BL_wait_SNAP                   (* (reg8 *) BL_wait__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_BL_wait_H */


/* [] END OF FILE */
