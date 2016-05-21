/*******************************************************************************
* File Name: .h
* Version 2.80
*
* Description:
*  This private file provides constants and parameter values for the
*  USBFS Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USB_Bootloader_pvt_H)
#define CY_USBFS_USB_Bootloader_pvt_H


/***************************************
*     Private Variables
***************************************/

/* Generated external references for descriptors*/
extern const uint8 CYCODE USB_Bootloader_DEVICE0_DESCR[18u];
extern const uint8 CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_DESCR[41u];
extern const T_USB_Bootloader_LUT CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_TABLE[1u];
extern const T_USB_Bootloader_EP_SETTINGS_BLOCK CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_EP_SETTINGS_TABLE[2u];
extern const uint8 CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE_CLASS[1u];
extern const T_USB_Bootloader_LUT CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_TABLE[4u];
extern const T_USB_Bootloader_LUT CYCODE USB_Bootloader_DEVICE0_TABLE[2u];
extern const T_USB_Bootloader_LUT CYCODE USB_Bootloader_TABLE[1u];
extern const uint8 CYCODE USB_Bootloader_SN_STRING_DESCRIPTOR[10];
extern const uint8 CYCODE USB_Bootloader_STRING_DESCRIPTORS[157u];
extern T_USB_Bootloader_XFER_STATUS_BLOCK USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_RPT_SCB;
extern uint8 USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF[
            USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF_SIZE];
extern T_USB_Bootloader_XFER_STATUS_BLOCK USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_SCB;
extern uint8 USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[
            USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF_SIZE];
extern const uint8 CYCODE USB_Bootloader_HIDREPORT_DESCRIPTOR1[40u];
extern const T_USB_Bootloader_TD CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_RPT_TABLE[1u];
extern const T_USB_Bootloader_TD CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_TABLE[1u];
extern const T_USB_Bootloader_LUT CYCODE USB_Bootloader_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_TABLE[5u];


extern const uint8 CYCODE USB_Bootloader_MSOS_DESCRIPTOR[USB_Bootloader_MSOS_DESCRIPTOR_LENGTH];
extern const uint8 CYCODE USB_Bootloader_MSOS_CONFIGURATION_DESCR[USB_Bootloader_MSOS_CONF_DESCR_LENGTH];
#if defined(USB_Bootloader_ENABLE_IDSN_STRING)
    extern uint8 USB_Bootloader_idSerialNumberStringDescriptor[USB_Bootloader_IDSN_DESCR_LENGTH];
#endif /* USB_Bootloader_ENABLE_IDSN_STRING */

extern volatile uint8 USB_Bootloader_interfaceNumber;
extern volatile uint8 USB_Bootloader_interfaceSetting[USB_Bootloader_MAX_INTERFACES_NUMBER];
extern volatile uint8 USB_Bootloader_interfaceSetting_last[USB_Bootloader_MAX_INTERFACES_NUMBER];
extern volatile uint8 USB_Bootloader_deviceAddress;
extern volatile uint8 USB_Bootloader_interfaceStatus[USB_Bootloader_MAX_INTERFACES_NUMBER];
extern const uint8 CYCODE *USB_Bootloader_interfaceClass;

extern volatile T_USB_Bootloader_EP_CTL_BLOCK USB_Bootloader_EP[USB_Bootloader_MAX_EP];
extern volatile T_USB_Bootloader_TD USB_Bootloader_currentTD;

#if(USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL)
    extern uint8 USB_Bootloader_DmaChan[USB_Bootloader_MAX_EP];
    extern uint8 USB_Bootloader_DmaTd[USB_Bootloader_MAX_EP];
