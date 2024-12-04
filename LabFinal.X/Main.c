// ECE3301L Spring 2024 Session 2 Table 8 Lab Final
// Ethan McKendell and Christopher Valdivia
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "Main_Screen.h"
#include "Setup_Alarm_Time.h"
#include "Setup_Fan_Temp.h"
#include "Setup_Time.h"
#include "ST7735_TFT.h"
#include "utils.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF

#define OFF 0b00000000
#define RED 0b00000001
#define GREEN 0b00000010
#define YELLOW 0b00000011
#define BLUE 0b00000100
#define PURPLE 0b00000101
#define CYAN 0b00000110
#define WHITE 0b00000111

void Set_DC_RGB(int duty_cycle);
void Set_RPM_RGB(int rpm);
void Set_ALARM_RGB(void);

void Test_Alarm();

int colors[8] = {OFF, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE};

char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char tempSecond = 0xff; 
signed char tempC, tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

unsigned char fan_set_temp = 26;

float volt;
char INT0_flag, INT1_flag, INT2_flag;

char found;
char Nec_Button;
unsigned char Nec_state = 0;
short Nec_ok = 0;

char duty_cycle;
int rps, rpm;
int Alarm_SW, MATCHED, Alarm_Mode;
int i = 0;

char buffer[31]     = "ECE3301L FinSSyy/s/tt\0";
char *nbr;
char *txt;
char tempC_Txt[]        = "+25";
char tempF_Txt[]        = "+77";
char time_Txt[]         = "00:00:00";
char date_Txt[]         = "00/00/00";
char alarm_time_Txt[]   = "00:00:00";
char Alarm_SW_Txt[]     = "OFF";
char Fan_Set_Temp_Txt[] = "16C";
char Fan_Mode_Txt[]     = "MANU";              // text storage for Fan Mode
char Fan_SW_Txt[]       = "OFF";                // text storage for Fan On/Off
char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};
 
char DC_Txt[]       = "000";                // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";
char RTC_ALARM_Txt[]= "0";                  //
char RPM_Txt[]      = "0000";               // text storage for RPM

char setup_time[]           = "00:00:00";
char setup_date[]           = "01/01/00";
char setup_alarm_time[]     = "00:00:00"; 
char setup_fan_set_text[]   = "16C";

void Do_Init()                      // Initialize the ports 
{ 
    Init_UART();                    // Initialize the UART
    Init_ADC();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    TRISA = 0x01;
    TRISB = 0x07;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;

    RBPU=0;

    I2C_Init(100000); 

    DS1621_Init();
    Init_Interrupt();
    
    //Fan Setup
    FAN_EN = 0;
    FAN_MODE = 0;
    duty_cycle = 0;
    //fan_set_temp = 26;
    do_update_pwm(duty_cycle);
}


void main() 
{
    Do_Init();                                                  // Initialization  
    Initialize_Screen();

    TMR3L = 0x00;                   
    T3CON = 0x03;
    DS3231_Turn_Off_Alarm();                                    
    DS3231_Read_Alarm_Time();                                   // Read alarm time for the first time

    tempSecond = 0xff;
    
    while (1)
    {
        DS3231_Read_Time();

        if(tempSecond != second)
        {
            tempSecond = second;
            
            rpm = get_RPM();
            volt = Read_Volt(0);
            tempC = DS1621_Read_Temp();
            tempF = (tempC * 9 / 5) + 32;
            
            Monitor_Fan();
            Test_Alarm();
            Update_Screen();
            
            Set_DC_RGB(duty_cycle);
            Set_RPM_RGB(rpm);
            
            printf ("%02x:%02x:%02x %02x/%02x/%02x\r\n",hour,minute,second,month,day,year);
            printf ("Voltage = %f\r\n", volt);
            printf ("Temp = %dF = %dC\r\n", tempF, tempC);
            printf ("RTC_ALARM_NOT = %d MATCHED = %d SW = %d\r\n", RTC_ALARM_NOT, Alarm_Mode, Alarm_SW);
            printf ("RPM = %d DC = %d\r\n\n", rpm, duty_cycle);
        }
        
        if (Check_Nec_Button() == 1) // IR Receiver Output
        {
            Nec_ok = 0;
            switch (found)
            {
                case Ch_Minus:        
                    Do_Beep_Good();
                    printf("=== SETUP TIME ===\r\n");
                    Do_Setup_Time();
                    break;
                
                case Channel:           
                    Do_Beep_Good();
                    printf("=== SETUP ALARM TIME ===\r\n");
                    Do_Setup_Alarm_Time();
                    break;
                    
                case Ch_Plus:
                    Do_Beep_Good();
                    printf("=== SETUP FAN TEMP ===\r\n");
                    Setup_Temp_Fan();           
                    break;
                    
                case Play_Pause:
                    Do_Beep_Good();
                    Toggle_Fan();
                    printf("*Toggling Fan\r\n\n");
                    break;           

                case Minus: 
                    Do_Beep_Good();                   
                    Decrease_Duty_Cycle();
                    break;
                    
                case Plus:
                    Do_Beep_Good();                    
                    Increase_Duty_Cycle();
                    break;
                    
                case Zero_Button:
                    Do_Beep_Good();
                    FAN_MODE = !FAN_MODE;
                    if (FAN_MODE == 1) printf("*Enabling FAN_MODE\r\n\n");
                    else printf("*Disabling FAN_MODE\r\n\n");
                    
                    break;
                    
                default:
                    Do_Beep_Bad();
                    break;
            }
        }
		
		if (INT2_flag == 1) // Alarm_SW
        {
            INT2_flag = 0;
            Alarm_SW = !Alarm_SW;
        }
    }
}

void Set_DC_RGB(int duty_cycle)
{
    if (duty_cycle >= 70) PORTE = colors[7];
    else PORTE = colors[duty_cycle / 10];
}

void Set_RPM_RGB(int rpm)
{
    if (rpm == 0) PORTB = (PORTB & 0x01) | ((colors[0] << 3) & 0xFE);
    else if (rpm >= 3000) PORTB = (PORTB & 0x01) | ((colors[7] << 3) & 0xFE);
    else PORTB = (PORTB & 0x01) | ((colors[(rpm / 500) + 1] << 3) & 0xFE);
}

void Set_ALARM_RGB()
{
    PORTD = (PORTD & 0x8F) | ((colors[i] << 4) & 0x70);
    i++;
    if (i > 7) i = 0;
}

void Test_Alarm()
{
	if ((Alarm_SW == 1) && (Alarm_Mode == 1) && (volt < 2.5)) // RTC_MATCH#
    {
        Activate_Buzzer_4KHz();
        //DS3231_Turn_On_Alarm(); //Does this even work???
        Set_ALARM_RGB();
    }
    else
    {
        Deactivate_Buzzer();
        DS3231_Turn_Off_Alarm();
        Alarm_Mode = 0;
        PORTD = (PORTD & 0x8F) | ((0x00 << 4) & 0x70);
    }
}