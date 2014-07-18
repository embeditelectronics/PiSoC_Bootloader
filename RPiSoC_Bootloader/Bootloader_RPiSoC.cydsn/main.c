/* ========================================
 *
 * RPiSoC rev C Bootloader
 *
 * ========================================
*/
#include <project.h>

void main()
{
   // PWM_1_Start();
    
    //checks if bootloader pin is low, if it is the bootloader is started
    //otherwise go to the user application (bootloadable)
    if((BL_wait_Read() == 0) || (Bootloader_1_GET_RUN_TYPE == Bootloader_1_START_BTLDR))
	{	
		Bootloader_1_SET_RUN_TYPE(Bootloader_1_START_BTLDR);
	}
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    CyBtldr_Start();

    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