#endif /*  USB_Bootloader_EP_MM */
#if((USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO) && (USB_Bootloader_EP_DMA_AUTO_OPT == 0u))
    extern uint8 USB_Bootloader_DmaNextTd[USB_Bootloader_MAX_EP];
    extern const uint8 USB_Bootloader_epX_TD_TERMOUT_EN[USB_Bootloader_MAX_EP];
    extern volatile uint16 USB_Bootloader_inLength[USB_Bootloader_MAX_EP];
    extern const uint8 *USB_Bootloader_inDataPointer[USB_Bootloader_MAX_EP];
    extern volatile uint8 USB_Bootloader_inBufFull[USB_Bootloader_MAX_EP];
#endif /*  ((USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO) && (USB_Bootloader_EP_DMA_AUTO_OPT == 0u)) */

extern volatile uint8 USB_Bootloader_ep0Toggle;
extern volatile uint8 USB_Bootloader_lastPacketSize;
extern volatile uint8 USB_Bootloader_ep0Mode;
extern volatile uint8 USB_Bootloader_ep0Count;
extern volatile uint16 USB_Bootloader_transferByteCount;


/***************************************
*     Private Function Prototypes
***************************************/
void  USB_Bootloader_ReInitComponent(void) ;
void  USB_Bootloader_HandleSetup(void) ;
void  USB_Bootloader_HandleIN(void) ;
void  USB_Bootloader_HandleOUT(void) ;
void  USB_Bootloader_LoadEP0(void) ;
uint8 USB_Bootloader_InitControlRead(void) ;
uint8 USB_Bootloader_InitControlWrite(void) ;
void  USB_Bootloader_ControlReadDataStage(void) ;
void  USB_Bootloader_ControlReadStatusStage(void) ;
void  USB_Bootloader_ControlReadPrematureStatus(void)
                                                ;
uint8 USB_Bootloader_InitControlWrite(void) ;
uint8 USB_Bootloader_InitZeroLengthControlTransfer(void)
                                                ;
void  USB_Bootloader_ControlWriteDataStage(void) ;
void  USB_Bootloader_ControlWriteStatusStage(void) ;
void  USB_Bootloader_ControlWritePrematureStatus(void)
                                                ;
uint8 USB_Bootloader_InitNoDataControlTransfer(void) ;
void  USB_Bootloader_NoDataControlStatusStage(void) ;
void  USB_Bootloader_InitializeStatusBlock(void) ;
void  USB_Bootloader_UpdateStatusBlock(uint8 completionCode) ;
uint8 USB_Bootloader_DispatchClassRqst(void) ;

void USB_Bootloader_Config(uint8 clearAltSetting) ;
void USB_Bootloader_ConfigAltChanged(void) ;
void USB_Bootloader_ConfigReg(void) ;

const T_USB_Bootloader_LUT CYCODE *USB_Bootloader_GetConfigTablePtr(uint8 confIndex)
                                                            ;
const T_USB_Bootloader_LUT CYCODE *USB_Bootloader_GetDeviceTablePtr(void)
                                                            ;
const uint8 CYCODE *USB_Bootloader_GetInterfaceClassTablePtr(void)
                                                    ;
uint8 USB_Bootloader_ClearEndpointHalt(void) ;
uint8 USB_Bootloader_SetEndpointHalt(void) ;
uint8 USB_Bootloader_ValidateAlternateSetting(void) ;

void USB_Bootloader_SaveConfig(void) ;
void USB_Bootloader_RestoreConfig(void) ;

#if ((USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO) && (USB_Bootloader_EP_DMA_AUTO_OPT == 0u))
    void USB_Bootloader_LoadNextInEP(uint8 epNumber, uint8 mode) ;
#endif /* (USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO) && (USB_Bootloader_EP_DMA_AUTO_OPT == 0u) */

#if defined(USB_Bootloader_ENABLE_IDSN_STRING)
    void USB_Bootloader_ReadDieID(uint8 descr[]) ;
#endif /* USB_Bootloader_ENABLE_IDSN_STRING */

#if defined(USB_Bootloader_ENABLE_HID_CLASS)
    uint8 USB_Bootloader_DispatchHIDClassRqst(void);
