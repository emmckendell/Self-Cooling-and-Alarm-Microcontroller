#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Main.h"
#include "I2C_Support.h"
#include "Setup_Fan_Temp.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"
#include "utils.h"

extern char setup_fan_set_text[];
unsigned char setup_fan_set_temp;

char Select_Fan_Temp_Field;
char In_Fan_Temp_Setup;

extern char *txt;
extern char found;
extern unsigned char fan_set_temp;
extern short Nec_ok;
extern char duty_cycle;

void Setup_Temp_Fan(void)
{
    In_Fan_Temp_Setup = 1;
    setup_fan_set_temp = fan_set_temp;
    Initialize_Setup_Fan_Screen();                  // Initialize setup heater screen
    Update_Setup_Fan_Screen();                      // Update the screen with latest info

    while (In_Fan_Temp_Setup == 1)
    {   
        if (Check_Nec_Button() == 1)
        {
            Nec_ok = 0;
            if (found >= Play_Pause && found <= EQ)
            {
                Do_Beep_Good(); 
                if (found == Play_Pause) Do_Save_New_Fan_Temp();
				// Handle 'Plus', 'Minus', 'EQ' buttons
                if (found == EQ) Exit_Setup_Fan_Temp();
                if (found == Plus) Increase_Fan_Temp();
                if (found == Minus) Decrease_Fan_Temp();

                found = 0xff;
            }
            else Do_Beep_Bad();       
        }                   
    }
}

void  Increase_Fan_Temp()
{
    // Increase setup_fan_set_temp by 1
	// then check if it is greater than 110. If so, limit to 110
    setup_fan_set_temp++;
    if(setup_fan_set_temp > 30)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(duty_cycle);
        setup_fan_set_temp = 30;
        printf("*Maximum fan temp reached\r\n\n");
    }
    Update_Setup_Fan_Screen();              		// Update screen with latest info
}

void  Decrease_Fan_Temp()        
{   
    // Decrease setup_fan_set_temp by 1
	// Then check if it is less than 50. If so, limit to 50
    setup_fan_set_temp--;
    if(setup_fan_set_temp < 10) 
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(duty_cycle);
        setup_fan_set_temp = 10;
        printf("*Minimum fan temp reached\r\n\n");
    }  
    Update_Setup_Fan_Screen();                      // Update screen with latest info
}

void Exit_Setup_Fan_Temp()
{
    DS3231_Read_Time();                                 // Read time
    Initialize_Screen();                                // Initialize the screen before returning
    In_Fan_Temp_Setup = 0;                                 
    printf("*Exit without saving fan temp\r\n\n");
}

void Do_Save_New_Fan_Temp()
{
    fan_set_temp = setup_fan_set_temp;
    DS3231_Read_Time();                                 // Read time
    Initialize_Screen();                                // Initialize the screen before returning  
    In_Fan_Temp_Setup = 0;   
    printf("*Saving fan temp\r\n\n");
}
    
void Initialize_Setup_Fan_Screen(void) 
{ 
    fillScreen(ST7735_BLACK);                           // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L");         // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);   
    // X and Y coordinates of where the text is to be displayed
    
    strcpy(txt, "FinSP24/2/08");
    drawtext(start_x + 55 , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
    
    strcpy(txt, " Fan Setup\0"); 
    drawtext(start_x , start_y+25, txt, ST7735_YELLOW, ST7735_BLACK, TS_2);     
                         
    strcpy(txt, "  Set Fan Temp");
    drawtext(setup_fan_x , setup_fan_y, txt, ST7735_CYAN  , ST7735_BLACK, TS_1);
}
    
void Update_Setup_Fan_Screen(void)
{
    char dc_char1 = setup_fan_set_temp/10;
    char dc_char2 = setup_fan_set_temp%10;   // Temp ones place

    setup_fan_set_text[0] = dc_char1 + '0';
    setup_fan_set_text[1] = dc_char2 + '0';
    drawtext(setup_data_fan_x, setup_data_fan_y ,setup_fan_set_text, ST7735_RED, ST7735_BLACK, TS_2);
}