#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Main.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"
#include "Fan_Support.h"

extern char buffer[];
extern char *nbr;
extern char *txt;
extern char tempC_Txt[];
extern char tempF_Txt[];
extern char time_Txt[];
extern char date_Txt[];
extern char alarm_time_Txt[];
extern char Alarm_SW_Txt[];
extern char Fan_Set_Temp_Txt[];
extern char Fan_Mode_Txt[];
extern char Fan_SW_Txt[];
 
extern char DC_Txt[];
extern char Volt_Txt[];
extern char RPM_Txt[];
extern char RTC_ALARM_Txt[];

extern signed char tempC, tempF;
extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
extern unsigned char fan_set_temp;
extern char duty_cycle;
extern int Alarm_SW;

extern int rpm;
extern float volt;

void Initialize_Screen(void) 
{ 
    LCD_Reset();                                    // Screen reset
    TFT_GreenTab_Initialize();         
    fillScreen(ST7735_BLACK);                       // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L");         // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);   
    // X and Y coordinates of where the text is to be displayed
    
    strcpy(txt, "FinSP24/2/08");
    drawtext(start_x + 55 , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);   
    
    strcpy(txt, "Temperature:");
    drawtext(temp_x  , temp_y , txt, ST7735_MAGENTA, ST7735_BLACK, TS_1);                                                                                               // including text color and the background of it
    drawCircle(circ_x, circ_y , 2  , ST7735_YELLOW);
    strcpy(txt, "C/");
    drawtext(tempc_x , tempc_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2); 
    strcpy(txt, "F");         
    drawCircle(cirf_x, cirf_y , 2  , ST7735_YELLOW);
    drawtext(tempf_x , tempf_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2);
    strcpy(txt, "Time");
    drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);
    strcpy(txt, "Date");
    drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);
    strcpy(txt, "Alarm Time");
    drawtext(alarm_time_x  , alarm_time_y , txt, ST7735_YELLOW, ST7735_BLACK, TS_1); 
    strcpy(txt, "Alarm SW");
    drawtext(alarm_sw_x, alarm_sw_y, txt, ST7735_YELLOW, ST7735_BLACK, TS_1);    
    strcpy(txt, "FAN SetTemp");
    drawtext(fan_set_temp_x, fan_set_temp_y, txt, ST7735_BLUE  , ST7735_BLACK, TS_1); 
    strcpy(txt, "Mode");
    drawtext(fan_mode_x, fan_mode_y, txt, ST7735_BLUE  , ST7735_BLACK, TS_1);
    strcpy(txt, "SW");
    drawtext(fan_sw_x, fan_sw_y, txt, ST7735_BLUE  , ST7735_BLACK, TS_1);
    strcpy(txt, "DC");
    drawtext(dc_x, dc_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
    strcpy(txt, "RM");
    drawtext(rtc_x, rtc_y, txt, ST7735_WHITE       , ST7735_BLACK  , TS_1);
    strcpy(txt, "Volt");
    drawtext(volt_x, volt_y, txt, ST7735_WHITE       , ST7735_BLACK  , TS_1);
    strcpy(txt, "RPM");
    drawtext(rpm_x, rpm_y, txt, ST7735_WHITE       , ST7735_BLACK  , TS_1);   
}