#endif /*  USB_Bootloader_ENABLE_HID_CLASS */
#if defined(USB_Bootloader_ENABLE_AUDIO_CLASS)
    uint8 USB_Bootloader_DispatchAUDIOClassRqst(void);
#endif /*  USB_Bootloader_ENABLE_HID_CLASS */
#if defined(USB_Bootloader_ENABLE_CDC_CLASS)
    uint8 USB_Bootloader_DispatchCDCClassRqst(void);
#endif /*  USB_Bootloader_ENABLE_CDC_CLASS */

CY_ISR_PROTO(USB_Bootloader_EP_0_ISR);
#if(USB_Bootloader_EP1_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_1_ISR);
#endif /*  USB_Bootloader_EP1_ISR_REMOVE */
#if(USB_Bootloader_EP2_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_2_ISR);
#endif /*  USB_Bootloader_EP2_ISR_REMOVE */
#if(USB_Bootloader_EP3_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_3_ISR);
#endif /*  USB_Bootloader_EP3_ISR_REMOVE */
#if(USB_Bootloader_EP4_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_4_ISR);
#endif /*  USB_Bootloader_EP4_ISR_REMOVE */
#if(USB_Bootloader_EP5_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_5_ISR);
#endif /*  USB_Bootloader_EP5_ISR_REMOVE */
#if(USB_Bootloader_EP6_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_6_ISR);
#endif /*  USB_Bootloader_EP6_ISR_REMOVE */
#if(USB_Bootloader_EP7_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_7_ISR);
#endif /*  USB_Bootloader_EP7_ISR_REMOVE */
#if(USB_Bootloader_EP8_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_EP_8_ISR);
#endif /*  USB_Bootloader_EP8_ISR_REMOVE */
CY_ISR_PROTO(USB_Bootloader_BUS_RESET_ISR);
#if(USB_Bootloader_SOF_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_SOF_ISR);
#endif /*  USB_Bootloader_SOF_ISR_REMOVE */
#if(USB_Bootloader_EP_MM != USB_Bootloader__EP_MANUAL)
    CY_ISR_PROTO(USB_Bootloader_ARB_ISR);
#endif /*  USB_Bootloader_EP_MM */
#if(USB_Bootloader_DP_ISR_REMOVE == 0u)
    CY_ISR_PROTO(USB_Bootloader_DP_ISR);
#endif /*  USB_Bootloader_DP_ISR_REMOVE */
#if ((USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO) && (USB_Bootloader_EP_DMA_AUTO_OPT == 0u))
    CY_ISR_PROTO(USB_Bootloader_EP_DMA_DONE_ISR);
#endif /* (USB_Bootloader_EP_MM == USB_Bootloader__EP_DMAAUTO) && (USB_Bootloader_EP_DMA_AUTO_OPT == 0u) */

/***************************************
* Request Handlers
***************************************/

uint8 USB_Bootloader_HandleStandardRqst(void) ;
uint8 USB_Bootloader_DispatchClassRqst(void) ;
uint8 USB_Bootloader_HandleVendorRqst(void) ;


/***************************************
*    HID Internal references
***************************************/

#if defined(USB_Bootloader_ENABLE_HID_CLASS)
    void USB_Bootloader_FindReport(void) ;
    void USB_Bootloader_FindReportDescriptor(void) ;
    void USB_Bootloader_FindHidClassDecriptor(void) ;
#endif /* USB_Bootloader_ENABLE_HID_CLASS */


/***************************************
*    MIDI Internal references
***************************************/

#if defined(USB_Bootloader_ENABLE_MIDI_STREAMING)
    void USB_Bootloader_MIDI_IN_EP_Service(void) ;
#endif /* USB_Bootloader_ENABLE_MIDI_STREAMING */


#endif /* CY_USBFS_USB_Bootloader_pvt_H */


/* [] END OF FILE */