void Update_Screen(void)
{
    if ((tempC & 0x80) == 0x80) 
    {
        tempC_Txt[0] = '-';
        int neg_tempC = -tempC;
        printf ("Temp = %d\r\n", tempC);
        tempC_Txt[1]  = (neg_tempC & 0x7f)/10  + '0';   // Tens digit of C
        tempC_Txt[2]  = (neg_tempC & 0x7f)%10  + '0';   // Ones digit of C
    }
    else
    {
        tempC_Txt[0] = '+';
        tempC_Txt[1]  = tempC/10  + '0';                // Tens digit of C
        tempC_Txt[2]  = tempC%10  + '0';                // Ones digit of C
    }
    
    if (tempF < 0) 
    {
        tempF_Txt[0] = '-';
        int neg_tempF = -tempF;
        tempF_Txt[1]  = neg_tempF/10  + '0';            // Tens digit of C
        tempF_Txt[2]  = neg_tempF%10  + '0';            // Ones digit of C
    }
    else
    {
        tempF_Txt[0] = '+';
        tempF_Txt[1]  = tempF/10  + '0';                // Tens digit of C
        tempF_Txt[2]  = tempF%10  + '0';                // Ones digit of C
    }    
    time_Txt[0]  = (hour>>4)  + '0';                    // Hour MSD
    time_Txt[1]  = (hour & 0x0f)+ '0';                  // Hour LSD
    time_Txt[3]  = (minute>>4)  + '0';                  // Minute MSD
    time_Txt[4]  = (minute & 0x0f)+ '0';                // Minute LSD
    time_Txt[6]  = (second>>4)  + '0';                  // Second MSD
    time_Txt[7]  = (second & 0x0f)+ '0';                // Second LSD
    date_Txt[0]  = (month>>4)  + '0';                   // Month MSD
    date_Txt[1]  = (month & 0x0f)+ '0';                 // Month LSD
    date_Txt[3]  = (day>>4)  + '0';                     // Day MSD
    date_Txt[4]  = (day & 0x0f)+ '0';                   // Day LSD
    date_Txt[6]  = (year>>4)  + '0';                    // Year MSD
    date_Txt[7]  = (year & 0x0f)+ '0';                  // Year LSD
    alarm_time_Txt[0]  = (alarm_hour>>4)  + '0';        // Alarm Hour MSD
    alarm_time_Txt[1]  = (alarm_hour & 0x0f)+ '0';      // Alarm Hour LSD
    alarm_time_Txt[3]  = (alarm_minute>>4)  + '0';      // Alarm Minute MSD
    alarm_time_Txt[4]  = (alarm_minute & 0x0f)+ '0';    // Alarm Minute LSD
    alarm_time_Txt[6]  = (alarm_second>>4)  + '0';      // Alarm Second MSD
    alarm_time_Txt[7]  = (alarm_second & 0x0f)+ '0';    // Alarm Second LSD
    
    if (Alarm_SW == 1) 
    {   
        strcpy(Alarm_SW_Txt, "ON ");
    }
    else 
    {
        strcpy(Alarm_SW_Txt, "OFF");
    }
 
    Fan_Set_Temp_Txt[0] = fan_set_temp/100 + '0';
    Fan_Set_Temp_Txt[1] = (fan_set_temp / 10) % 10  + '0';  // Fan Set Temp tens place
    Fan_Set_Temp_Txt[2] = fan_set_temp % 10  + '0';         // Fan Set Temp ones place
    
    if (FAN_MODE == 1)
    {
        strcpy(Fan_Mode_Txt, "AUTO");
    }
    else 
    {
        strcpy(Fan_Mode_Txt, "MANU");
    }
    
    if (FAN_EN == 1)
    {
        strcpy(Fan_SW_Txt, "ON ");
    }
    else 
    {
        strcpy(Fan_SW_Txt, "OFF");
    }
    
    if (RTC_ALARM_NOT == 0) RTC_ALARM_Txt[0] = '1';
    else RTC_ALARM_Txt[0] = '0';
       
    
    DC_Txt[0] = duty_cycle/100  + '0';
    DC_Txt[1] = (duty_cycle / 10) % 10  + '0';  // DC tens place
    DC_Txt[2] = duty_cycle % 10  + '0';         // DC ones place      
    
    Volt_Txt[0] = ((int)  volt        ) + '0';
    Volt_Txt[2] = ((int) (volt*10 )%10) + '0';
    Volt_Txt[3] = ((int) (volt*100)%10) + '0';
    
    RPM_Txt[0] = rpm/1000  + '0';
    RPM_Txt[1] = (rpm/100) % 10  + '0';         // RPM hundreds place
    RPM_Txt[2] = (rpm/10) % 10  + '0';          // RPM tens place  
    RPM_Txt[3] = rpm % 10  + '0';               // PRM ones place         
    
    drawtext(data_tempc_x, data_tempc_y, tempC_Txt , ST7735_YELLOW , ST7735_BLACK , TS_2);       
    drawtext(data_tempf_x, data_tempf_y, tempF_Txt , ST7735_YELLOW , ST7735_BLACK , TS_2);
    drawtext(data_time_x , data_time_y , time_Txt  , ST7735_CYAN   , ST7735_BLACK , TS_2);
    drawtext(data_date_x , data_date_y , date_Txt  , ST7735_GREEN  , ST7735_BLACK , TS_2);
    drawtext(data_alarm_time_x , data_alarm_time_y , alarm_time_Txt  , ST7735_CYAN   , ST7735_BLACK , TS_1); 
    drawtext(data_alarm_sw_x , data_alarm_sw_y , Alarm_SW_Txt  , ST7735_CYAN   , ST7735_BLACK , TS_1); 
    drawtext(data_fan_set_temp_x, data_fan_set_temp_y ,Fan_Set_Temp_Txt, ST7735_RED, ST7735_BLACK, TS_1);
    drawtext(data_fan_mode_x, data_fan_mode_y, Fan_Mode_Txt, ST7735_RED    , ST7735_BLACK , TS_1); 
    drawtext(data_fan_sw_x, data_fan_sw_y, Fan_SW_Txt, ST7735_RED    , ST7735_BLACK , TS_1);    
    drawtext(data_dc_x, data_dc_y, DC_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);
    drawtext(data_rtc_x, data_rtc_y, RTC_ALARM_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);    
    drawtext(data_volt_x, data_volt_y, Volt_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);    
    drawtext(data_rpm_x, data_rpm_y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);
}